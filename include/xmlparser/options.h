#pragma once

#include <cstddef>
#include <functional>
#include <string>
#include <string_view>

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

using ExternalDtdResolver = std::function<std::string(std::string_view system_id)>;

struct ParserOptions {
  XmlVersion version = XmlVersion::Xml10;
  Encoding encoding = Encoding::Auto;
  ValidationMode validation = ValidationMode::None;
  NamespaceMode namespaces = NamespaceMode::Enabled;
  bool allow_external_dtd = false;
  ExternalDtdResolver external_dtd_resolver;

  std::size_t max_document_bytes = 16U * 1024U * 1024U;
  std::size_t max_depth = 256U;
  std::size_t max_token_bytes = 1024U * 1024U;
  std::size_t max_attributes_per_element = 1024U;
  std::size_t max_entity_expansions = 100000U;
  std::size_t max_dom_nodes = 1000000U;
  std::size_t max_dtd_declarations = 4096U;
  std::size_t max_entity_replacement_bytes = 1024U * 1024U;
  std::size_t max_external_subset_bytes = 1024U * 1024U;
};

struct SerializeOptions {
  Encoding encoding = Encoding::Utf8;
  bool pretty_print = false;
  std::string indentation = "  ";
};

}  // namespace xmlparser::v1
