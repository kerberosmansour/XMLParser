#include <catch2/catch_test_macros.hpp>

#include <xmlparser/xmlparser.h>

TEST_CASE("REQ_DOM_02_creates_inserts_modifies_and_removes_nodes",
          "[req][bdd][m4][REQ-DOM-02]") {
  xmlparser::v1::Document document;
  auto& root = document.create_element("root");
  auto& text = document.create_text("hello");

  document.append_child(root);
  root.append_child(text);
  text.set_text("goodbye");

  REQUIRE(document.document_element() == &root);
  REQUIRE(root.children().size() == 1);
  REQUIRE(text.text() == "goodbye");
  REQUIRE(text.parent() == &root);

  root.remove_child(text);
  REQUIRE(root.children().empty());
  REQUIRE(text.parent() == nullptr);
}

TEST_CASE("REQ_DOM_02_rejects_cycle_creating_insert",
          "[req][bdd][m4][REQ-DOM-02][REQ-ERR-05]") {
  xmlparser::v1::Document document;
  auto& root = document.create_element("root");
  auto& child = document.create_element("child");
  document.append_child(root);
  root.append_child(child);

  try {
    child.append_child(root);
    FAIL("cycle creating insert was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::WellFormedness);
  }

  REQUIRE(document.document_element() == &root);
  REQUIRE(root.parent() == &document);
  REQUIRE(child.parent() == &root);
}

TEST_CASE("REQ_DOM_02_rolls_back_failed_mutation",
          "[req][bdd][m4][REQ-DOM-02][REQ-ERR-05]") {
  xmlparser::v1::Document first;
  xmlparser::v1::Document second;
  auto& root = first.create_element("root");
  auto& foreign = second.create_element("foreign");
  first.append_child(root);

  try {
    root.append_child(foreign);
    FAIL("cross-document insert was accepted");
  } catch (const xmlparser::v1::XmlParseException& error) {
    REQUIRE(error.kind() == xmlparser::v1::ErrorKind::WellFormedness);
  }

  REQUIRE(root.children().empty());
  REQUIRE(foreign.parent() == nullptr);
}
