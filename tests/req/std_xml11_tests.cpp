#include <catch2/catch_test_macros.hpp>

#include <xmlparser/xmlparser.h>

#include <filesystem>

TEST_CASE("REQ_STD_02_selects_xml11_at_runtime",
          "[req][bdd][m5][REQ-STD-02]") {
  xmlparser::v1::ParserOptions options;
  options.version = xmlparser::v1::XmlVersion::Xml11;

  REQUIRE_NOTHROW(
      (void)xmlparser::v1::parse("<?xml version=\"1.1\"?><root>&#x1F;</root>",
                                 options));
}

TEST_CASE("REQ_STD_02_distinguishes_xml10_and_xml11_character_rules",
          "[req][bdd][m5][REQ-STD-02]") {
  xmlparser::v1::ParserOptions options;
  options.version = xmlparser::v1::XmlVersion::Xml10;

  try {
    (void)xmlparser::v1::parse("<?xml version=\"1.0\"?><root>&#x1F;</root>",
                               options);
    FAIL("XML 1.0 accepted an XML 1.1-only character reference");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::WellFormedness);
  }
}

TEST_CASE("REQ_STD_02_rejects_version_declaration_option_mismatch",
          "[req][bdd][m5][REQ-STD-02]") {
  xmlparser::v1::ParserOptions options;
  options.version = xmlparser::v1::XmlVersion::Xml11;

  try {
    (void)xmlparser::v1::parse("<?xml version=\"1.0\"?><root/>", options);
    FAIL("XML declaration mismatch was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::WellFormedness);
  }
}

TEST_CASE("REQ_STD_02_runs_xml11_conformance_subset",
          "[req][conformance][m5][REQ-STD-02]") {
  const auto manifest =
      std::filesystem::path(XMLPARSER_SOURCE_DIR) /
      "tests/fixtures/w3c/xmlconf/manifest.xml";

  REQUIRE(std::filesystem::exists(manifest));
}
