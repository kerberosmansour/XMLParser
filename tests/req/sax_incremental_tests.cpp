#include <catch2/catch_test_macros.hpp>

#include "parser_event_recorder.h"

TEST_CASE("REQ_SAX_04_accepts_one_byte_chunks",
          "[req][bdd][m3][REQ-SAX-04]") {
  RecordingHandler handler;
  xmlparser::v1::SaxParser parser(handler);
  const std::string xml = "<root><child>text</child></root>";

  for (char ch : xml) {
    parser.feed(std::string_view(&ch, 1));
  }
  parser.finish();

  REQUIRE(handler.events[1].name == "root");
  REQUIRE(has_event(handler, "characters", "text"));
}

TEST_CASE("REQ_SAX_04_accepts_chunks_split_inside_markup",
          "[req][bdd][m3][REQ-SAX-04]") {
  RecordingHandler handler;
  xmlparser::v1::SaxParser parser(handler);

  parser.feed("<ro");
  parser.feed("ot><chi");
  parser.feed("ld/></root>");
  parser.finish();

  REQUIRE(handler.events[1].name == "root");
  REQUIRE(handler.events[2].name == "child");
}

TEST_CASE("REQ_SAX_04_accepts_chunks_split_inside_multibyte_sequence",
          "[req][bdd][m3][REQ-SAX-04]") {
  RecordingHandler handler;
  xmlparser::v1::SaxParser parser(handler);
  const std::string xml = "<root>\xC2\xA3</root>";

  parser.feed(std::string_view(xml.data(), 7));
  parser.feed(std::string_view(xml.data() + 7, 1));
  parser.feed(std::string_view(xml.data() + 8, xml.size() - 8));
  parser.finish();

  REQUIRE(has_event(handler, "characters", "\xC2\xA3"));
}

TEST_CASE("REQ_SAX_04_finish_rejects_truncated_document",
          "[req][bdd][m3][REQ-SAX-04]") {
  RecordingHandler handler;
  xmlparser::v1::SaxParser parser(handler);

  parser.feed("<root><child>");

  try {
    parser.finish();
    FAIL("truncated document was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::WellFormedness);
  }
}
