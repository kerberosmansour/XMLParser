#include <catch2/catch_test_macros.hpp>

#include "parser_event_recorder.h"

TEST_CASE("REQ_STD_05_parses_xml_declaration",
          "[req][bdd][m2][REQ-STD-05]") {
  RecordingHandler handler;

  xmlparser::v1::parse("<?xml version=\"1.0\" encoding=\"UTF-8\"?><root/>", handler);

  REQUIRE(handler.events[1].type == "start_element");
  REQUIRE(handler.events[1].name == "root");
}

TEST_CASE("REQ_STD_05_emits_processing_instruction",
          "[req][bdd][m2][REQ-STD-05]") {
  RecordingHandler handler;

  xmlparser::v1::parse("<root><?work do-it?></root>", handler);

  REQUIRE(has_event(handler, "processing_instruction", "do-it"));
}

TEST_CASE("REQ_STD_05_preserves_cdata_boundaries_in_events",
          "[req][bdd][m2][REQ-STD-05]") {
  RecordingHandler handler;

  xmlparser::v1::parse("<root><![CDATA[x<y&z]]></root>", handler);

  REQUIRE(has_event(handler, "cdata", "x<y&z"));
}

TEST_CASE("REQ_STD_05_parses_comments", "[req][bdd][m2][REQ-STD-05]") {
  RecordingHandler handler;

  xmlparser::v1::parse("<root><!--comment--></root>", handler);

  REQUIRE(has_event(handler, "comment", "comment"));
}

TEST_CASE("REQ_STD_05_resolves_predefined_and_character_entities",
          "[req][bdd][m2][REQ-STD-05]") {
  RecordingHandler handler;

  xmlparser::v1::parse("<root attr=\"&quot;A&amp;B&quot;\">&lt;&#65;&#x41;&gt;</root>",
                       handler);

  REQUIRE(handler.events[1].attributes.size() == 1);
  REQUIRE(handler.events[1].attributes[0].value == "\"A&B\"");
  REQUIRE(has_event(handler, "characters", "<AA>"));
}
