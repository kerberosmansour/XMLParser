#include <catch2/catch_test_macros.hpp>

#include "parser_event_recorder.h"

TEST_CASE("REQ_STD_01_accepts_well_formed_xml10_documents",
          "[req][bdd][conformance][m2][REQ-STD-01]") {
  RecordingHandler handler;

  xmlparser::v1::parse("<root><child attr=\"value\">text</child></root>", handler);

  REQUIRE(handler.events.size() == 7);
  REQUIRE(handler.events[0].type == "start_document");
  REQUIRE(handler.events[1].type == "start_element");
  REQUIRE(handler.events[1].name == "root");
  REQUIRE(handler.events[2].type == "start_element");
  REQUIRE(handler.events[2].name == "child");
  REQUIRE(handler.events[2].attributes.size() == 1);
  REQUIRE(handler.events[2].attributes[0].qname == "attr");
  REQUIRE(handler.events[2].attributes[0].value == "value");
  REQUIRE(handler.events[3].type == "characters");
  REQUIRE(handler.events[3].value == "text");
  REQUIRE(handler.events[4].type == "end_element");
  REQUIRE(handler.events[4].name == "child");
  REQUIRE(handler.events[5].type == "end_element");
  REQUIRE(handler.events[5].name == "root");
  REQUIRE(handler.events[6].type == "end_document");
}

TEST_CASE("REQ_STD_01_rejects_xml10_well_formedness_violations",
          "[req][bdd][m2][REQ-STD-01]") {
  RecordingHandler handler;

  try {
    xmlparser::v1::parse("<root><child></root>", handler);
    FAIL("mismatched tags were accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::WellFormedness);
    REQUIRE(error.location().line == 1U);
    REQUIRE(error.location().column > 1U);
    REQUIRE(error.location().byte_offset > 0U);
  }
}
