#include <catch2/catch_test_macros.hpp>

#include <stdexcept>
#include <string>
#include <vector>

#include "parser_event_recorder.h"

TEST_CASE("REQ_ERR_03_recoverable_callback_observes_validity_error",
          "[req][bdd][m5][REQ-ERR-03]") {
  xmlparser::v1::ParserOptions options;
  options.validation = xmlparser::v1::ValidationMode::Dtd;

  std::vector<std::string> messages;
  options.recoverable_error_handler =
      [&](const xmlparser::v1::XmlParseException& error) {
        REQUIRE(error.kind() == xmlparser::v1::ErrorKind::Validity);
        REQUIRE(error.location().line > 0);
        messages.push_back(error.what());
      };

  RecordingHandler handler;
  xmlparser::v1::parse(
      "<!DOCTYPE root [<!ELEMENT root EMPTY>]><root>content</root>", handler,
      options);

  REQUIRE(messages.size() == 1);
  REQUIRE(messages.front() == "EMPTY element has content");
  REQUIRE(has_event(handler, "characters", "content"));
  REQUIRE(handler.events.back().type == "end_document");
}

TEST_CASE("REQ_ERR_03_recoverable_callback_throw_stops_parse",
          "[req][bdd][m5][REQ-ERR-03]") {
  xmlparser::v1::ParserOptions options;
  options.validation = xmlparser::v1::ValidationMode::Dtd;
  options.recoverable_error_handler =
      [](const xmlparser::v1::XmlParseException&) {
        throw std::runtime_error("recoverable callback failed");
      };

  RecordingHandler handler;
  REQUIRE_THROWS_AS(xmlparser::v1::parse(
                        "<!DOCTYPE root [<!ELEMENT root EMPTY>]>"
                        "<root>content</root>",
                        handler, options),
                    std::runtime_error);
}

TEST_CASE("REQ_ERR_03_non_recoverable_well_formedness_error_stops_parse",
          "[req][bdd][m3][REQ-ERR-03]") {
  RecordingHandler handler;
  xmlparser::v1::ParserOptions options;
  int recoverable_errors = 0;
  options.recoverable_error_handler =
      [&](const xmlparser::v1::XmlParseException&) { ++recoverable_errors; };

  try {
    xmlparser::v1::parse("<root><child></root>", handler, options);
    FAIL("well-formedness error was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::WellFormedness);
    REQUIRE(recoverable_errors == 0);
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
