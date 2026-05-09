#include <xmlparser/xmlparser.h>

#include "parser_core.h"

#include <ostream>
#include <string>
#include <string_view>
#include <utility>

namespace xmlparser::v1 {
namespace {

constexpr SourceLocation start_location() noexcept {
  return SourceLocation{};
}

XmlParseException empty_input_error() {
  return XmlParseException(ErrorKind::EmptyInput, start_location(),
                           "XML input is empty");
}

XmlParseException unsupported_parser_error() {
  return XmlParseException(ErrorKind::Unsupported, start_location(),
                           "incremental XML parsing is not implemented until milestone M3");
}

XmlParseException unsupported_serializer_error() {
  return XmlParseException(ErrorKind::Unsupported, start_location(),
                           "XML serialization is not implemented until milestone M4");
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

Document parse(std::string_view xml, const ParserOptions& options) {
  class NullHandler : public SaxHandler {};
  NullHandler handler;
  detail::parse_xml_document(xml, handler, options);
  return Document{};
}

void parse(std::string_view xml, SaxHandler& handler, const ParserOptions& options) {
  detail::parse_xml_document(xml, handler, options);
}

SaxParser::SaxParser(SaxHandler& handler, ParserOptions options)
    : handler_(&handler), options_(options) {}

void SaxParser::parse(std::string_view xml) {
  v1::parse(xml, *handler_, options_);
}

void SaxParser::feed(std::string_view chunk) {
  if (chunk.empty()) {
    throw empty_input_error();
  }
  throw unsupported_parser_error();
}

void SaxParser::finish() {
  throw unsupported_parser_error();
}

const ParserOptions& SaxParser::options() const noexcept {
  return options_;
}

std::string serialize(const Document&, const SerializeOptions&) {
  throw unsupported_serializer_error();
}

void serialize(const Document&, std::ostream&, const SerializeOptions&) {
  throw unsupported_serializer_error();
}

}  // namespace xmlparser::v1
