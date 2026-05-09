#include <catch2/catch_test_macros.hpp>

#include <xmlparser/xmlparser.h>

#include <string>

TEST_CASE("REQ_ERR_02_exception_contains_message_line_column_and_byte_offset",
          "[req][bdd][m2][REQ-ERR-02]") {
  try {
    xmlparser::v1::parse("<root>\n  <child>\n</root>");
    FAIL("mismatched tags were accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::WellFormedness);
    REQUIRE(error.location().line == 3U);
    REQUIRE(error.location().column == 1U);
    REQUIRE(error.location().byte_offset == 17U);
    REQUIRE(std::string(error.what()).find("<child>") == std::string::npos);
  }
}

TEST_CASE("REQ_ERR_02_location_tracks_utf16_input_offsets",
          "[req][bdd][m2][REQ-ERR-02]") {
  const char bytes[] = {static_cast<char>(0xFF), static_cast<char>(0xFE),
                        '<', 0, 'r', 0, 'o', 0, 'o', 0, 't', 0, '>', 0,
                        '<', 0, '/', 0, 'b', 0, 'a', 0, 'd', 0, '>', 0};
  const std::string xml(bytes, sizeof(bytes));

  try {
    xmlparser::v1::parse(xml);
    FAIL("mismatched UTF-16 tags were accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::WellFormedness);
    REQUIRE(error.location().byte_offset == 14U);
  }
}
