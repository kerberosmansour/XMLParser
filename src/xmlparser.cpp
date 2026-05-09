#include <xmlparser/xmlparser.h>

#include "parser_core.h"

#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace xmlparser::v1 {
namespace {

constexpr SourceLocation start_location() noexcept {
  return SourceLocation{};
}

XmlParseException unsupported_parser_error() {
  return XmlParseException(ErrorKind::Unsupported, start_location(),
                           "incremental XML parsing is not implemented until milestone M3");
}

}  // namespace

XmlParseException::XmlParseException(ErrorKind kind,
                                     SourceLocation location,
                                     std::string message)
    : std::runtime_error(std::move(message)), kind_(kind), location_(location) {}

ErrorKind XmlParseException::kind() const noexcept {
  return kind_;
}

SourceLocation XmlParseException::location() const noexcept {
  return location_;
}

XmlValidityException::XmlValidityException(SourceLocation location,
                                           std::string message)
    : std::runtime_error(std::move(message)), location_(location) {}

ErrorKind XmlValidityException::kind() const noexcept {
  return ErrorKind::Validity;
}

SourceLocation XmlValidityException::location() const noexcept {
  return location_;
}

namespace {

class DomBuilder final : public SaxHandler {
 public:
  explicit DomBuilder(const ParserOptions& options) : options_(options) {}

  void start_element(const QualifiedName& name,
                     const std::vector<AttributeView>& attributes) override {
    ensure_node_available();
    Element& element =
        document.create_element(name.qualified_name, name.uri, name.local_name);
    for (const auto& attribute : attributes) {
      element.set_attribute_ns(attribute.name.uri, attribute.name.local_name,
                               attribute.name.qualified_name,
                               std::string(attribute.value));
    }
    append_to_current_parent(element);
    element_stack_.push_back(&element);
  }

  void end_element(const QualifiedName&) override {
    if (!element_stack_.empty()) {
      element_stack_.pop_back();
    }
  }

  void characters(std::string_view text) override {
    ensure_node_available();
    Text& node = document.create_text(std::string(text));
    append_to_current_parent(node);
  }

  void processing_instruction(std::string_view target,
                              std::string_view data) override {
    ensure_node_available();
    ProcessingInstruction& node =
        document.create_processing_instruction(std::string(target), std::string(data));
    append_to_current_parent(node);
  }

  void comment(std::string_view text) override {
    ensure_node_available();
    Comment& node = document.create_comment(std::string(text));
    append_to_current_parent(node);
  }

  void cdata(std::string_view text) override {
    ensure_node_available();
    CDataSection& node = document.create_cdata_section(std::string(text));
    append_to_current_parent(node);
  }

  Document document;

 private:
  void ensure_node_available() {
    if (node_count_ + 1 > options_.max_dom_nodes) {
      throw XmlParseException(ErrorKind::ResourceLimit, SourceLocation{},
                              "DOM node count exceeds max_dom_nodes");
    }
    ++node_count_;
  }

  void append_to_current_parent(Node& node) {
    if (element_stack_.empty()) {
      document.append_child(node);
    } else {
      element_stack_.back()->append_child(node);
    }
  }

  const ParserOptions& options_;
  std::size_t node_count_ = 0;
  std::vector<Element*> element_stack_;
};

}  // namespace

Document parse(std::string_view xml, const ParserOptions& options) {
  DomBuilder builder(options);
  detail::parse_xml_document(xml, builder, options);
  return std::move(builder.document);
}

void parse(std::string_view xml, SaxHandler& handler, const ParserOptions& options) {
  detail::parse_xml_document(xml, handler, options);
}

namespace {

class CallbackHandler final : public SaxHandler {
 public:
  explicit CallbackHandler(SaxCallbacks& callbacks) : callbacks_(callbacks) {}

  void start_document() override {
    if (callbacks_.on_start_document) {
      callbacks_.on_start_document();
    }
  }

  void end_document() override {
    if (callbacks_.on_end_document) {
      callbacks_.on_end_document();
    }
  }

  void start_element(const QualifiedName& name,
                     const std::vector<AttributeView>& attributes) override {
    if (callbacks_.on_start_element) {
      callbacks_.on_start_element(name, attributes);
    }
  }

  void end_element(const QualifiedName& name) override {
    if (callbacks_.on_end_element) {
      callbacks_.on_end_element(name);
    }
  }

  void characters(std::string_view text) override {
    if (callbacks_.on_characters) {
      callbacks_.on_characters(text);
    }
  }

  void processing_instruction(std::string_view target,
                              std::string_view data) override {
    if (callbacks_.on_processing_instruction) {
      callbacks_.on_processing_instruction(target, data);
    }
  }

  void comment(std::string_view text) override {
    if (callbacks_.on_comment) {
      callbacks_.on_comment(text);
    }
  }

  void cdata(std::string_view text) override {
    if (callbacks_.on_cdata) {
      callbacks_.on_cdata(text);
    }
  }

 private:
  SaxCallbacks& callbacks_;
};

}  // namespace

void parse(std::string_view xml, SaxCallbacks& callbacks, const ParserOptions& options) {
  CallbackHandler handler(callbacks);
  detail::parse_xml_document(xml, handler, options);
}

SaxParser::SaxParser(SaxHandler& handler, ParserOptions options)
    : handler_(&handler), options_(options) {}

void SaxParser::parse(std::string_view xml) {
  v1::parse(xml, *handler_, options_);
}

void SaxParser::feed(std::string_view chunk) {
  if (finished_) {
    throw XmlParseException(ErrorKind::Unsupported, SourceLocation{},
                            "SaxParser cannot be reused after finish");
  }
  if (buffer_.size() + chunk.size() > options_.max_document_bytes) {
    throw XmlParseException(ErrorKind::ResourceLimit, SourceLocation{},
                            "XML input exceeds max_document_bytes");
  }
  buffer_.append(chunk.data(), chunk.size());
}

void SaxParser::finish() {
  finished_ = true;
  v1::parse(buffer_, *handler_, options_);
  buffer_.clear();
}

const ParserOptions& SaxParser::options() const noexcept {
  return options_;
}

}  // namespace xmlparser::v1
