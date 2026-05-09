# Interface Lock - xml-parsing-library

> Downstream milestones must not rename or reshape these without explicit migration work.

## Public Headers

| Interface | Description | Stability |
|---|---|---|
| `#include <xmlparser/xmlparser.h>` | Aggregate include for the full public API. | stable |
| `include/xmlparser/*.h` | Split public headers included by the aggregate header. | evolving until first release |
| `namespace xmlparser::v1` | Versioned public namespace. | stable |

## Core Public Types

| Interface | Description | Stability |
|---|---|---|
| `XmlVersion` | Selects XML 1.0 or XML 1.1 behavior. | stable |
| `ParserOptions` | Encoding, XML version, validation mode, namespace mode, and resource limits. | evolving |
| `SerializeOptions` | Output encoding and formatting options. | evolving |
| `SourceLocation` | Line, column, and byte offset for diagnostics. | stable |
| `ErrorKind` | Separates well-formedness, validity, encoding, resource-limit, and I/O style errors. | stable |
| `XmlParseException` | Typed exception for well-formedness and encoding failures. | stable |
| `XmlValidityException` | Typed exception or diagnostic type for validity failures. | stable |
| `QualifiedName` | URI, local name, and qualified name for namespace-aware APIs. | stable |
| `AttributeView` | SAX attribute event view, including namespace data. | evolving |

## SAX Interfaces

| Interface | Description | Stability |
|---|---|---|
| `SaxHandler` | Virtual callback interface for document, element, character, PI, comment, and CDATA events. | stable |
| `SaxCallbacks` | Optional `std::function` adapter for selective event handling. | evolving |
| `SaxParser::parse(std::string_view)` | One-shot SAX parse. | stable |
| `SaxParser::feed(std::string_view)` | Incremental input. | stable |
| `SaxParser::finish()` | Completes incremental parse and detects truncated input. | stable |
| recoverable error callback | Lets callers observe non-fatal validity errors where continuing is permitted. | evolving |

## DOM Interfaces

| Interface | Description | Stability |
|---|---|---|
| `Document` | Root owner for DOM tree. | stable |
| `Node` | Base node view/handle for traversal. | evolving |
| `Element` | Element node operations, children, attributes, namespace declarations. | stable |
| `Attribute` | Attribute representation with namespace metadata. | stable |
| `Text`, `Comment`, `ProcessingInstruction`, `CDataSection` | DOM leaf node types. | stable |
| `Document::create_*` methods | Node creation APIs. | evolving |
| `Node` traversal APIs | parent, children, siblings, depth-first iteration. | stable |
| `Element::getAttribute` | Namespace-unaware attribute access. | stable |
| `Element::getAttributeNS` | Namespace-aware attribute access. | stable |
| `serialize(const Document&, SerializeOptions)` | String serialization. | stable |
| `serialize(const Document&, std::ostream&, SerializeOptions)` | Stream serialization. | stable |

## Build and Packaging Interfaces

| Interface | Description | Stability |
|---|---|---|
| `xmlparser::xmlparser` | Exported CMake target. | stable |
| `XmlParserConfig.cmake` | Generated package config for `find_package(XmlParser CONFIG REQUIRED)`. | stable |
| `BUILD_SHARED_LIBS` | Controls static/shared build mode. | stable |
| `XMLPARSER_BUILD_TESTS` | Enables tests. | stable |
| `XMLPARSER_ENABLE_EXTERNAL_DTD` | Enables optional external DTD resolver integration. | evolving |

## Requirement Traceability

Every test file introduced by the runbook must include requirement IDs in names, sections, or assertions so coverage can be traced back to `REQ-STD-*`, `REQ-DOM-*`, `REQ-SAX-*`, `REQ-ERR-*`, `REQ-PLAT-*`, and `REQ-API-*`.
