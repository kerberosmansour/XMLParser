#include <catch2/catch_test_macros.hpp>

#include "parser_event_recorder.h"

TEST_CASE("REQ_STD_03_resolves_default_namespace_scope",
          "[req][bdd][m3][REQ-STD-03]") {
  RecordingHandler handler;

  xmlparser::v1::parse("<root xmlns=\"urn:a\"><child/></root>", handler);

  REQUIRE(handler.events[1].uri == "urn:a");
  REQUIRE(handler.events[1].local == "root");
  REQUIRE(handler.events[2].uri == "urn:a");
  REQUIRE(handler.events[2].local == "child");
}

TEST_CASE("REQ_STD_03_resolves_prefixed_names_across_nested_scopes",
          "[req][bdd][m3][REQ-STD-03]") {
  RecordingHandler handler;

  xmlparser::v1::parse(
      "<root xmlns:p=\"urn:one\"><p:item/><inner xmlns:p=\"urn:two\"><p:item "
      "p:code=\"7\"/></inner></root>",
      handler);

  std::vector<RecordedEvent> item_starts;
  for (const auto& event : handler.events) {
    if (event.type == "start_element" && event.name == "p:item") {
      item_starts.push_back(event);
    }
  }

  REQUIRE(item_starts.size() == 2);
  REQUIRE(item_starts[0].uri == "urn:one");
  REQUIRE(item_starts[1].uri == "urn:two");
  REQUIRE(item_starts[1].attributes.size() == 1);
  REQUIRE(item_starts[1].attributes[0].uri == "urn:two");
  REQUIRE(item_starts[1].attributes[0].local == "code");
}

TEST_CASE("REQ_STD_03_rejects_duplicate_expanded_attribute_names",
          "[req][bdd][m3][REQ-STD-03]") {
  try {
    xmlparser::v1::parse(
        "<root xmlns:a=\"urn:dup\" xmlns:b=\"urn:dup\" a:x=\"1\" b:x=\"2\"/>");
    FAIL("duplicate expanded attribute names were accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::WellFormedness);
  }
}

TEST_CASE("REQ_STD_03_handles_namespace_11_undeclaration",
          "[req][bdd][m3][REQ-STD-03]") {
  RecordingHandler handler;

  xmlparser::v1::parse("<root xmlns=\"urn:a\"><child xmlns=\"\"/></root>", handler);

  REQUIRE(handler.events[1].uri == "urn:a");
  REQUIRE(handler.events[2].name == "child");
  REQUIRE(handler.events[2].uri.empty());
}

TEST_CASE("REQ_STD_03_can_disable_namespace_processing",
          "[req][bdd][m5][REQ-STD-03]") {
  RecordingHandler handler;
  xmlparser::v1::ParserOptions options;
  options.namespaces = xmlparser::v1::NamespaceMode::Disabled;

  xmlparser::v1::parse("<p:root xmlns:p=\"urn:p\" p:code=\"7\"/>", handler, options);

  REQUIRE(handler.events[1].name == "p:root");
  REQUIRE(handler.events[1].uri.empty());
  REQUIRE(handler.events[1].attributes.size() == 2);
}

TEST_CASE("REQ_STD_03_rejects_invalid_namespace_qualified_name",
          "[req][bdd][m5][REQ-STD-03]") {
  try {
    xmlparser::v1::parse("<a:b:c xmlns:a=\"urn:a\"/>");
    FAIL("invalid namespace-qualified name was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::WellFormedness);
  }
}

TEST_CASE("REQ_STD_03_rejects_undeclared_namespace_prefix",
          "[req][bdd][m5][REQ-STD-03]") {
  try {
    xmlparser::v1::parse("<p:root/>");
    FAIL("undeclared namespace prefix was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::WellFormedness);
  }
}

TEST_CASE("REQ_STD_03_resolves_reserved_xml_prefix",
          "[req][bdd][m5][REQ-STD-03]") {
  RecordingHandler handler;

  xmlparser::v1::parse("<root xml:lang=\"en\"/>", handler);

  REQUIRE(handler.events[1].attributes.size() == 1);
  REQUIRE(handler.events[1].attributes[0].uri ==
          "http://www.w3.org/XML/1998/namespace");
}
