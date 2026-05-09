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

TEST_CASE("REQ_STD_04_rejects_invalid_utf8_leading_byte",
          "[req][bdd][m5][REQ-STD-04][REQ-ERR-06]") {
  const std::string xml = std::string("<root>") + static_cast<char>(0x80) +
                          "</root>";

  try {
    xmlparser::v1::parse(xml);
    FAIL("invalid UTF-8 leading byte was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::Encoding);
  }
}

TEST_CASE("REQ_STD_04_rejects_invalid_utf8_continuation",
          "[req][bdd][m5][REQ-STD-04][REQ-ERR-06]") {
  const std::string xml = std::string("<root>") + static_cast<char>(0xC2) +
                          "A</root>";

  try {
    xmlparser::v1::parse(xml);
    FAIL("invalid UTF-8 continuation was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::Encoding);
  }
}

TEST_CASE("REQ_STD_04_rejects_overlong_utf8_scalar",
          "[req][bdd][m5][REQ-STD-04][REQ-ERR-06]") {
  const std::string xml = std::string("<root>") + static_cast<char>(0xC0) +
                          static_cast<char>(0x80) + "</root>";

  try {
    xmlparser::v1::parse(xml);
    FAIL("overlong UTF-8 sequence was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::Encoding);
  }
}

TEST_CASE("REQ_STD_04_accepts_four_byte_utf8_scalar",
          "[req][bdd][m5][REQ-STD-04]") {
  RecordingHandler handler;
  const std::string xml = std::string("<root>") + static_cast<char>(0xF0) +
                          static_cast<char>(0x9F) + static_cast<char>(0x98) +
                          static_cast<char>(0x80) + "</root>";

  xmlparser::v1::parse(xml, handler);

  const std::string expected = std::string() + static_cast<char>(0xF0) +
                               static_cast<char>(0x9F) + static_cast<char>(0x98) +
                               static_cast<char>(0x80);
  REQUIRE(has_event(handler, "characters", expected));
}

TEST_CASE("REQ_STD_04_accepts_three_byte_utf8_scalar",
          "[req][bdd][m5][REQ-STD-04]") {
  RecordingHandler handler;
  const std::string xml = std::string("<root>") + static_cast<char>(0xE2) +
                          static_cast<char>(0x82) + static_cast<char>(0xAC) +
                          "</root>";

  xmlparser::v1::parse(xml, handler);

  const std::string expected = std::string() + static_cast<char>(0xE2) +
                               static_cast<char>(0x82) + static_cast<char>(0xAC);
  REQUIRE(has_event(handler, "characters", expected));
}

TEST_CASE("REQ_STD_04_rejects_utf8_scalar_above_unicode_max",
          "[req][bdd][m5][REQ-STD-04][REQ-ERR-06]") {
  const std::string xml = std::string("<root>") + static_cast<char>(0xF4) +
                          static_cast<char>(0x90) + static_cast<char>(0x80) +
                          static_cast<char>(0x80) + "</root>";

  try {
    xmlparser::v1::parse(xml);
    FAIL("out-of-range UTF-8 scalar was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::Encoding);
  }
}

TEST_CASE("REQ_STD_04_rejects_literal_xml10_control_character",
          "[req][bdd][m5][REQ-STD-04][REQ-ERR-06]") {
  const std::string xml = std::string("<root>") + static_cast<char>(0x01) +
                          "</root>";

  try {
    xmlparser::v1::parse(xml);
    FAIL("literal XML 1.0 control character was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::WellFormedness);
  }
}

TEST_CASE("REQ_STD_04_rejects_truncated_utf16_code_unit",
          "[req][bdd][m5][REQ-STD-04][REQ-ERR-06]") {
  std::string xml;
  xml.push_back(static_cast<char>(0xFF));
  xml.push_back(static_cast<char>(0xFE));
  xml.push_back('<');

  try {
    xmlparser::v1::parse(xml);
    FAIL("truncated UTF-16 code unit was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::Encoding);
  }
}

TEST_CASE("REQ_STD_04_rejects_invalid_utf16_surrogate_pair",
          "[req][bdd][m5][REQ-STD-04][REQ-ERR-06]") {
  std::string xml;
  xml.push_back(static_cast<char>(0xFF));
  xml.push_back(static_cast<char>(0xFE));
  xml.push_back(static_cast<char>(0x00));
  xml.push_back(static_cast<char>(0xD8));
  xml.push_back('A');
  xml.push_back('\0');

  try {
    xmlparser::v1::parse(xml);
    FAIL("invalid UTF-16 surrogate pair was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::Encoding);
  }
}

TEST_CASE("REQ_STD_04_rejects_truncated_utf16_surrogate_pair",
          "[req][bdd][m5][REQ-STD-04][REQ-ERR-06]") {
  std::string xml;
  xml.push_back(static_cast<char>(0xFF));
  xml.push_back(static_cast<char>(0xFE));
  xml.push_back(static_cast<char>(0x00));
  xml.push_back(static_cast<char>(0xD8));

  try {
    xmlparser::v1::parse(xml);
    FAIL("truncated UTF-16 surrogate pair was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::Encoding);
  }
}

TEST_CASE("REQ_STD_04_rejects_unexpected_utf16_trailing_surrogate",
          "[req][bdd][m5][REQ-STD-04][REQ-ERR-06]") {
  std::string xml;
  xml.push_back(static_cast<char>(0xFF));
  xml.push_back(static_cast<char>(0xFE));
  xml.push_back(static_cast<char>(0x00));
  xml.push_back(static_cast<char>(0xDC));

  try {
    xmlparser::v1::parse(xml);
    FAIL("unexpected UTF-16 trailing surrogate was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::Encoding);
  }
}
