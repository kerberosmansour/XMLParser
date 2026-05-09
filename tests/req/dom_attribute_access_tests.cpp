#include <catch2/catch_test_macros.hpp>

#include <xmlparser/xmlparser.h>

TEST_CASE("REQ_DOM_05_get_attribute_namespace_unaware",
          "[req][bdd][m4][REQ-DOM-05]") {
  xmlparser::v1::Document document =
      xmlparser::v1::parse("<root plain=\"x\" other=\"y\"/>");
  const auto* root = document.document_element();

  REQUIRE(root != nullptr);
  REQUIRE(root->get_attribute("plain") == "x");
  REQUIRE(root->get_attribute("missing").empty());
}

TEST_CASE("REQ_DOM_05_get_attribute_ns_distinguishes_same_local_name_different_uri",
          "[req][bdd][m4][REQ-DOM-05]") {
  xmlparser::v1::Document document = xmlparser::v1::parse(
      "<root xmlns:a=\"urn:a\" xmlns:b=\"urn:b\" a:code=\"1\" b:code=\"2\"/>");
  const auto* root = document.document_element();

  REQUIRE(root != nullptr);
  REQUIRE(root->get_attribute_ns("urn:a", "code") == "1");
  REQUIRE(root->get_attribute_ns("urn:b", "code") == "2");
  REQUIRE(root->get_attribute_ns("", "code").empty());
}
