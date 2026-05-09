#include <catch2/catch_test_macros.hpp>

#include <xmlparser/xmlparser.h>

#include <string>
#include <vector>

#include "parser_event_recorder.h"

TEST_CASE("REQ_SAX_03_registers_virtual_handler",
          "[req][bdd][m3][REQ-SAX-03]") {
  RecordingHandler handler;

  xmlparser::v1::parse("<root><child/></root>", handler);

  REQUIRE(handler.events.size() == 6);
  REQUIRE(handler.events[1].name == "root");
}

TEST_CASE("REQ_SAX_03_registers_std_function_callbacks",
          "[req][bdd][m3][REQ-SAX-03]") {
  std::vector<std::string> names;
  xmlparser::v1::SaxCallbacks callbacks;
  callbacks.on_start_element =
      [&](const xmlparser::v1::QualifiedName& name,
          const std::vector<xmlparser::v1::AttributeView>&) {
        names.push_back(name.qualified_name);
      };

  xmlparser::v1::parse("<root><child/></root>", callbacks);

  REQUIRE(names == std::vector<std::string>{"root", "child"});
}

TEST_CASE("REQ_SAX_03_allows_selective_callbacks",
          "[req][bdd][m3][REQ-SAX-03]") {
  std::string text;
  xmlparser::v1::SaxCallbacks callbacks;
  callbacks.on_characters = [&](std::string_view value) {
    text += value;
  };

  xmlparser::v1::parse("<root>a<child/>b</root>", callbacks);

  REQUIRE(text == "ab");
}
