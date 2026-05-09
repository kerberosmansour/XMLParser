#include <catch2/catch_test_macros.hpp>

#include "parser_event_recorder.h"

TEST_CASE("REQ_SAX_01_parses_without_dom_allocation",
          "[req][bdd][m3][REQ-SAX-01]") {
  RecordingHandler handler;
  xmlparser::v1::SaxParser parser(handler);

  parser.feed("<root><child/></root>");
  parser.finish();

  REQUIRE(handler.events[1].type == "start_element");
  REQUIRE(handler.events[2].type == "start_element");
}

TEST_CASE("REQ_SAX_01_streams_large_document_with_bounded_memory",
          "[req][bdd][m3][REQ-SAX-01]") {
  RecordingHandler handler;
  xmlparser::v1::ParserOptions options;
  options.max_document_bytes = 12;
  xmlparser::v1::SaxParser parser(handler, options);

  parser.feed("<root>");
  try {
    parser.feed("<child/>");
    FAIL("incremental buffer over document limit was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::ResourceLimit);
  }
}
