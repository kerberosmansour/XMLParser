#include <catch2/catch_test_macros.hpp>

#include "parser_event_recorder.h"

TEST_CASE("REQ_SAX_02_emits_document_start_end",
          "[req][bdd][m3][REQ-SAX-02]") {
  RecordingHandler handler;

  xmlparser::v1::parse("<root/>", handler);

  REQUIRE(handler.events.front().type == "start_document");
  REQUIRE(handler.events.back().type == "end_document");
}

TEST_CASE("REQ_SAX_02_emits_element_start_end",
          "[req][bdd][m3][REQ-SAX-02]") {
  RecordingHandler handler;

  xmlparser::v1::parse("<root><child/></root>", handler);

  REQUIRE(handler.events[1].type == "start_element");
  REQUIRE(handler.events[2].type == "start_element");
  REQUIRE(handler.events[3].type == "end_element");
  REQUIRE(handler.events[4].type == "end_element");
}

TEST_CASE("REQ_SAX_02_emits_character_pi_comment_and_cdata_events",
          "[req][bdd][m3][REQ-SAX-02]") {
  RecordingHandler handler;

  xmlparser::v1::parse("<?work go?><root><!--c--><![CDATA[x<y]]>text</root>",
                       handler);

  REQUIRE(has_event(handler, "processing_instruction", "go"));
  REQUIRE(has_event(handler, "comment", "c"));
  REQUIRE(has_event(handler, "cdata", "x<y"));
  REQUIRE(has_event(handler, "characters", "text"));
}
