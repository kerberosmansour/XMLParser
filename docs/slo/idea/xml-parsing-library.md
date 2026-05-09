---
name: xml-parsing-library
created: 2026-05-09
status: requirements-intake
tla_required: false
---

# C++ XML Parsing Library

## Source

This intake document is generated from the user-supplied Requirements Document v1.0 for a C++ XML parsing library. The requirements are already specific enough that `/slo-ideate` was skipped; this file preserves the requirements context needed by downstream SLO architecture and planning steps.

## The pain

C++ teams need a standards-compliant XML parsing library that exposes both in-memory DOM workflows and streaming SAX workflows without mandatory runtime dependencies. The concrete pain is integration risk: consumers need predictable namespace handling, encoding detection, validation behavior, error locations, and installable CMake targets across Linux, macOS, and Windows.

## Required capabilities

- XML 1.0 5th Edition and XML 1.1 parsing, with parser version selected at runtime or compile time.
- Namespace-aware parsing for XML Namespaces 1.0 and 1.1.
- Encoding detection for UTF-8 and UTF-16 LE/BE, including BOM handling.
- DOM tree construction, mutation, traversal, serialization, and namespace-aware attribute access.
- SAX/event-driven streaming parsing with incremental chunked input and namespace information on events.
- Distinct well-formedness and validity errors with typed exceptions and source locations.
- Internal DTD validation, optional external DTD validation disabled by default.
- C++17 public API in versioned namespace `xmlparser::v1`.
- CMake >= 3.20 build with installable package config.
- Cross-platform compilation on supported GCC, Clang, Apple Clang, and MSVC versions.
- Test suite covering all requirement IDs and at least 90 percent parser-core line coverage.

## Top risks

- **Breach**: Callers may parse XML containing secrets or personal data; the library must never log raw XML payloads, entity replacement text, or internal file paths by default.
- **Compliance fine**: Consumers may use the library in regulated products; incorrect entity handling or malformed-input crashes could undermine OWASP ASVS-style input validation obligations in downstream systems.
- **Prolonged outage**: A malicious or accidental oversized XML document, excessive nesting, or entity expansion could exhaust CPU or memory in a service using the parser.

## Recommendation

Build a dependency-light C++17 library with a small internal parser core shared by SAX and DOM layers. Implement XML 1.0 well-formed parsing and error reporting first, then layer namespaces, incremental SAX, DOM mutation/serialization, and DTD/XML 1.1 conformance in planned increments.

## Open questions for implementation

1. Which published XML conformance suites will be used as fixtures, and how will licensing be tracked?
2. Should XML 1.1 be enabled by default or require explicit `ParserOptions::version = XmlVersion::Xml11`?
3. What exact resource limit defaults should ship in `ParserOptions` for depth, token length, entity expansion, attributes per element, and document bytes?
4. Should external DTD fetching remain unimplemented behind an extension interface in v1, or ship as an opt-in feature with caller-provided resolver only?
