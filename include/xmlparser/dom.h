#pragma once

#include <string>

namespace xmlparser::v1 {

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
  virtual ~Node() = default;

  virtual NodeType type() const noexcept {
    return NodeType::Document;
  }
};

class Element : public Node {
 public:
  NodeType type() const noexcept override {
    return NodeType::Element;
  }
};

struct Attribute {
  std::string name;
  std::string value;
};

class Text : public Node {
 public:
  NodeType type() const noexcept override {
    return NodeType::Text;
  }
};

class Comment : public Node {
 public:
  NodeType type() const noexcept override {
    return NodeType::Comment;
  }
};

class ProcessingInstruction : public Node {
 public:
  NodeType type() const noexcept override {
    return NodeType::ProcessingInstruction;
  }
};

class CDataSection : public Node {
 public:
  NodeType type() const noexcept override {
    return NodeType::CDataSection;
  }
};

class Document : public Node {
 public:
  NodeType type() const noexcept override {
    return NodeType::Document;
  }
};

}  // namespace xmlparser::v1
