#include <catch2/catch_test_macros.hpp>

#include "parser_event_recorder.h"

#include <string>

namespace {

std::string utf16le_with_bom(const char* ascii) {
  std::string bytes;
  bytes.push_back(static_cast<char>(0xFF));
  bytes.push_back(static_cast<char>(0xFE));
  for (const char* cursor = ascii; *cursor != '\0'; ++cursor) {
    bytes.push_back(*cursor);
    bytes.push_back('\0');
  }
  return bytes;
}

std::string utf16be_with_bom(const char* ascii) {
  std::string bytes;
  bytes.push_back(static_cast<char>(0xFE));
  bytes.push_back(static_cast<char>(0xFF));
  for (const char* cursor = ascii; *cursor != '\0'; ++cursor) {
    bytes.push_back('\0');
    bytes.push_back(*cursor);
  }
  return bytes;
}

}  // namespace

TEST_CASE("REQ_STD_04_detects_utf8_bom", "[req][bdd][m2][REQ-STD-04]") {
  RecordingHandler handler;
  const std::string xml = "\xEF\xBB\xBF<root/>";

  xmlparser::v1::parse(xml, handler);

  REQUIRE(handler.events[1].type == "start_element");
  REQUIRE(handler.events[1].name == "root");
}

TEST_CASE("REQ_STD_04_detects_utf16le_bom", "[req][bdd][m2][REQ-STD-04]") {
  RecordingHandler handler;

  xmlparser::v1::parse(utf16le_with_bom("<root/>"), handler);

  REQUIRE(handler.events[1].type == "start_element");
  REQUIRE(handler.events[1].name == "root");
}

TEST_CASE("REQ_STD_04_detects_utf16be_bom", "[req][bdd][m2][REQ-STD-04]") {
  RecordingHandler handler;

  xmlparser::v1::parse(utf16be_with_bom("<root/>"), handler);

  REQUIRE(handler.events[1].type == "start_element");
  REQUIRE(handler.events[1].name == "root");
}

TEST_CASE("REQ_STD_04_rejects_truncated_multibyte_sequence_with_byte_offset",
          "[req][bdd][m2][REQ-STD-04][REQ-ERR-06]") {
  const std::string xml = std::string("<root>") + static_cast<char>(0xE2);

  try {
    xmlparser::v1::parse(xml);
    FAIL("truncated UTF-8 sequence was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::Encoding);
    REQUIRE(error.location().byte_offset == 6U);
  }
}
