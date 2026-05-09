#include <catch2/catch_test_macros.hpp>

#include <xmlparser/xmlparser.h>

TEST_CASE("REQ_DOM_01_builds_document_element_attribute_text_comment_pi_cdata_nodes",
          "[req][bdd][m4][REQ-DOM-01]") {
  xmlparser::v1::Document document = xmlparser::v1::parse(
      "<?work go?><root plain=\"x\"><child>text</child><!--c--><![CDATA[d]]></root>");

  REQUIRE(document.type() == xmlparser::v1::NodeType::Document);
  REQUIRE(document.document_element() != nullptr);

  const auto* root = document.document_element();
  REQUIRE(root->type() == xmlparser::v1::NodeType::Element);
  REQUIRE(root->qualified_name() == "root");
  REQUIRE(root->get_attribute("plain") == "x");

  REQUIRE(document.children().size() == 2);
  REQUIRE(document.children()[0]->type() == xmlparser::v1::NodeType::ProcessingInstruction);
  REQUIRE(document.children()[1] == root);

  REQUIRE(root->children().size() == 3);
  REQUIRE(root->children()[0]->type() == xmlparser::v1::NodeType::Element);
  REQUIRE(root->children()[1]->type() == xmlparser::v1::NodeType::Comment);
  REQUIRE(root->children()[2]->type() == xmlparser::v1::NodeType::CDataSection);

  const auto* child = dynamic_cast<const xmlparser::v1::Element*>(root->children()[0]);
  REQUIRE(child != nullptr);
  REQUIRE(child->children()[0]->type() == xmlparser::v1::NodeType::Text);
}

TEST_CASE("REQ_DOM_01_preserves_node_type_identity_after_parse",
          "[req][bdd][m4][REQ-DOM-01]") {
  xmlparser::v1::Document document =
      xmlparser::v1::parse("<root><child/>plain<![CDATA[cdata]]></root>");
  const auto* root = document.document_element();

  REQUIRE(root != nullptr);
  REQUIRE(root->type() == xmlparser::v1::NodeType::Element);
  REQUIRE(root->children()[0]->type() == xmlparser::v1::NodeType::Element);
  REQUIRE(root->children()[1]->type() == xmlparser::v1::NodeType::Text);
  REQUIRE(root->children()[2]->type() == xmlparser::v1::NodeType::CDataSection);
}
