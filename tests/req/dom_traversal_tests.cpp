#include <catch2/catch_test_macros.hpp>

#include <xmlparser/xmlparser.h>

#include <vector>

TEST_CASE("REQ_DOM_03_returns_parent_children_and_siblings",
          "[req][bdd][m4][REQ-DOM-03]") {
  xmlparser::v1::Document document =
      xmlparser::v1::parse("<root><a/><b/><c/></root>");
  const auto* root = document.document_element();

  REQUIRE(root != nullptr);
  REQUIRE(root->parent() == &document);
  REQUIRE(root->children().size() == 3);
  REQUIRE(root->children()[1]->previous_sibling() == root->children()[0]);
  REQUIRE(root->children()[1]->next_sibling() == root->children()[2]);
  REQUIRE(root->children()[0]->previous_sibling() == nullptr);
  REQUIRE(root->children()[2]->next_sibling() == nullptr);
}

TEST_CASE("REQ_DOM_03_depth_first_iterator_visits_nodes_in_document_order",
          "[req][bdd][m4][REQ-DOM-03]") {
  xmlparser::v1::Document document =
      xmlparser::v1::parse("<root><a><b/></a><c/></root>");

  std::vector<std::string> names;
  for (const auto* node : document.depth_first()) {
    if (node->type() == xmlparser::v1::NodeType::Element) {
      names.push_back(dynamic_cast<const xmlparser::v1::Element*>(node)->qualified_name());
    }
  }

  REQUIRE(names == std::vector<std::string>{"root", "a", "b", "c"});
}
