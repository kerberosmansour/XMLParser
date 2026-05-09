#include <catch2/catch_test_macros.hpp>

#include <xmlparser/xmlparser.h>

#include <sstream>

TEST_CASE("REQ_DOM_04_serializes_to_string", "[req][bdd][m4][REQ-DOM-04]") {
  xmlparser::v1::Document document;
  auto& root = document.create_element("root");
  root.set_attribute("plain", "A&B");
  root.append_child(document.create_text("<hello>&goodbye"));
  document.append_child(root);

  REQUIRE(xmlparser::v1::serialize(document) ==
          "<root plain=\"A&amp;B\">&lt;hello&gt;&amp;goodbye</root>");
}

TEST_CASE("REQ_DOM_04_serializes_to_stream", "[req][bdd][m4][REQ-DOM-04]") {
  xmlparser::v1::Document document = xmlparser::v1::parse("<root><child/></root>");
  std::ostringstream output;

  xmlparser::v1::serialize(document, output);

  REQUIRE(output.str() == "<root><child/></root>");
}

TEST_CASE("REQ_DOM_04_preserves_namespace_declarations",
          "[req][bdd][m4][REQ-DOM-04]") {
  xmlparser::v1::Document document =
      xmlparser::v1::parse("<p:root xmlns:p=\"urn:p\" p:code=\"7\"/>");

  const std::string output = xmlparser::v1::serialize(document);

  REQUIRE(output.find("<p:root") != std::string::npos);
  REQUIRE(output.find("xmlns:p=\"urn:p\"") != std::string::npos);
  REQUIRE(output.find("p:code=\"7\"") != std::string::npos);
}

TEST_CASE("REQ_DOM_04_reports_stream_failure",
          "[req][bdd][m4][REQ-DOM-04]") {
  xmlparser::v1::Document document = xmlparser::v1::parse("<root/>");
  std::ostringstream output;
  output.setstate(std::ios::badbit);

  try {
    xmlparser::v1::serialize(document, output);
    FAIL("failed stream was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::Io);
  }
}
