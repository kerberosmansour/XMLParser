#pragma once

#include <memory>
#include <string>
#include <vector>

#include <xmlparser/errors.h>
#include <xmlparser/sax.h>

namespace xmlparser::v1 {

class Document;
class Node;
class Element;

enum class NodeType {
  Document,
  Element,
  Attribute,
  Text,
  Comment,
  ProcessingInstruction,
  CDataSection,
};

class Node {
 public:
  Node(Document* owner, NodeType type);
  virtual ~Node() = default;

  NodeType type() const noexcept;
  Document* owner_document() const noexcept;
  Node* parent() noexcept;
  const Node* parent() const noexcept;
  const std::vector<Node*>& children() const noexcept;
  Node* previous_sibling() noexcept;
  const Node* previous_sibling() const noexcept;
  Node* next_sibling() noexcept;
  const Node* next_sibling() const noexcept;

  void append_child(Node& child);
  void remove_child(Node& child);
  void set_owner_document(Document* owner) noexcept;
  void set_parent(Node* parent) noexcept;
  std::vector<Node*>& mutable_children() noexcept;

 private:
  Document* owner_;
  NodeType type_;
  Node* parent_ = nullptr;
  std::vector<Node*> children_;
};

struct Attribute {
  std::string namespace_uri;
  std::string local_name;
  std::string qualified_name;
  std::string value;
};

class Element : public Node {
 public:
  Element(Document* owner, std::string qualified_name,
          std::string namespace_uri = {}, std::string local_name = {});

  const std::string& qualified_name() const noexcept;
  const std::string& namespace_uri() const noexcept;
  const std::string& local_name() const noexcept;

  const std::vector<Attribute>& attributes() const noexcept;
  void set_attribute(std::string name, std::string value);
  std::string get_attribute(const std::string& name) const;
  void set_attribute_ns(std::string namespace_uri,
                        std::string local_name,
                        std::string qualified_name,
                        std::string value);
  std::string get_attribute_ns(const std::string& namespace_uri,
                               const std::string& local_name) const;

 private:
  std::string qualified_name_;
  std::string namespace_uri_;
  std::string local_name_;
  std::vector<Attribute> attributes_;
};

class Text : public Node {
 public:
  Text(Document* owner, std::string text);

  const std::string& text() const noexcept;
  void set_text(std::string text);

 private:
  std::string text_;
};

class Comment : public Node {
 public:
  Comment(Document* owner, std::string text);

  const std::string& text() const noexcept;

 private:
  std::string text_;
};

class ProcessingInstruction : public Node {
 public:
  ProcessingInstruction(Document* owner, std::string target, std::string data);

  const std::string& target() const noexcept;
  const std::string& data() const noexcept;

 private:
  std::string target_;
  std::string data_;
};

class CDataSection : public Node {
 public:
  CDataSection(Document* owner, std::string text);

  const std::string& text() const noexcept;

 private:
  std::string text_;
};

class Document : public Node {
 public:
  Document();
  Document(Document&& other) noexcept;
  Document& operator=(Document&& other) noexcept;
  Document(const Document&) = delete;
  Document& operator=(const Document&) = delete;
  ~Document() override;

  Element& create_element(std::string qualified_name,
                          std::string namespace_uri = {},
                          std::string local_name = {});
  Text& create_text(std::string text);
  Comment& create_comment(std::string text);
  ProcessingInstruction& create_processing_instruction(std::string target,
                                                       std::string data);
  CDataSection& create_cdata_section(std::string text);

  Element* document_element() noexcept;
  const Element* document_element() const noexcept;
  std::vector<Node*> depth_first() noexcept;
  std::vector<const Node*> depth_first() const;

 private:
  template <typename T, typename... Args>
  T& make_node(Args&&... args);

  std::vector<std::unique_ptr<Node>> storage_;
};

}  // namespace xmlparser::v1
