#include <catch2/catch_test_macros.hpp>

#include <xmlparser/xmlparser.h>

#include <string>

TEST_CASE("REQ_ERR_06_empty_input_returns_defined_error",
          "[req][bdd][m2][REQ-ERR-06]") {
  try {
    xmlparser::v1::parse("");
    FAIL("empty input was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::EmptyInput);
    REQUIRE(error.location().line == 1U);
    REQUIRE(error.location().column == 1U);
    REQUIRE(error.location().byte_offset == 0U);
  }
}

TEST_CASE("REQ_ERR_06_truncated_input_returns_defined_error",
          "[req][bdd][m2][REQ-ERR-06]") {
  try {
    xmlparser::v1::parse("<root><![CDATA[unfinished");
    FAIL("truncated CDATA was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::WellFormedness);
    REQUIRE(error.location().byte_offset > 0U);
  }
}

TEST_CASE("REQ_ERR_06_secret_payload_is_not_echoed",
          "[req][bdd][m2][REQ-ERR-06][REQ-SEC-02]") {
  const std::string xml = "<root><password>hunter2</password><child></root>";

  try {
    xmlparser::v1::parse(xml);
    FAIL("malformed XML was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    const std::string message = error.what();
    REQUIRE(message.find(xml) == std::string::npos);
    REQUIRE(message.find("hunter2") == std::string::npos);
    REQUIRE(message.find("password") == std::string::npos);
  }
}
