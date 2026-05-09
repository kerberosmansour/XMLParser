#include "parser_core.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace xmlparser::v1::detail {
namespace {

struct DecodedChar {
  char32_t value;
  SourceLocation location;
};

struct DecodedDocument {
  std::vector<DecodedChar> chars;
  SourceLocation end_location;
};

struct RawAttribute {
  std::string name;
  std::string value;
  SourceLocation location;
};

enum class ElementModelKind {
  Any,
  Empty,
  Pcdata,
  Children,
};

struct ElementDeclaration {
  ElementModelKind kind = ElementModelKind::Any;
  std::vector<std::string> children;
};

struct DtdState {
  bool has_doctype = false;
  bool has_external_subset = false;
  std::string root_name;
  std::string external_system_id;
  std::map<std::string, ElementDeclaration> elements;
  std::map<std::string, std::string> entities;
  std::size_t declaration_count = 0;
};

struct ValidationFrame {
  std::string name;
  std::vector<std::string> child_elements;
  bool has_non_whitespace_text = false;
};

bool is_space(char32_t value) {
  return value == U' ' || value == U'\t' || value == U'\n' || value == U'\r';
}

bool is_name_start(char32_t value) {
  return (value >= U'A' && value <= U'Z') || (value >= U'a' && value <= U'z') ||
         value == U'_' || value == U':';
}

bool is_name_char(char32_t value) {
  return is_name_start(value) || (value >= U'0' && value <= U'9') ||
         value == U'-' || value == U'.';
}

bool is_valid_xml10_char(char32_t value) {
  return value == U'\t' || value == U'\n' || value == U'\r' ||
         (value >= 0x20 && value <= 0xD7FF) ||
         (value >= 0xE000 && value <= 0xFFFD) ||
         (value >= 0x10000 && value <= 0x10FFFF);
}

bool is_valid_xml11_char(char32_t value) {
  return (value >= 0x1 && value <= 0xD7FF) ||
         (value >= 0xE000 && value <= 0xFFFD) ||
         (value >= 0x10000 && value <= 0x10FFFF);
}

bool is_valid_xml_char(char32_t value, XmlVersion version) {
  if (version == XmlVersion::Xml11) {
    return is_valid_xml11_char(value);
  }
  return is_valid_xml10_char(value);
}

std::string to_ascii(std::u32string_view text) {
  std::string result;
  result.reserve(text.size());
  for (char32_t value : text) {
    result.push_back(value <= 0x7F ? static_cast<char>(value) : '?');
  }
  return result;
}

std::string lowercase_ascii(std::string value) {
  for (char& ch : value) {
    if (ch >= 'A' && ch <= 'Z') {
      ch = static_cast<char>(ch - 'A' + 'a');
    }
  }
  return value;
}

std::string strip_ascii_space(std::string value) {
  value.erase(std::remove_if(value.begin(), value.end(),
                             [](char ch) {
                               return ch == ' ' || ch == '\t' || ch == '\n' ||
                                      ch == '\r';
                             }),
              value.end());
  return value;
}

std::vector<std::string> split_names(std::string_view text) {
  std::vector<std::string> names;
  std::string current;
  for (char ch : text) {
    if (ch == ',') {
      if (!current.empty()) {
        names.push_back(current);
        current.clear();
      }
      continue;
    }
    if (ch == '?' || ch == '*' || ch == '+') {
      continue;
    }
    current.push_back(ch);
  }
  if (!current.empty()) {
    names.push_back(current);
  }
  return names;
}

void append_utf8(std::string& output, char32_t value) {
  if (value <= 0x7F) {
    output.push_back(static_cast<char>(value));
  } else if (value <= 0x7FF) {
    output.push_back(static_cast<char>(0xC0 | (value >> 6)));
    output.push_back(static_cast<char>(0x80 | (value & 0x3F)));
  } else if (value <= 0xFFFF) {
    output.push_back(static_cast<char>(0xE0 | (value >> 12)));
    output.push_back(static_cast<char>(0x80 | ((value >> 6) & 0x3F)));
    output.push_back(static_cast<char>(0x80 | (value & 0x3F)));
  } else {
    output.push_back(static_cast<char>(0xF0 | (value >> 18)));
    output.push_back(static_cast<char>(0x80 | ((value >> 12) & 0x3F)));
    output.push_back(static_cast<char>(0x80 | ((value >> 6) & 0x3F)));
    output.push_back(static_cast<char>(0x80 | (value & 0x3F)));
  }
}

class LocationTracker {
 public:
  SourceLocation current(std::size_t byte_offset) const {
    return SourceLocation{line_, column_, byte_offset};
  }

  void advance(char32_t value) {
    if (value == U'\n') {
      ++line_;
      column_ = 1;
    } else {
      ++column_;
    }
  }

 private:
  std::size_t line_ = 1;
  std::size_t column_ = 1;
};

[[noreturn]] void throw_parse(ErrorKind kind,
                              SourceLocation location,
                              std::string message) {
  throw XmlParseException(kind, location, std::move(message));
}

DecodedDocument decode_utf8(std::string_view xml,
                            std::size_t start_offset,
                            const ParserOptions& options) {
  DecodedDocument document;
  LocationTracker tracker;
  std::size_t offset = start_offset;

  while (offset < xml.size()) {
    const SourceLocation location = tracker.current(offset);
    const auto byte = static_cast<unsigned char>(xml[offset]);
    char32_t value = 0;
    std::size_t width = 0;

    if (byte < 0x80) {
      value = byte;
      width = 1;
    } else if ((byte & 0xE0) == 0xC0) {
      value = byte & 0x1F;
      width = 2;
      if (value == 0) {
        throw_parse(ErrorKind::Encoding, location, "invalid UTF-8 sequence");
      }
    } else if ((byte & 0xF0) == 0xE0) {
      value = byte & 0x0F;
      width = 3;
    } else if ((byte & 0xF8) == 0xF0) {
      value = byte & 0x07;
      width = 4;
    } else {
      throw_parse(ErrorKind::Encoding, location, "invalid UTF-8 sequence");
    }

    if (offset + width > xml.size()) {
      throw_parse(ErrorKind::Encoding, location, "truncated UTF-8 sequence");
    }

    for (std::size_t index = 1; index < width; ++index) {
      const auto continuation = static_cast<unsigned char>(xml[offset + index]);
      if ((continuation & 0xC0) != 0x80) {
        throw_parse(ErrorKind::Encoding, location, "invalid UTF-8 continuation");
      }
      value = (value << 6) | (continuation & 0x3F);
    }

    if ((width == 2 && value < 0x80) || (width == 3 && value < 0x800) ||
        (width == 4 && value < 0x10000) || value > 0x10FFFF ||
        (value >= 0xD800 && value <= 0xDFFF)) {
      throw_parse(ErrorKind::Encoding, location, "invalid UTF-8 scalar value");
    }
    if (!is_valid_xml_char(value, options.version)) {
      throw_parse(ErrorKind::WellFormedness, location,
                  "invalid XML character");
    }

    document.chars.push_back({value, location});
    tracker.advance(value);
    offset += width;
  }

  document.end_location = tracker.current(xml.size());
  return document;
}

DecodedDocument decode_utf16(std::string_view xml,
                             bool little_endian,
                             const ParserOptions& options) {
  DecodedDocument document;
  LocationTracker tracker;
  std::size_t offset = 2;

  auto read_unit = [&](std::size_t unit_offset) -> std::uint16_t {
    const auto first = static_cast<unsigned char>(xml[unit_offset]);
    const auto second = static_cast<unsigned char>(xml[unit_offset + 1]);
    if (little_endian) {
      return static_cast<std::uint16_t>(first | (second << 8));
    }
    return static_cast<std::uint16_t>((first << 8) | second);
  };

  while (offset < xml.size()) {
    const SourceLocation location = tracker.current(offset);
    if (offset + 1 >= xml.size()) {
      throw_parse(ErrorKind::Encoding, location, "truncated UTF-16 code unit");
    }

    const std::uint16_t unit = read_unit(offset);
    char32_t value = unit;
    std::size_t width = 2;

    if (unit >= 0xD800 && unit <= 0xDBFF) {
      if (offset + 3 >= xml.size()) {
        throw_parse(ErrorKind::Encoding, location,
                    "truncated UTF-16 surrogate pair");
      }
      const std::uint16_t trail = read_unit(offset + 2);
      if (trail < 0xDC00 || trail > 0xDFFF) {
        throw_parse(ErrorKind::Encoding, location,
                    "invalid UTF-16 surrogate pair");
      }
      value = 0x10000 + (((unit - 0xD800) << 10) | (trail - 0xDC00));
      width = 4;
    } else if (unit >= 0xDC00 && unit <= 0xDFFF) {
      throw_parse(ErrorKind::Encoding, location,
                  "unexpected UTF-16 trailing surrogate");
    }

    if (!is_valid_xml_char(value, options.version)) {
      throw_parse(ErrorKind::WellFormedness, location,
                  "invalid XML character");
    }

    document.chars.push_back({value, location});
    tracker.advance(value);
    offset += width;
  }

  document.end_location = tracker.current(xml.size());
  return document;
}

DecodedDocument decode_document(std::string_view xml, const ParserOptions& options) {
  if (xml.empty()) {
    throw_parse(ErrorKind::EmptyInput, SourceLocation{}, "XML input is empty");
  }
  if (xml.size() > options.max_document_bytes) {
    throw_parse(ErrorKind::ResourceLimit, SourceLocation{},
                "XML input exceeds max_document_bytes");
  }
  if (xml.size() >= 3 &&
      static_cast<unsigned char>(xml[0]) == 0xEF &&
      static_cast<unsigned char>(xml[1]) == 0xBB &&
      static_cast<unsigned char>(xml[2]) == 0xBF) {
    DecodedDocument decoded = decode_utf8(xml, 3, options);
    if (decoded.chars.empty()) {
      throw_parse(ErrorKind::EmptyInput, SourceLocation{1, 1, 3},
                  "XML input is empty");
    }
    return decoded;
  }
  if (xml.size() >= 2 &&
      static_cast<unsigned char>(xml[0]) == 0xFF &&
      static_cast<unsigned char>(xml[1]) == 0xFE) {
    DecodedDocument decoded = decode_utf16(xml, true, options);
    if (decoded.chars.empty()) {
      throw_parse(ErrorKind::EmptyInput, SourceLocation{1, 1, 2},
                  "XML input is empty");
    }
    return decoded;
  }
  if (xml.size() >= 2 &&
      static_cast<unsigned char>(xml[0]) == 0xFE &&
      static_cast<unsigned char>(xml[1]) == 0xFF) {
    DecodedDocument decoded = decode_utf16(xml, false, options);
    if (decoded.chars.empty()) {
      throw_parse(ErrorKind::EmptyInput, SourceLocation{1, 1, 2},
                  "XML input is empty");
    }
    return decoded;
  }
  DecodedDocument decoded = decode_utf8(xml, 0, options);
  if (decoded.chars.empty()) {
    throw_parse(ErrorKind::EmptyInput, SourceLocation{}, "XML input is empty");
  }
  return decoded;
}

class Parser {
 public:
  Parser(std::string_view xml, SaxHandler& handler, const ParserOptions& options)
      : document_(decode_document(xml, options)), handler_(handler), options_(options) {}

  void parse() {
    handler_.start_document();
    parse_xml_declaration_if_present();
    skip_misc();
    parse_doctype_if_present();
    skip_misc();
    parse_element();
    skip_misc();
    if (!eof()) {
      fail(ErrorKind::WellFormedness, "extra content after document element");
    }
    handler_.end_document();
  }

 private:
  bool eof() const {
    return index_ >= document_.chars.size();
  }

  char32_t peek(std::size_t lookahead = 0) const {
    if (index_ + lookahead >= document_.chars.size()) {
      return U'\0';
    }
    return document_.chars[index_ + lookahead].value;
  }

  SourceLocation location() const {
    if (index_ < document_.chars.size()) {
      return document_.chars[index_].location;
    }
    return document_.end_location;
  }

  SourceLocation previous_location() const {
    if (index_ == 0 || document_.chars.empty()) {
      return SourceLocation{};
    }
    return document_.chars[index_ - 1].location;
  }

  [[noreturn]] void fail(ErrorKind kind, std::string message) const {
    throw_parse(kind, location(), std::move(message));
  }

  [[noreturn]] void fail_at(ErrorKind kind,
                            SourceLocation source_location,
                            std::string message) const {
    throw_parse(kind, source_location, std::move(message));
  }

  [[noreturn]] void validity_error(SourceLocation source_location,
                                   std::string message) const {
    throw_parse(ErrorKind::Validity, source_location, std::move(message));
  }

  void recoverable_validity_error(SourceLocation source_location,
                                  std::string message) const {
    if (!options_.recoverable_error_handler) {
      throw_parse(ErrorKind::Validity, source_location, std::move(message));
    }
    const XmlParseException error(ErrorKind::Validity, source_location,
                                  std::move(message));
    options_.recoverable_error_handler(error);
  }

  bool starts_with(std::u32string_view text) const {
    if (index_ + text.size() > document_.chars.size()) {
      return false;
    }
    for (std::size_t offset = 0; offset < text.size(); ++offset) {
      if (document_.chars[index_ + offset].value != text[offset]) {
        return false;
      }
    }
    return true;
  }

  void advance() {
    if (eof()) {
      fail(ErrorKind::WellFormedness, "unexpected end of XML input");
    }
    ++index_;
  }

  void expect(char32_t value, std::string_view description) {
    if (peek() != value) {
      fail(ErrorKind::WellFormedness, "expected " + std::string(description));
    }
    advance();
  }

  void expect_sequence(std::u32string_view text, std::string_view description) {
    if (!starts_with(text)) {
      fail(ErrorKind::WellFormedness, "expected " + std::string(description));
    }
    index_ += text.size();
  }

  void enforce_token_size(std::size_t size, SourceLocation start) const {
    if (size > options_.max_token_bytes) {
      fail_at(ErrorKind::ResourceLimit, start, "XML token exceeds max_token_bytes");
    }
  }

  void skip_whitespace() {
    while (!eof() && is_space(peek())) {
      advance();
    }
  }

  void skip_misc() {
    bool consumed = true;
    while (consumed) {
      consumed = false;
      while (!eof() && is_space(peek())) {
        advance();
        consumed = true;
      }
      if (starts_with(U"<!--")) {
        parse_comment(&handler_);
        consumed = true;
      } else if (starts_with(U"<?")) {
        parse_processing_instruction(&handler_);
        consumed = true;
      }
    }
  }

  void parse_xml_declaration_if_present() {
    if (!starts_with(U"<?xml") || !is_space(peek(5))) {
      return;
    }

    const SourceLocation start = location();
    expect_sequence(U"<?xml", "XML declaration");
    if (!is_space(peek())) {
      fail(ErrorKind::WellFormedness, "invalid XML declaration");
    }
    std::string declaration;
    while (!eof() && !starts_with(U"?>")) {
      append_utf8(declaration, peek());
      enforce_token_size(declaration.size(), start);
      advance();
    }
    expect_sequence(U"?>", "XML declaration close");
    const bool declares_xml10 = declaration.find("1.0") != std::string::npos;
    const bool declares_xml11 = declaration.find("1.1") != std::string::npos;
    if (declaration.find("version") == std::string::npos ||
        (!declares_xml10 && !declares_xml11)) {
      fail_at(ErrorKind::WellFormedness, start,
              "XML declaration must declare version 1.0 or 1.1");
    }
    if ((declares_xml10 && options_.version != XmlVersion::Xml10) ||
        (declares_xml11 && options_.version != XmlVersion::Xml11)) {
      fail_at(ErrorKind::WellFormedness, start,
              "XML declaration version does not match ParserOptions");
    }
  }

  std::string parse_name() {
    const SourceLocation start = location();
    if (!is_name_start(peek())) {
      fail(ErrorKind::WellFormedness, "expected XML name");
    }

    std::u32string buffer;
    while (!eof() && is_name_char(peek())) {
      buffer.push_back(peek());
      enforce_token_size(buffer.size(), start);
      advance();
    }
    return to_ascii(buffer);
  }

  std::string parse_quoted_literal() {
    const char32_t quote = peek();
    if (quote != U'\'' && quote != U'"') {
      fail(ErrorKind::WellFormedness, "expected quoted literal");
    }
    advance();
    std::string value;
    while (!eof() && peek() != quote) {
      append_utf8(value, peek());
      advance();
    }
    expect(quote, "literal quote");
    return value;
  }

  void parse_doctype_if_present() {
    if (!starts_with(U"<!DOCTYPE")) {
      return;
    }

    dtd_.has_doctype = true;
    expect_sequence(U"<!DOCTYPE", "DOCTYPE declaration");
    if (!is_space(peek())) {
      fail(ErrorKind::WellFormedness, "DOCTYPE requires whitespace after keyword");
    }
    skip_whitespace();
    dtd_.root_name = parse_name();
    skip_whitespace();

    if (starts_with(U"SYSTEM")) {
      index_ += 6;
      skip_whitespace();
      dtd_.has_external_subset = true;
      dtd_.external_system_id = parse_quoted_literal();
      skip_whitespace();
    } else if (starts_with(U"PUBLIC")) {
      index_ += 6;
      skip_whitespace();
      (void)parse_quoted_literal();
      skip_whitespace();
      dtd_.has_external_subset = true;
      dtd_.external_system_id = parse_quoted_literal();
      skip_whitespace();
    }

    if (peek() == U'[') {
      advance();
      parse_internal_subset();
      expect(U']', "DOCTYPE internal subset close");
      skip_whitespace();
    }

    expect(U'>', "DOCTYPE close");
    resolve_external_subset_if_needed();
  }

  void parse_internal_subset() {
    while (!eof()) {
      skip_whitespace();
      if (peek() == U']') {
        return;
      }
      if (starts_with(U"<!--")) {
        parse_comment(nullptr);
      } else if (starts_with(U"<!ELEMENT")) {
        parse_element_declaration();
      } else if (starts_with(U"<!ENTITY")) {
        parse_entity_declaration();
      } else {
        fail(ErrorKind::WellFormedness, "unsupported DTD declaration");
      }
    }
    fail(ErrorKind::WellFormedness, "truncated DOCTYPE internal subset");
  }

  void enforce_dtd_declaration_limit() {
    ++dtd_.declaration_count;
    if (dtd_.declaration_count > options_.max_dtd_declarations) {
      fail(ErrorKind::ResourceLimit,
           "DTD declaration count exceeds max_dtd_declarations");
    }
  }

  void parse_element_declaration() {
    const SourceLocation start = location();
    enforce_dtd_declaration_limit();
    expect_sequence(U"<!ELEMENT", "ELEMENT declaration");
    if (!is_space(peek())) {
      fail(ErrorKind::WellFormedness, "ELEMENT declaration requires whitespace");
    }
    skip_whitespace();
    const std::string name = parse_name();
    skip_whitespace();

    std::string model;
    while (!eof() && peek() != U'>') {
      append_utf8(model, peek());
      advance();
    }
    expect(U'>', "ELEMENT declaration close");

    if (dtd_.elements.find(name) != dtd_.elements.end()) {
      validity_error(start, "duplicate element declaration");
    }
    dtd_.elements.emplace(name, parse_element_model(model, start));
  }

  ElementDeclaration parse_element_model(std::string model,
                                         SourceLocation source_location) const {
    model = strip_ascii_space(std::move(model));
    if (model == "ANY") {
      return {ElementModelKind::Any, {}};
    }
    if (model == "EMPTY") {
      return {ElementModelKind::Empty, {}};
    }
    if (model == "(#PCDATA)") {
      return {ElementModelKind::Pcdata, {}};
    }
    if (model.size() >= 2 && model.front() == '(' && model.back() == ')') {
      std::string_view body(model.data() + 1, model.size() - 2);
      const auto children = split_names(body);
      if (!children.empty()) {
        return {ElementModelKind::Children, children};
      }
    }
    throw_parse(ErrorKind::Validity, source_location,
                "unsupported element declaration model");
  }

  void parse_entity_declaration() {
    const SourceLocation start = location();
    enforce_dtd_declaration_limit();
    expect_sequence(U"<!ENTITY", "ENTITY declaration");
    if (!is_space(peek())) {
      fail(ErrorKind::WellFormedness, "ENTITY declaration requires whitespace");
    }
    skip_whitespace();
    if (peek() == U'%') {
      fail(ErrorKind::WellFormedness, "parameter entities are not supported");
    }
    const std::string name = parse_name();
    skip_whitespace();
    const std::string replacement = parse_quoted_literal();
    if (replacement.size() > options_.max_entity_replacement_bytes) {
      fail_at(ErrorKind::ResourceLimit, start,
              "entity replacement exceeds max_entity_replacement_bytes");
    }
    skip_whitespace();
    expect(U'>', "ENTITY declaration close");

    if (dtd_.entities.find(name) != dtd_.entities.end()) {
      validity_error(start, "duplicate entity declaration");
    }
    dtd_.entities.emplace(name, replacement);
  }

  void resolve_external_subset_if_needed() {
    if (!dtd_.has_external_subset || options_.validation != ValidationMode::Dtd) {
      return;
    }
    if (!options_.allow_external_dtd || !options_.external_dtd_resolver) {
      validity_error(location(), "external DTD resolution is disabled");
    }

    const std::string subset =
        options_.external_dtd_resolver(dtd_.external_system_id);
    if (subset.size() > options_.max_external_subset_bytes) {
      fail(ErrorKind::ResourceLimit,
           "external DTD subset exceeds max_external_subset_bytes");
    }
    parse_external_subset_text(subset);
  }

  void parse_external_subset_text(std::string_view subset) {
    std::size_t cursor = 0;
    while (cursor < subset.size()) {
      while (cursor < subset.size() &&
             (subset[cursor] == ' ' || subset[cursor] == '\t' ||
              subset[cursor] == '\n' || subset[cursor] == '\r')) {
        ++cursor;
      }
      if (cursor >= subset.size()) {
        return;
      }
      if (subset.compare(cursor, 9, "<!ELEMENT") == 0) {
        const std::size_t close = subset.find('>', cursor);
        if (close == std::string_view::npos) {
          validity_error(location(), "truncated external element declaration");
        }
        parse_external_element_declaration(subset.substr(cursor, close - cursor + 1));
        cursor = close + 1;
        continue;
      }
      validity_error(location(), "unsupported external DTD declaration");
    }
  }

  void parse_external_element_declaration(std::string_view declaration) {
    enforce_dtd_declaration_limit();
    const std::string prefix = "<!ELEMENT";
    std::size_t cursor = prefix.size();
    while (cursor < declaration.size() && declaration[cursor] == ' ') {
      ++cursor;
    }
    const std::size_t name_start = cursor;
    while (cursor < declaration.size() && declaration[cursor] != ' ') {
      ++cursor;
    }
    const std::string name(declaration.substr(name_start, cursor - name_start));
    while (cursor < declaration.size() && declaration[cursor] == ' ') {
      ++cursor;
    }
    if (declaration.empty() || declaration.back() != '>') {
      validity_error(location(), "invalid external element declaration");
    }
    const std::string model(
        declaration.substr(cursor, declaration.size() - cursor - 1));
    if (dtd_.elements.find(name) != dtd_.elements.end()) {
      validity_error(location(), "duplicate element declaration");
    }
    dtd_.elements.emplace(name, parse_element_model(model, location()));
  }

  void parse_element() {
    const SourceLocation start = location();
    expect(U'<', "'<'");
    if (peek() == U'/' || peek() == U'!' || peek() == U'?') {
      fail_at(ErrorKind::WellFormedness, start, "expected element start tag");
    }

    const std::string name = parse_name();
    const std::size_t depth = element_stack_.size() + 1;
    if (depth > options_.max_depth) {
      fail_at(ErrorKind::ResourceLimit, start, "XML element depth limit exceeded");
    }

    std::vector<RawAttribute> attributes = parse_attributes();
    const std::map<std::string, std::string> namespace_declarations =
        collect_namespace_declarations(attributes);
    namespace_scopes_.push_back(namespace_declarations);
    const QualifiedName element_name = resolve_name(name, true, start);

    std::vector<std::string> attribute_value_storage;
    attribute_value_storage.reserve(attributes.size());
    for (const auto& attribute : attributes) {
      if (!is_namespace_declaration(attribute.name)) {
        attribute_value_storage.push_back(attribute.value);
      }
    }

    std::vector<AttributeView> attribute_views;
    attribute_views.reserve(attribute_value_storage.size());
    std::set<std::pair<std::string, std::string>> expanded_attribute_names;
    std::size_t emitted_attribute_index = 0;
    for (const auto& attribute : attributes) {
      if (is_namespace_declaration(attribute.name)) {
        continue;
      }
      QualifiedName attribute_name = resolve_name(attribute.name, false,
                                                 attribute.location);
      const auto expanded_name =
          std::make_pair(attribute_name.uri, attribute_name.local_name);
      if (!expanded_attribute_names.insert(expanded_name).second) {
        fail_at(ErrorKind::WellFormedness, attribute.location,
                "duplicate expanded attribute name");
      }
      attribute_views.push_back(
          {std::move(attribute_name), attribute_value_storage[emitted_attribute_index],
           attribute.location});
      ++emitted_attribute_index;
    }

    element_stack_.push_back(name);
    element_name_stack_.push_back(element_name);
    validation_stack_.push_back({name, {}, false});
    handler_.start_element(element_name, attribute_views);

    if (starts_with(U"/>")) {
      expect_sequence(U"/>", "empty element close");
      handler_.end_element(element_name);
      finish_validation_frame(start);
      namespace_scopes_.pop_back();
      element_name_stack_.pop_back();
      element_stack_.pop_back();
      return;
    }

    expect(U'>', "'>'");
    parse_content(name);
    finish_validation_frame(start);
    namespace_scopes_.pop_back();
    element_name_stack_.pop_back();
    element_stack_.pop_back();
  }

  void finish_validation_frame(SourceLocation source_location) {
    ValidationFrame frame = validation_stack_.back();
    validation_stack_.pop_back();
    validate_element(frame, source_location);
    if (!validation_stack_.empty()) {
      validation_stack_.back().child_elements.push_back(frame.name);
    }
  }

  void validate_element(const ValidationFrame& frame,
                        SourceLocation source_location) const {
    if (options_.validation != ValidationMode::Dtd || !dtd_.has_doctype) {
      return;
    }
    if (validation_stack_.empty() && frame.name != dtd_.root_name) {
      recoverable_validity_error(source_location,
                                 "document element does not match DOCTYPE");
    }
    const auto declaration = dtd_.elements.find(frame.name);
    if (declaration == dtd_.elements.end()) {
      recoverable_validity_error(source_location, "element has no DTD declaration");
      return;
    }

    switch (declaration->second.kind) {
      case ElementModelKind::Any:
        return;
      case ElementModelKind::Empty:
        if (!frame.child_elements.empty() || frame.has_non_whitespace_text) {
          recoverable_validity_error(source_location, "EMPTY element has content");
        }
        return;
      case ElementModelKind::Pcdata:
        if (!frame.child_elements.empty()) {
          recoverable_validity_error(source_location,
                                     "PCDATA element has child elements");
        }
        return;
      case ElementModelKind::Children:
        if (frame.child_elements != declaration->second.children ||
            frame.has_non_whitespace_text) {
          recoverable_validity_error(source_location,
                                     "element children do not match DTD model");
        }
        return;
    }
  }

  std::vector<RawAttribute> parse_attributes() {
    std::vector<RawAttribute> attributes;

    while (true) {
      skip_whitespace();
      if (peek() == U'>' || starts_with(U"/>")) {
        return attributes;
      }

      const SourceLocation attribute_location = location();
      const std::string name = parse_name();
      if (std::any_of(attributes.begin(), attributes.end(),
                      [&](const auto& existing) { return existing.name == name; })) {
        fail(ErrorKind::WellFormedness, "duplicate attribute name");
      }
      if (attributes.size() + 1 > options_.max_attributes_per_element) {
        fail(ErrorKind::ResourceLimit,
             "attribute count exceeds max_attributes_per_element");
      }

      skip_whitespace();
      expect(U'=', "'=' after attribute name");
      skip_whitespace();
      attributes.push_back({name, parse_attribute_value(), attribute_location});
    }
  }

  bool is_namespace_declaration(const std::string& name) const {
    return options_.namespaces == NamespaceMode::Enabled &&
           (name == "xmlns" || name.rfind("xmlns:", 0) == 0);
  }

  std::map<std::string, std::string> collect_namespace_declarations(
      const std::vector<RawAttribute>& attributes) const {
    std::map<std::string, std::string> declarations;
    if (options_.namespaces == NamespaceMode::Disabled) {
      return declarations;
    }
    for (const auto& attribute : attributes) {
      if (attribute.name == "xmlns") {
        declarations[""] = attribute.value;
      } else if (attribute.name.rfind("xmlns:", 0) == 0) {
        declarations[attribute.name.substr(6)] = attribute.value;
      }
    }
    return declarations;
  }

  QualifiedName resolve_name(const std::string& qname,
                             bool is_element,
                             SourceLocation source_location) const {
    if (options_.namespaces == NamespaceMode::Disabled) {
      return QualifiedName{"", qname, qname};
    }

    const std::size_t colon = qname.find(':');
    if (colon == std::string::npos) {
      const std::string uri = is_element ? lookup_namespace("") : "";
      return QualifiedName{uri, qname, qname};
    }
    if (colon == 0 || colon + 1 >= qname.size() ||
        qname.find(':', colon + 1) != std::string::npos) {
      fail_at(ErrorKind::WellFormedness, source_location,
              "invalid namespace-qualified name");
    }

    const std::string prefix = qname.substr(0, colon);
    const std::string local_name = qname.substr(colon + 1);
    const std::string uri = lookup_namespace(prefix);
    if (uri.empty() && prefix != "xml") {
      fail_at(ErrorKind::WellFormedness, source_location,
              "undeclared namespace prefix");
    }
    return QualifiedName{uri, local_name, qname};
  }

  std::string lookup_namespace(const std::string& prefix) const {
    if (prefix == "xml") {
      return "http://www.w3.org/XML/1998/namespace";
    }
    for (auto scope = namespace_scopes_.rbegin(); scope != namespace_scopes_.rend();
         ++scope) {
      const auto found = scope->find(prefix);
      if (found != scope->end()) {
        return found->second;
      }
    }
    return "";
  }

  std::string parse_attribute_value() {
    const SourceLocation start = location();
    const char32_t quote = peek();
    if (quote != U'\'' && quote != U'"') {
      fail(ErrorKind::WellFormedness, "expected quoted attribute value");
    }
    advance();

    std::string value;
    while (!eof() && peek() != quote) {
      if (peek() == U'<') {
        fail(ErrorKind::WellFormedness, "'<' is not allowed in attribute values");
      }
      if (peek() == U'&') {
        value += parse_reference();
      } else {
        append_utf8(value, peek());
        advance();
      }
      enforce_token_size(value.size(), start);
    }
    expect(quote, "attribute quote");
    return value;
  }

  void parse_content(const std::string& open_name) {
    while (!eof()) {
      if (starts_with(U"</")) {
        const SourceLocation end_start = location();
        expect_sequence(U"</", "end tag");
        const std::string end_name = parse_name();
        skip_whitespace();
        expect(U'>', "'>'");
        if (end_name != open_name) {
          fail_at(ErrorKind::WellFormedness, end_start, "mismatched end tag");
        }
        handler_.end_element(element_name_stack_.back());
        return;
      }
      if (starts_with(U"<!--")) {
        parse_comment(&handler_);
      } else if (starts_with(U"<![CDATA[")) {
        parse_cdata();
      } else if (starts_with(U"<?")) {
        parse_processing_instruction(&handler_);
      } else if (peek() == U'<') {
        parse_element();
      } else {
        parse_text();
      }
    }

    fail(ErrorKind::WellFormedness, "truncated XML element content");
  }

  void parse_text() {
    const SourceLocation start = location();
    std::string text;
    while (!eof() && peek() != U'<') {
      if (peek() == U'&') {
        text += parse_reference();
      } else {
        append_utf8(text, peek());
        advance();
      }
      enforce_token_size(text.size(), start);
    }
    if (!text.empty()) {
      if (!validation_stack_.empty() &&
          std::any_of(text.begin(), text.end(), [](char ch) {
            return ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r';
          })) {
        validation_stack_.back().has_non_whitespace_text = true;
      }
      handler_.characters(text);
    }
  }

  void parse_comment(SaxHandler* callback_handler) {
    const SourceLocation start = location();
    expect_sequence(U"<!--", "comment start");
    std::string text;
    while (!eof()) {
      if (starts_with(U"-->")) {
        expect_sequence(U"-->", "comment close");
        if (callback_handler != nullptr) {
          callback_handler->comment(text);
        }
        return;
      }
      if (starts_with(U"--")) {
        fail(ErrorKind::WellFormedness, "comment text cannot contain '--'");
      }
      append_utf8(text, peek());
      enforce_token_size(text.size(), start);
      advance();
    }
    fail_at(ErrorKind::WellFormedness, start, "truncated XML comment");
  }

  void parse_cdata() {
    const SourceLocation start = location();
    expect_sequence(U"<![CDATA[", "CDATA start");
    std::string text;
    while (!eof()) {
      if (starts_with(U"]]>")) {
        expect_sequence(U"]]>", "CDATA close");
        handler_.cdata(text);
        return;
      }
      append_utf8(text, peek());
      enforce_token_size(text.size(), start);
      advance();
    }
    fail_at(ErrorKind::WellFormedness, start, "truncated CDATA section");
  }

  void parse_processing_instruction(SaxHandler* callback_handler) {
    const SourceLocation start = location();
    expect_sequence(U"<?", "processing instruction start");
    const std::string target = parse_name();
    if (lowercase_ascii(target) == "xml") {
      fail_at(ErrorKind::WellFormedness, start,
              "XML declaration is only allowed at document start");
    }

    if (!eof() && is_space(peek())) {
      skip_whitespace();
    }

    std::string data;
    while (!eof() && !starts_with(U"?>")) {
      append_utf8(data, peek());
      enforce_token_size(data.size(), start);
      advance();
    }
    expect_sequence(U"?>", "processing instruction close");
    if (callback_handler != nullptr) {
      callback_handler->processing_instruction(target, data);
    }
  }

  std::string parse_reference() {
    const SourceLocation start = location();
    expect(U'&', "'&'");
    ++entity_expansions_;
    if (entity_expansions_ > options_.max_entity_expansions) {
      fail_at(ErrorKind::ResourceLimit, start,
              "entity expansion count exceeds max_entity_expansions");
    }

    std::u32string name;
    while (!eof() && peek() != U';') {
      name.push_back(peek());
      enforce_token_size(name.size(), start);
      advance();
    }
    expect(U';', "';' after entity reference");

    const std::string ascii = to_ascii(name);
    if (ascii == "lt") {
      return "<";
    }
    if (ascii == "gt") {
      return ">";
    }
    if (ascii == "amp") {
      return "&";
    }
    if (ascii == "apos") {
      return "'";
    }
    if (ascii == "quot") {
      return "\"";
    }
    if (!ascii.empty() && ascii[0] == '#') {
      return parse_character_reference(ascii, start);
    }
    const auto entity = dtd_.entities.find(ascii);
    if (entity != dtd_.entities.end()) {
      return expand_entity_text(entity->second, start, 0);
    }
    if (options_.validation == ValidationMode::Dtd) {
      fail_at(ErrorKind::Validity, start, "undeclared entity reference");
    }
    fail_at(ErrorKind::WellFormedness, start, "unknown entity reference");
  }

  std::string expand_entity_text(const std::string& text,
                                 SourceLocation start,
                                 std::size_t depth) {
    if (depth > options_.max_entity_expansions) {
      fail_at(ErrorKind::ResourceLimit, start,
              "entity expansion count exceeds max_entity_expansions");
    }
    std::string expanded;
    for (std::size_t cursor = 0; cursor < text.size(); ++cursor) {
      if (text[cursor] != '&') {
        expanded.push_back(text[cursor]);
        continue;
      }
      const std::size_t end = text.find(';', cursor + 1);
      if (end == std::string::npos) {
        fail_at(ErrorKind::Validity, start, "truncated entity replacement");
      }
      ++entity_expansions_;
      if (entity_expansions_ > options_.max_entity_expansions) {
        fail_at(ErrorKind::ResourceLimit, start,
                "entity expansion count exceeds max_entity_expansions");
      }
      const std::string name = text.substr(cursor + 1, end - cursor - 1);
      if (name == "lt") {
        expanded += "<";
      } else if (name == "gt") {
        expanded += ">";
      } else if (name == "amp") {
        expanded += "&";
      } else if (name == "apos") {
        expanded += "'";
      } else if (name == "quot") {
        expanded += "\"";
      } else if (!name.empty() && name[0] == '#') {
        expanded += parse_character_reference(name, start);
      } else {
        const auto nested = dtd_.entities.find(name);
        if (nested == dtd_.entities.end()) {
          fail_at(ErrorKind::Validity, start, "undeclared entity reference");
        }
        expanded += expand_entity_text(nested->second, start, depth + 1);
      }
      cursor = end;
      if (expanded.size() > options_.max_entity_replacement_bytes) {
        fail_at(ErrorKind::ResourceLimit, start,
                "entity replacement exceeds max_entity_replacement_bytes");
      }
    }
    return expanded;
  }

  std::string parse_character_reference(const std::string& ascii,
                                        SourceLocation start) const {
    int base = 10;
    std::size_t offset = 1;
    if (ascii.size() > 2 && (ascii[1] == 'x' || ascii[1] == 'X')) {
      base = 16;
      offset = 2;
    }
    if (offset >= ascii.size()) {
      fail_at(ErrorKind::WellFormedness, start, "empty character reference");
    }

    char32_t value = 0;
    for (; offset < ascii.size(); ++offset) {
      const char ch = ascii[offset];
      int digit = -1;
      if (ch >= '0' && ch <= '9') {
        digit = ch - '0';
      } else if (ch >= 'a' && ch <= 'f') {
        digit = 10 + ch - 'a';
      } else if (ch >= 'A' && ch <= 'F') {
        digit = 10 + ch - 'A';
      }
      if (digit < 0 || digit >= base) {
        fail_at(ErrorKind::WellFormedness, start,
                "invalid character reference");
      }
      if (value >
          (std::numeric_limits<char32_t>::max() - static_cast<char32_t>(digit)) /
              static_cast<char32_t>(base)) {
        fail_at(ErrorKind::WellFormedness, start,
                "character reference overflow");
      }
      value = static_cast<char32_t>(value * base + digit);
    }
    if (!is_valid_xml_char(value, options_.version)) {
      fail_at(ErrorKind::WellFormedness, start,
              "character reference is not an XML character");
    }

    std::string result;
    append_utf8(result, value);
    return result;
  }

  DecodedDocument document_;
  SaxHandler& handler_;
  const ParserOptions& options_;
  std::size_t index_ = 0;
  std::size_t entity_expansions_ = 0;
  std::vector<std::string> element_stack_;
  std::vector<QualifiedName> element_name_stack_;
  std::vector<std::map<std::string, std::string>> namespace_scopes_;
  DtdState dtd_;
  std::vector<ValidationFrame> validation_stack_;
};

}  // namespace

void parse_xml_document(std::string_view xml,
                        SaxHandler& handler,
                        const ParserOptions& options) {
  Parser parser(xml, handler, options);
  parser.parse();
}

}  // namespace xmlparser::v1::detail
