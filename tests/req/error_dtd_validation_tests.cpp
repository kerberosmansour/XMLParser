#include <catch2/catch_test_macros.hpp>

#include <xmlparser/xmlparser.h>

TEST_CASE("REQ_ERR_04_validates_internal_dtd_subset",
          "[req][bdd][m5][REQ-ERR-04]") {
  xmlparser::v1::ParserOptions options;
  options.validation = xmlparser::v1::ValidationMode::Dtd;

  REQUIRE_NOTHROW((void)xmlparser::v1::parse(
      "<!DOCTYPE root [<!ELEMENT root (child)> <!ELEMENT child EMPTY>]>"
      "<root><child/></root>",
      options));
}

TEST_CASE("REQ_ERR_04_resolves_internal_general_entity",
          "[req][bdd][m5][REQ-ERR-04]") {
  xmlparser::v1::ParserOptions options;
  options.validation = xmlparser::v1::ValidationMode::Dtd;

  xmlparser::v1::Document document = xmlparser::v1::parse(
      "<!DOCTYPE root [<!ELEMENT root (#PCDATA)> <!ENTITY company \"XMLParser\">]>"
      "<root>&company;</root>",
      options);

  REQUIRE(document.document_element() != nullptr);
  REQUIRE(document.document_element()->children().size() == 1);
  const auto* text =
      dynamic_cast<const xmlparser::v1::Text*>(document.document_element()->children()[0]);
  REQUIRE(text != nullptr);
  REQUIRE(text->text() == "XMLParser");
}

TEST_CASE("REQ_ERR_04_rejects_recursive_entity_expansion",
          "[req][bdd][m5][REQ-ERR-04][REQ-ERR-05]") {
  xmlparser::v1::ParserOptions options;
  options.validation = xmlparser::v1::ValidationMode::Dtd;
  options.max_entity_expansions = 8;

  try {
    (void)xmlparser::v1::parse(
        "<!DOCTYPE root [<!ELEMENT root (#PCDATA)> <!ENTITY loop \"&loop;\">]>"
        "<root>&loop;</root>",
        options);
    FAIL("recursive entity expansion was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE((error.kind() == xmlparser::v1::ErrorKind::ResourceLimit ||
             error.kind() == xmlparser::v1::ErrorKind::Validity));
  }
}

TEST_CASE("REQ_ERR_04_rejects_duplicate_dtd_declarations",
          "[req][bdd][m5][REQ-ERR-04]") {
  xmlparser::v1::ParserOptions options;
  options.validation = xmlparser::v1::ValidationMode::Dtd;

  try {
    (void)xmlparser::v1::parse(
        "<!DOCTYPE root [<!ELEMENT root EMPTY> <!ELEMENT root ANY>]><root/>",
        options);
    FAIL("duplicate DTD declaration was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::Validity);
  }
}

TEST_CASE("REQ_ERR_04_allows_any_element_model",
          "[req][bdd][m5][REQ-ERR-04]") {
  xmlparser::v1::ParserOptions options;
  options.validation = xmlparser::v1::ValidationMode::Dtd;

  REQUIRE_NOTHROW((void)xmlparser::v1::parse(
      "<!DOCTYPE root [<!ELEMENT root ANY> <!ELEMENT child EMPTY>]>"
      "<root>text<child/></root>",
      options));
}

TEST_CASE("REQ_ERR_04_rejects_pcdata_element_children",
          "[req][bdd][m5][REQ-ERR-04]") {
  xmlparser::v1::ParserOptions options;
  options.validation = xmlparser::v1::ValidationMode::Dtd;

  try {
    (void)xmlparser::v1::parse(
        "<!DOCTYPE root [<!ELEMENT root (#PCDATA)> <!ELEMENT child EMPTY>]>"
        "<root><child/></root>",
        options);
    FAIL("PCDATA model accepted child element");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::Validity);
  }
}

TEST_CASE("REQ_ERR_04_rejects_doctype_root_mismatch",
          "[req][bdd][m5][REQ-ERR-04]") {
  xmlparser::v1::ParserOptions options;
  options.validation = xmlparser::v1::ValidationMode::Dtd;

  try {
    (void)xmlparser::v1::parse(
        "<!DOCTYPE expected [<!ELEMENT expected EMPTY>]><actual/>", options);
    FAIL("DOCTYPE/root mismatch was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::Validity);
  }
}

TEST_CASE("REQ_ERR_04_rejects_unsupported_dtd_declaration",
          "[req][bdd][m5][REQ-ERR-04]") {
  xmlparser::v1::ParserOptions options;
  options.validation = xmlparser::v1::ValidationMode::Dtd;

  try {
    (void)xmlparser::v1::parse(
        "<!DOCTYPE root [<!NOTATION jpg SYSTEM \"image/jpeg\">]><root/>",
        options);
    FAIL("unsupported DTD declaration was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::WellFormedness);
  }
}

TEST_CASE("REQ_ERR_04_rejects_doctype_without_required_whitespace",
          "[req][bdd][m5][REQ-ERR-04]") {
  try {
    (void)xmlparser::v1::parse("<!DOCTYPEroot [<!ELEMENT root EMPTY>]><root/>");
    FAIL("DOCTYPE without whitespace was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::WellFormedness);
  }
}

TEST_CASE("REQ_ERR_04_rejects_unquoted_external_identifier",
          "[req][bdd][m5][REQ-ERR-04]") {
  try {
    (void)xmlparser::v1::parse("<!DOCTYPE root SYSTEM urn:test><root/>");
    FAIL("unquoted external identifier was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::WellFormedness);
  }
}

TEST_CASE("REQ_ERR_04_rejects_element_declaration_without_whitespace",
          "[req][bdd][m5][REQ-ERR-04]") {
  try {
    (void)xmlparser::v1::parse("<!DOCTYPE root [<!ELEMENTroot EMPTY>]><root/>");
    FAIL("ELEMENT declaration without whitespace was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::WellFormedness);
  }
}

TEST_CASE("REQ_ERR_04_rejects_unsupported_element_model",
          "[req][bdd][m5][REQ-ERR-04]") {
  try {
    (void)xmlparser::v1::parse("<!DOCTYPE root [<!ELEMENT root BADMODEL>]><root/>");
    FAIL("unsupported element model was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::Validity);
  }
}

TEST_CASE("REQ_ERR_04_rejects_entity_declaration_without_whitespace",
          "[req][bdd][m5][REQ-ERR-04]") {
  try {
    (void)xmlparser::v1::parse("<!DOCTYPE root [<!ENTITYbad \"x\">]><root/>");
    FAIL("ENTITY declaration without whitespace was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::WellFormedness);
  }
}

TEST_CASE("REQ_ERR_04_rejects_duplicate_entity_declaration",
          "[req][bdd][m5][REQ-ERR-04]") {
  try {
    (void)xmlparser::v1::parse(
        "<!DOCTYPE root [<!ELEMENT root EMPTY> <!ENTITY x \"1\"> <!ENTITY x \"2\">]>"
        "<root/>");
    FAIL("duplicate entity declaration was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::Validity);
  }
}

TEST_CASE("REQ_ERR_04_rejects_too_many_dtd_declarations",
          "[req][bdd][m5][REQ-ERR-04][REQ-ERR-05]") {
  xmlparser::v1::ParserOptions options;
  options.validation = xmlparser::v1::ValidationMode::Dtd;
  options.max_dtd_declarations = 1;

  try {
    (void)xmlparser::v1::parse(
        "<!DOCTYPE root [<!ELEMENT root (child)> <!ELEMENT child EMPTY>]>"
        "<root><child/></root>",
        options);
    FAIL("DTD declaration limit was not enforced");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::ResourceLimit);
  }
}

TEST_CASE("REQ_ERR_04_rejects_oversized_entity_replacement",
          "[req][bdd][m5][REQ-ERR-04][REQ-ERR-05]") {
  xmlparser::v1::ParserOptions options;
  options.validation = xmlparser::v1::ValidationMode::Dtd;
  options.max_entity_replacement_bytes = 2;

  try {
    (void)xmlparser::v1::parse(
        "<!DOCTYPE root [<!ELEMENT root (#PCDATA)> <!ENTITY big \"abcd\">]>"
        "<root/>",
        options);
    FAIL("oversized entity declaration was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::ResourceLimit);
  }
}

TEST_CASE("REQ_ERR_04_rejects_parameter_entities",
          "[req][bdd][m5][REQ-ERR-04]") {
  xmlparser::v1::ParserOptions options;
  options.validation = xmlparser::v1::ValidationMode::Dtd;

  try {
    (void)xmlparser::v1::parse(
        "<!DOCTYPE root [<!ENTITY % bad \"value\">]><root/>", options);
    FAIL("parameter entity was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::WellFormedness);
  }
}

TEST_CASE("REQ_ERR_04_handles_dtd_comments",
          "[req][bdd][m5][REQ-ERR-04]") {
  xmlparser::v1::ParserOptions options;
  options.validation = xmlparser::v1::ValidationMode::Dtd;

  REQUIRE_NOTHROW((void)xmlparser::v1::parse(
      "<!DOCTYPE root [<!--ok--><!ELEMENT root EMPTY>]><root/>", options));
}

TEST_CASE("REQ_ERR_04_external_dtd_resolver_is_not_called_by_default",
          "[req][bdd][m5][REQ-ERR-04]") {
  xmlparser::v1::ParserOptions options;
  options.validation = xmlparser::v1::ValidationMode::Dtd;
  bool called = false;
  options.external_dtd_resolver = [&](std::string_view) {
    called = true;
    return std::string{"<!ELEMENT root EMPTY>"};
  };

  try {
    (void)xmlparser::v1::parse("<!DOCTYPE root SYSTEM \"urn:test\"><root/>",
                               options);
    FAIL("external DTD was accepted without opt-in");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::Validity);
  }

  REQUIRE_FALSE(called);
}

TEST_CASE("REQ_ERR_04_external_dtd_resolver_is_called_only_when_configured",
          "[req][bdd][m5][REQ-ERR-04]") {
  xmlparser::v1::ParserOptions options;
  options.validation = xmlparser::v1::ValidationMode::Dtd;
  options.allow_external_dtd = true;
  bool called = false;
  options.external_dtd_resolver = [&](std::string_view system_id) {
    called = true;
    REQUIRE(std::string(system_id) == "urn:test");
    return std::string{"<!ELEMENT root EMPTY>"};
  };

  REQUIRE_NOTHROW(
      (void)xmlparser::v1::parse("<!DOCTYPE root SYSTEM \"urn:test\"><root/>",
                                 options));
  REQUIRE(called);
}

TEST_CASE("REQ_ERR_04_external_dtd_resolver_is_bounded",
          "[req][bdd][m5][REQ-ERR-04][REQ-ERR-05]") {
  xmlparser::v1::ParserOptions options;
  options.validation = xmlparser::v1::ValidationMode::Dtd;
  options.allow_external_dtd = true;
  options.max_external_subset_bytes = 4;
  options.external_dtd_resolver = [](std::string_view) {
    return std::string{"<!ELEMENT root EMPTY>"};
  };

  try {
    (void)xmlparser::v1::parse("<!DOCTYPE root SYSTEM \"urn:test\"><root/>",
                               options);
    FAIL("oversized external subset was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::ResourceLimit);
  }
}

TEST_CASE("REQ_ERR_04_rejects_truncated_external_subset",
          "[req][bdd][m5][REQ-ERR-04]") {
  xmlparser::v1::ParserOptions options;
  options.validation = xmlparser::v1::ValidationMode::Dtd;
  options.allow_external_dtd = true;
  options.external_dtd_resolver = [](std::string_view) {
    return std::string{"<!ELEMENT root EMPTY"};
  };

  try {
    (void)xmlparser::v1::parse("<!DOCTYPE root SYSTEM \"urn:test\"><root/>",
                               options);
    FAIL("truncated external subset was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::Validity);
  }
}

TEST_CASE("REQ_ERR_04_rejects_unsupported_external_subset_declaration",
          "[req][bdd][m5][REQ-ERR-04]") {
  xmlparser::v1::ParserOptions options;
  options.validation = xmlparser::v1::ValidationMode::Dtd;
  options.allow_external_dtd = true;
  options.external_dtd_resolver = [](std::string_view) {
    return std::string{"<!ENTITY x \"y\">"};
  };

  try {
    (void)xmlparser::v1::parse("<!DOCTYPE root SYSTEM \"urn:test\"><root/>",
                               options);
    FAIL("unsupported external subset declaration was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::Validity);
  }
}

TEST_CASE("REQ_ERR_04_rejects_duplicate_external_element_declaration",
          "[req][bdd][m5][REQ-ERR-04]") {
  xmlparser::v1::ParserOptions options;
  options.validation = xmlparser::v1::ValidationMode::Dtd;
  options.allow_external_dtd = true;
  options.external_dtd_resolver = [](std::string_view) {
    return std::string{"<!ELEMENT root EMPTY>"};
  };

  try {
    (void)xmlparser::v1::parse(
        "<!DOCTYPE root SYSTEM \"urn:test\" [<!ELEMENT root EMPTY>]><root/>",
        options);
    FAIL("duplicate external element declaration was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::Validity);
  }
}

TEST_CASE("REQ_ERR_04_empty_external_subset_leaves_validation_active",
          "[req][bdd][m5][REQ-ERR-04]") {
  xmlparser::v1::ParserOptions options;
  options.validation = xmlparser::v1::ValidationMode::Dtd;
  options.allow_external_dtd = true;
  options.external_dtd_resolver = [](std::string_view) {
    return std::string{"   "};
  };

  try {
    (void)xmlparser::v1::parse("<!DOCTYPE root SYSTEM \"urn:test\"><root/>",
                               options);
    FAIL("missing external declarations were accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::Validity);
  }
}

TEST_CASE("REQ_ERR_04_rejects_empty_element_text_content",
          "[req][bdd][m5][REQ-ERR-04]") {
  xmlparser::v1::ParserOptions options;
  options.validation = xmlparser::v1::ValidationMode::Dtd;

  try {
    (void)xmlparser::v1::parse(
        "<!DOCTYPE root [<!ELEMENT root EMPTY>]><root>text</root>", options);
    FAIL("EMPTY element text content was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::Validity);
  }
}

TEST_CASE("REQ_ERR_04_rejects_wrong_child_sequence",
          "[req][bdd][m5][REQ-ERR-04]") {
  xmlparser::v1::ParserOptions options;
  options.validation = xmlparser::v1::ValidationMode::Dtd;

  try {
    (void)xmlparser::v1::parse(
        "<!DOCTYPE root [<!ELEMENT root (a,b)> <!ELEMENT a EMPTY> <!ELEMENT b EMPTY>]>"
        "<root><b/><a/></root>",
        options);
    FAIL("wrong child sequence was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::Validity);
  }
}

TEST_CASE("REQ_ERR_04_declared_entity_expands_predefined_entities",
          "[req][bdd][m5][REQ-ERR-04]") {
  xmlparser::v1::ParserOptions options;
  options.validation = xmlparser::v1::ValidationMode::Dtd;

  xmlparser::v1::Document document = xmlparser::v1::parse(
      "<!DOCTYPE root [<!ELEMENT root (#PCDATA)> "
      "<!ENTITY marks \"&lt;&gt;&amp;&apos;&quot;\">]><root>&marks;</root>",
      options);

  const auto* text =
      dynamic_cast<const xmlparser::v1::Text*>(document.document_element()->children()[0]);
  REQUIRE(text != nullptr);
  REQUIRE(text->text() == "<>&'\"");
}

TEST_CASE("REQ_ERR_04_rejects_unknown_nested_entity_reference",
          "[req][bdd][m5][REQ-ERR-04]") {
  xmlparser::v1::ParserOptions options;
  options.validation = xmlparser::v1::ValidationMode::Dtd;

  try {
    (void)xmlparser::v1::parse(
        "<!DOCTYPE root [<!ELEMENT root (#PCDATA)> <!ENTITY x \"&missing;\">]>"
        "<root>&x;</root>",
        options);
    FAIL("unknown nested entity was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::Validity);
  }
}

TEST_CASE("REQ_ERR_04_public_external_dtd_resolver_is_called_when_configured",
          "[req][bdd][m5][REQ-ERR-04]") {
  xmlparser::v1::ParserOptions options;
  options.validation = xmlparser::v1::ValidationMode::Dtd;
  options.allow_external_dtd = true;
  bool called = false;
  options.external_dtd_resolver = [&](std::string_view system_id) {
    called = true;
    REQUIRE(std::string(system_id) == "urn:system");
    return std::string{"<!ELEMENT root EMPTY>"};
  };

  REQUIRE_NOTHROW((void)xmlparser::v1::parse(
      "<!DOCTYPE root PUBLIC \"urn:public\" \"urn:system\"><root/>", options));
  REQUIRE(called);
}
