#include <catch2/catch_test_macros.hpp>

#include <xmlparser/xmlparser.h>

TEST_CASE("REQ_ERR_01_reports_well_formedness_error_kind",
          "[req][bdd][m5][REQ-ERR-01]") {
  xmlparser::v1::ParserOptions options;
  options.validation = xmlparser::v1::ValidationMode::Dtd;

  try {
    (void)xmlparser::v1::parse("<root><child></root>", options);
    FAIL("malformed XML was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::WellFormedness);
  }
}

TEST_CASE("REQ_ERR_01_reports_dtd_validity_error_kind",
          "[req][bdd][m5][REQ-ERR-01]") {
  xmlparser::v1::ParserOptions options;
  options.validation = xmlparser::v1::ValidationMode::Dtd;

  try {
    (void)xmlparser::v1::parse(
        "<!DOCTYPE root [<!ELEMENT root EMPTY>]><root><child/></root>",
        options);
    FAIL("DTD-invalid XML was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::Validity);
  }
}

TEST_CASE("REQ_ERR_01_does_not_confuse_validity_with_parse_failure",
          "[req][bdd][m5][REQ-ERR-01]") {
  xmlparser::v1::ParserOptions options;
  options.validation = xmlparser::v1::ValidationMode::Dtd;

  REQUIRE_NOTHROW((void)xmlparser::v1::parse(
      "<!DOCTYPE root [<!ELEMENT root EMPTY>]><root/>", options));
}
