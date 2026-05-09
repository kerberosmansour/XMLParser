#pragma once

#include <cstddef>
#include <string>

#include <xmlparser/version.h>

namespace xmlparser::v1 {

enum class Encoding {
  Auto,
  Utf8,
  Utf16Le,
  Utf16Be,
};

enum class ValidationMode {
  None,
  Dtd,
};

enum class NamespaceMode {
  Enabled,
  Disabled,
};

struct ParserOptions {
  XmlVersion version = XmlVersion::Xml10;
  Encoding encoding = Encoding::Auto;
  ValidationMode validation = ValidationMode::None;
  NamespaceMode namespaces = NamespaceMode::Enabled;
  bool allow_external_dtd = false;

  std::size_t max_document_bytes = 16U * 1024U * 1024U;
  std::size_t max_depth = 256U;
  std::size_t max_token_bytes = 1024U * 1024U;
  std::size_t max_attributes_per_element = 1024U;
  std::size_t max_entity_expansions = 100000U;
  std::size_t max_dom_nodes = 1000000U;
};

struct SerializeOptions {
  Encoding encoding = Encoding::Utf8;
  bool pretty_print = false;
  std::string indentation = "  ";
};

}  // namespace xmlparser::v1
