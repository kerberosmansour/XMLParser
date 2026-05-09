#include <catch2/catch_test_macros.hpp>

#include <xmlparser/xmlparser.h>

#include <string>

TEST_CASE("REQ_ERR_06_empty_input_returns_defined_error",
          "[req][bdd][m2][REQ-ERR-06]") {
  try {
    xmlparser::v1::parse("");
    FAIL("empty input was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::EmptyInput);
    REQUIRE(error.location().line == 1U);
    REQUIRE(error.location().column == 1U);
    REQUIRE(error.location().byte_offset == 0U);
  }
}

TEST_CASE("REQ_ERR_06_truncated_input_returns_defined_error",
          "[req][bdd][m2][REQ-ERR-06]") {
  try {
    xmlparser::v1::parse("<root><![CDATA[unfinished");
    FAIL("truncated CDATA was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::WellFormedness);
    REQUIRE(error.location().byte_offset > 0U);
  }
}

TEST_CASE("REQ_ERR_06_secret_payload_is_not_echoed",
          "[req][bdd][m2][REQ-ERR-06][REQ-SEC-02]") {
  const std::string xml = "<root><password>hunter2</password><child></root>";

  try {
    xmlparser::v1::parse(xml);
    FAIL("malformed XML was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    const std::string message = error.what();
    REQUIRE(message.find(xml) == std::string::npos);
    REQUIRE(message.find("hunter2") == std::string::npos);
    REQUIRE(message.find("password") == std::string::npos);
  }
}

TEST_CASE("REQ_ERR_06_document_byte_limit_returns_resource_error",
          "[req][bdd][m5][REQ-ERR-06][REQ-ERR-05]") {
  xmlparser::v1::ParserOptions options;
  options.max_document_bytes = 4;

  try {
    xmlparser::v1::parse("<root/>", options);
    FAIL("oversized document was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::ResourceLimit);
  }
}

TEST_CASE("REQ_ERR_06_rejects_extra_content_after_document_element",
          "[req][bdd][m5][REQ-ERR-06]") {
  try {
    xmlparser::v1::parse("<root/><extra/>");
    FAIL("extra root content was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::WellFormedness);
  }
}

TEST_CASE("REQ_ERR_06_allows_misc_before_document_element",
          "[req][bdd][m5][REQ-ERR-06]") {
  REQUIRE_NOTHROW((void)xmlparser::v1::parse("<!--lead--><?work ok?><root/>"));
}

TEST_CASE("REQ_ERR_06_rejects_invalid_xml_declaration",
          "[req][bdd][m5][REQ-ERR-06]") {
  try {
    xmlparser::v1::parse("<?xml?><root/>");
    FAIL("invalid XML declaration was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::WellFormedness);
  }
}

TEST_CASE("REQ_ERR_06_rejects_unsupported_xml_declaration_version",
          "[req][bdd][m5][REQ-ERR-06]") {
  try {
    xmlparser::v1::parse("<?xml version=\"2.0\"?><root/>");
    FAIL("unsupported XML declaration version was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::WellFormedness);
  }
}

TEST_CASE("REQ_ERR_06_rejects_missing_element_start",
          "[req][bdd][m5][REQ-ERR-06]") {
  try {
    xmlparser::v1::parse("</root>");
    FAIL("end tag without start tag was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::WellFormedness);
  }
}

TEST_CASE("REQ_ERR_06_rejects_invalid_element_name",
          "[req][bdd][m5][REQ-ERR-06]") {
  try {
    xmlparser::v1::parse("<1root/>");
    FAIL("invalid element name was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::WellFormedness);
  }
}

TEST_CASE("REQ_ERR_06_rejects_processing_instruction_xml_target_in_content",
          "[req][bdd][m5][REQ-ERR-06]") {
  try {
    xmlparser::v1::parse("<root><?xml nope?></root>");
    FAIL("reserved XML PI target was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::WellFormedness);
  }
}

TEST_CASE("REQ_ERR_06_rejects_comment_double_hyphen",
          "[req][bdd][m5][REQ-ERR-06]") {
  try {
    xmlparser::v1::parse("<root><!--bad--comment--></root>");
    FAIL("comment double hyphen was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::WellFormedness);
  }
}
