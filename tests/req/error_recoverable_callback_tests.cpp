#include <catch2/catch_test_macros.hpp>

#include <stdexcept>

#include "parser_event_recorder.h"

TEST_CASE("REQ_ERR_03_non_recoverable_well_formedness_error_stops_parse",
          "[req][bdd][m3][REQ-ERR-03]") {
  RecordingHandler handler;

  try {
    xmlparser::v1::parse("<root><child></root>", handler);
    FAIL("well-formedness error was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::WellFormedness);
  }
}

TEST_CASE("REQ_ERR_05_callback_throw_does_not_leak",
          "[req][bdd][m3][REQ-ERR-05]") {
  class ThrowingHandler : public RecordingHandler {
   public:
    void start_element(
        const xmlparser::v1::QualifiedName& name,
        const std::vector<xmlparser::v1::AttributeView>& attributes) override {
      RecordingHandler::start_element(name, attributes);
      if (name.qualified_name == "child") {
        throw std::runtime_error("callback failed");
      }
    }
  };

  ThrowingHandler throwing_handler;
  REQUIRE_THROWS_AS(xmlparser::v1::parse("<root><child/></root>", throwing_handler),
                    std::runtime_error);

  RecordingHandler fresh_handler;
  REQUIRE_NOTHROW(xmlparser::v1::parse("<root/>", fresh_handler));
  REQUIRE(fresh_handler.events[1].name == "root");
}
