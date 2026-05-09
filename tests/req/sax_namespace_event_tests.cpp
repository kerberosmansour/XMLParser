#include <catch2/catch_test_macros.hpp>

#include "parser_event_recorder.h"

TEST_CASE("REQ_SAX_05_element_events_include_uri_local_and_qname",
          "[req][bdd][m3][REQ-SAX-05]") {
  RecordingHandler handler;

  xmlparser::v1::parse("<p:root xmlns:p=\"urn:p\"/>", handler);

  REQUIRE(handler.events[1].uri == "urn:p");
  REQUIRE(handler.events[1].local == "root");
  REQUIRE(handler.events[1].name == "p:root");
}

TEST_CASE("REQ_SAX_05_attribute_events_include_uri_local_and_qname",
          "[req][bdd][m3][REQ-SAX-05]") {
  RecordingHandler handler;

  xmlparser::v1::parse("<root xmlns:p=\"urn:p\" p:code=\"7\" plain=\"x\"/>",
                       handler);

  REQUIRE(handler.events[1].attributes.size() == 2);
  REQUIRE(handler.events[1].attributes[0].uri == "urn:p");
  REQUIRE(handler.events[1].attributes[0].local == "code");
  REQUIRE(handler.events[1].attributes[0].qname == "p:code");
  REQUIRE(handler.events[1].attributes[1].uri.empty());
  REQUIRE(handler.events[1].attributes[1].local == "plain");
}
