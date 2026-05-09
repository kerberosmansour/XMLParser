#include <catch2/catch_test_macros.hpp>

#include <xmlparser/xmlparser.h>

#include <string>

TEST_CASE("REQ_ERR_05_depth_limit_returns_resource_error",
          "[req][bdd][m2][REQ-ERR-05][REQ-ERR-06]") {
  xmlparser::v1::ParserOptions options;
  options.max_depth = 1;

  try {
    xmlparser::v1::parse("<root><child/></root>", options);
    FAIL("document exceeding depth limit was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::ResourceLimit);
  }
}

TEST_CASE("REQ_ERR_05_token_limit_returns_resource_error",
          "[req][bdd][m2][REQ-ERR-05][REQ-ERR-06]") {
  xmlparser::v1::ParserOptions options;
  options.max_token_bytes = 4;

  try {
    xmlparser::v1::parse("<root>long text</root>", options);
    FAIL("text exceeding token limit was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::ResourceLimit);
  }
}

TEST_CASE("REQ_ERR_05_entity_expansion_limit_returns_resource_error",
          "[req][bdd][m2][REQ-ERR-05][REQ-ERR-06]") {
  xmlparser::v1::ParserOptions options;
  options.max_entity_expansions = 1;

  try {
    xmlparser::v1::parse("<root>&amp;&lt;</root>", options);
    FAIL("entity expansion over limit was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::ResourceLimit);
  }
}
