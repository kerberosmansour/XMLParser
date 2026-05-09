#include <xmlparser/dom.h>
#include <xmlparser/serializer.h>

#include <algorithm>
#include <memory>
#include <ostream>
#include <set>
#include <sstream>
#include <utility>

namespace xmlparser::v1 {
namespace {

[[noreturn]] void dom_error(std::string message) {
  throw XmlParseException(ErrorKind::WellFormedness, SourceLocation{},
                          std::move(message));
}

void update_owner(Node& node, Document* owner) {
  node.set_owner_document(owner);
  for (Node* child : node.mutable_children()) {
    update_owner(*child, owner);
  }
}

bool is_ancestor_of(const Node& possible_ancestor, const Node& node) {
  for (const Node* cursor = node.parent(); cursor != nullptr; cursor = cursor->parent()) {
    if (cursor == &possible_ancestor) {
      return true;
    }
  }
  return false;
}

void collect_depth_first(Node& node, std::vector<Node*>& nodes) {
  for (Node* child : node.children()) {
    nodes.push_back(child);
    collect_depth_first(*child, nodes);
  }
}

void collect_depth_first(const Node& node, std::vector<const Node*>& nodes) {
  for (const Node* child : node.children()) {
    nodes.push_back(child);
    collect_depth_first(*child, nodes);
  }
}

std::string escape_text(std::string_view text) {
  std::string escaped;
  for (char ch : text) {
    switch (ch) {
      case '&':
        escaped += "&amp;";
        break;
      case '<':
        escaped += "&lt;";
        break;
      case '>':
        escaped += "&gt;";
        break;
      default:
        escaped.push_back(ch);
        break;
    }
  }
  return escaped;
}

std::string escape_attribute(std::string_view text) {
  std::string escaped;
  for (char ch : text) {
    switch (ch) {
      case '&':
        escaped += "&amp;";
        break;
      case '<':
        escaped += "&lt;";
        break;
      case '"':
        escaped += "&quot;";
        break;
      default:
        escaped.push_back(ch);
        break;
    }
  }
  return escaped;
}

std::string prefix_of(const std::string& qname) {
  const std::size_t colon = qname.find(':');
  if (colon == std::string::npos) {
    return {};
  }
  return qname.substr(0, colon);
}

void serialize_node(const Node& node, std::ostream& output);

void serialize_element(const Element& element, std::ostream& output) {
  output << '<' << element.qualified_name();

  std::set<std::string> declared_prefixes;
  if (!element.namespace_uri().empty()) {
    const std::string prefix = prefix_of(element.qualified_name());
    output << ' ';
    if (prefix.empty()) {
      output << "xmlns=\"" << escape_attribute(element.namespace_uri()) << '"';
    } else {
      output << "xmlns:" << prefix << "=\"" << escape_attribute(element.namespace_uri())
             << '"';
    }
    declared_prefixes.insert(prefix);
  }

  for (const Attribute& attribute : element.attributes()) {
    const std::string prefix = prefix_of(attribute.qualified_name);
    if (!attribute.namespace_uri.empty() && declared_prefixes.insert(prefix).second) {
      output << " xmlns:" << prefix << "=\""
             << escape_attribute(attribute.namespace_uri) << '"';
    }
  }

  for (const Attribute& attribute : element.attributes()) {
    output << ' ' << attribute.qualified_name << "=\""
           << escape_attribute(attribute.value) << '"';
  }

  if (element.children().empty()) {
    output << "/>";
    return;
  }

  output << '>';
  for (const Node* child : element.children()) {
    serialize_node(*child, output);
  }
  output << "</" << element.qualified_name() << '>';
}

void serialize_node(const Node& node, std::ostream& output) {
  switch (node.type()) {
    case NodeType::Document:
      for (const Node* child : node.children()) {
        serialize_node(*child, output);
      }
      break;
    case NodeType::Element:
      serialize_element(static_cast<const Element&>(node), output);
      break;
    case NodeType::Text:
      output << escape_text(static_cast<const Text&>(node).text());
      break;
    case NodeType::Comment:
      output << "<!--" << static_cast<const Comment&>(node).text() << "-->";
      break;
    case NodeType::ProcessingInstruction: {
      const auto& pi = static_cast<const ProcessingInstruction&>(node);
      output << "<?" << pi.target();
      if (!pi.data().empty()) {
        output << ' ' << pi.data();
      }
      output << "?>";
      break;
    }
    case NodeType::CDataSection:
      output << "<![CDATA[" << static_cast<const CDataSection&>(node).text()
             << "]]>";
      break;
    case NodeType::Attribute:
      break;
  }
}

}  // namespace

Node::Node(Document* owner, NodeType type) : owner_(owner), type_(type) {}

NodeType Node::type() const noexcept {
  return type_;
}

Document* Node::owner_document() const noexcept {
  return owner_;
}

Node* Node::parent() noexcept {
  return parent_;
}

const Node* Node::parent() const noexcept {
  return parent_;
}

const std::vector<Node*>& Node::children() const noexcept {
  return children_;
}

Node* Node::previous_sibling() noexcept {
  if (parent_ == nullptr) {
    return nullptr;
  }
  const auto& siblings = parent_->children();
  const auto found = std::find(siblings.begin(), siblings.end(), this);
  if (found == siblings.begin() || found == siblings.end()) {
    return nullptr;
  }
  return *(found - 1);
}

const Node* Node::previous_sibling() const noexcept {
  return const_cast<Node*>(this)->previous_sibling();
}

Node* Node::next_sibling() noexcept {
  if (parent_ == nullptr) {
    return nullptr;
  }
  const auto& siblings = parent_->children();
  const auto found = std::find(siblings.begin(), siblings.end(), this);
  if (found == siblings.end() || found + 1 == siblings.end()) {
    return nullptr;
  }
  return *(found + 1);
}

const Node* Node::next_sibling() const noexcept {
  return const_cast<Node*>(this)->next_sibling();
}

void Node::append_child(Node& child) {
  if (child.type() == NodeType::Document) {
    dom_error("Document nodes cannot be inserted as children");
  }
  if (owner_ == nullptr || child.owner_document() != owner_) {
    dom_error("Cannot insert a node owned by another document");
  }
  if (&child == this || is_ancestor_of(child, *this)) {
    dom_error("Cannot create a DOM cycle");
  }
  if (type_ == NodeType::Document && child.type() == NodeType::Element) {
    for (const Node* existing : children_) {
      if (existing != &child && existing->type() == NodeType::Element) {
        dom_error("Document already has a document element");
      }
    }
  }

  if (child.parent_ != nullptr) {
    child.parent_->remove_child(child);
  }
  children_.push_back(&child);
  child.set_parent(this);
}

void Node::remove_child(Node& child) {
  const auto found = std::find(children_.begin(), children_.end(), &child);
  if (found == children_.end()) {
    dom_error("Node is not a child of this parent");
  }
  children_.erase(found);
  child.set_parent(nullptr);
}

void Node::set_owner_document(Document* owner) noexcept {
  owner_ = owner;
}

void Node::set_parent(Node* parent) noexcept {
  parent_ = parent;
}

std::vector<Node*>& Node::mutable_children() noexcept {
  return children_;
}

Element::Element(Document* owner,
                 std::string qualified_name,
                 std::string namespace_uri,
                 std::string local_name)
    : Node(owner, NodeType::Element),
      qualified_name_(std::move(qualified_name)),
      namespace_uri_(std::move(namespace_uri)),
      local_name_(std::move(local_name)) {
  if (local_name_.empty()) {
    const std::size_t colon = qualified_name_.find(':');
    local_name_ =
        colon == std::string::npos ? qualified_name_ : qualified_name_.substr(colon + 1);
  }
}

const std::string& Element::qualified_name() const noexcept {
  return qualified_name_;
}

const std::string& Element::namespace_uri() const noexcept {
  return namespace_uri_;
}

const std::string& Element::local_name() const noexcept {
  return local_name_;
}

const std::vector<Attribute>& Element::attributes() const noexcept {
  return attributes_;
}

void Element::set_attribute(std::string name, std::string value) {
  set_attribute_ns({}, name, name, std::move(value));
}

std::string Element::get_attribute(const std::string& name) const {
  for (const auto& attribute : attributes_) {
    if (attribute.qualified_name == name) {
      return attribute.value;
    }
  }
  return {};
}

void Element::set_attribute_ns(std::string namespace_uri,
                               std::string local_name,
                               std::string qualified_name,
                               std::string value) {
  for (auto& attribute : attributes_) {
    if (attribute.namespace_uri == namespace_uri && attribute.local_name == local_name) {
      attribute.qualified_name = std::move(qualified_name);
      attribute.value = std::move(value);
      return;
    }
  }
  attributes_.push_back({std::move(namespace_uri), std::move(local_name),
                         std::move(qualified_name), std::move(value)});
}

std::string Element::get_attribute_ns(const std::string& namespace_uri,
                                      const std::string& local_name) const {
  for (const auto& attribute : attributes_) {
    if (attribute.namespace_uri == namespace_uri && attribute.local_name == local_name) {
      return attribute.value;
    }
  }
  return {};
}

Text::Text(Document* owner, std::string text)
    : Node(owner, NodeType::Text), text_(std::move(text)) {}

const std::string& Text::text() const noexcept {
  return text_;
}

void Text::set_text(std::string text) {
  text_ = std::move(text);
}

Comment::Comment(Document* owner, std::string text)
    : Node(owner, NodeType::Comment), text_(std::move(text)) {}

const std::string& Comment::text() const noexcept {
  return text_;
}

ProcessingInstruction::ProcessingInstruction(Document* owner,
                                             std::string target,
                                             std::string data)
    : Node(owner, NodeType::ProcessingInstruction),
      target_(std::move(target)),
      data_(std::move(data)) {}

const std::string& ProcessingInstruction::target() const noexcept {
  return target_;
}

const std::string& ProcessingInstruction::data() const noexcept {
  return data_;
}

CDataSection::CDataSection(Document* owner, std::string text)
    : Node(owner, NodeType::CDataSection), text_(std::move(text)) {}

const std::string& CDataSection::text() const noexcept {
  return text_;
}

Document::Document() : Node(this, NodeType::Document) {}

Document::Document(Document&& other) noexcept : Document() {
  storage_ = std::move(other.storage_);
  mutable_children() = std::move(other.mutable_children());
  for (Node* child : mutable_children()) {
    child->set_parent(this);
  }
  for (auto& node : storage_) {
    update_owner(*node, this);
  }
}

Document& Document::operator=(Document&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  storage_ = std::move(other.storage_);
  mutable_children() = std::move(other.mutable_children());
  for (Node* child : mutable_children()) {
    child->set_parent(this);
  }
  for (auto& node : storage_) {
    update_owner(*node, this);
  }
  return *this;
}

Document::~Document() = default;

template <typename T, typename... Args>
T& Document::make_node(Args&&... args) {
  auto node = std::make_unique<T>(this, std::forward<Args>(args)...);
  T& reference = *node;
  storage_.push_back(std::move(node));
  return reference;
}

Element& Document::create_element(std::string qualified_name,
                                  std::string namespace_uri,
                                  std::string local_name) {
  return make_node<Element>(std::move(qualified_name), std::move(namespace_uri),
                            std::move(local_name));
}

Text& Document::create_text(std::string text) {
  return make_node<Text>(std::move(text));
}

Comment& Document::create_comment(std::string text) {
  return make_node<Comment>(std::move(text));
}

ProcessingInstruction& Document::create_processing_instruction(std::string target,
                                                               std::string data) {
  return make_node<ProcessingInstruction>(std::move(target), std::move(data));
}

CDataSection& Document::create_cdata_section(std::string text) {
  return make_node<CDataSection>(std::move(text));
}

Element* Document::document_element() noexcept {
  for (Node* child : children()) {
    if (child->type() == NodeType::Element) {
      return static_cast<Element*>(child);
    }
  }
  return nullptr;
}

const Element* Document::document_element() const noexcept {
  for (const Node* child : children()) {
    if (child->type() == NodeType::Element) {
      return static_cast<const Element*>(child);
    }
  }
  return nullptr;
}

std::vector<Node*> Document::depth_first() noexcept {
  std::vector<Node*> nodes;
  collect_depth_first(*this, nodes);
  return nodes;
}

std::vector<const Node*> Document::depth_first() const {
  std::vector<const Node*> nodes;
  collect_depth_first(*this, nodes);
  return nodes;
}

std::string serialize(const Document& document, const SerializeOptions&) {
  std::ostringstream output;
  serialize(document, output);
  return output.str();
}

void serialize(const Document& document, std::ostream& output, const SerializeOptions&) {
  if (!output.good()) {
    throw XmlParseException(ErrorKind::Io, SourceLocation{},
                            "XML output stream is not writable");
  }
  serialize_node(document, output);
  if (!output.good()) {
    throw XmlParseException(ErrorKind::Io, SourceLocation{},
                            "XML output stream write failed");
  }
}

}  // namespace xmlparser::v1
