---
slug: xml-parsing-library
tla_required: false
tla_justification: "Single-process parser state machine; correctness is better covered by conformance, property, fuzz, and sanitizer tests than TLA+."
security_libs_required: false
ai_component: false
compliance: [soc2, asvs]
created: 2026-05-09
---

# Design Overview - xml-parsing-library

## Goal

Create a robust, standards-oriented C++17 XML parsing library with DOM and SAX APIs, namespace support, typed error handling, optional validation, and CMake packaging.

## Value Check

**Question**: What am I getting out of this project?

**Answer**: The user gets an embeddable C++17 XML parser that can be trusted in production code: one dependency-light library with standards-oriented XML parsing, DOM and SAX APIs, namespace handling, typed diagnostics, defined malformed-input behavior, and installable CMake packaging. The value is not "yet another XML parser"; it is a predictable parser contract for teams that need conformance evidence, safe defaults, and both tree and streaming APIs without pulling a mandatory runtime dependency into their product.

## Requirements Fit

| Requirement Goal | Design Mechanism | Evidence Required |
|---|---|---|
| XML 1.0 and XML 1.1 standards support | Version-aware tokenizer, character rules, declaration handling, and conformance harness | W3C fixture matrix and version-specific tests |
| Namespace correctness | Dedicated namespace stack and `QualifiedName` event/model types | scope, default namespace, undeclaration, and duplicate attribute tests |
| Encoding correctness | `InputSource + EncodingDetector` with UTF-8/UTF-16 LE/BE BOM handling | malformed/truncated encoding tests with byte offsets |
| DOM API | `Document`-owned node model with checked mutation and traversal APIs | DOM creation, insert, remove, traversal, and exception-safety tests |
| SAX API | Incremental `SaxParser::feed` / `finish` with handler callbacks | chunk-boundary, callback exception, and namespace event tests |
| Error handling and validation | typed error kinds, source locations, recoverable validity callback, DTD validation layer | well-formedness vs validity tests and no-UB malformed input tests |
| Cross-platform packaging | CMake target export, `XmlParserConfig.cmake`, static/shared support | install-tree consumer test and compiler matrix |
| API constraints | `xmlparser::v1`, aggregate include, standard library public types | public-header compile tests |
| Test coverage | requirement traceability matrix plus coverage target | coverage report at 90 percent parser-core line coverage |

## Design Goal Check

The design meets the requirements goals if each milestone proves a user-visible capability rather than only adding internals: M1 proves the library can be consumed, M2 proves safe XML tokenization and diagnostics, M3 proves streaming and namespaces, M4 proves DOM workflows, and M5 proves validation, XML 1.1, coverage, and release packaging. If a milestone cannot point to a requirement ID and a user-visible reason to exist, it should not be executed in this runbook.

## Stack

- Language: C++17.
- Build: CMake >= 3.20.
- Runtime dependencies: C++ standard library only.
- Test dependency: Catch2, enabled only for test builds and pinned before implementation.
- Tooling: `clang-format`, `clang-tidy`, compiler warnings as errors, sanitizers where supported, coverage tooling per platform.

## Scope Boundaries

In scope:

- XML 1.0 5th Edition and XML 1.1 parsing.
- XML Namespaces 1.0 and 1.1.
- UTF-8 and UTF-16 LE/BE input with BOM handling.
- DOM API, SAX API, typed errors, DTD validation, and serialization.
- CMake install package and cross-platform CI.

Out of scope:

- XSLT.
- XPath/XQuery.
- Binary XML formats.
- Mandatory network fetching of external DTDs.

## Planning State

The architecture artifacts are generated from requirements intake plus external research in [docs/slo/research/xml-parsing-library](../research/xml-parsing-library). `/slo-plan` should continue milestone-by-milestone from Milestone 1 after user confirmation.
