---
name: xml-parsing-library
researched: 2026-05-09
incomplete: false
---

# Research Dossier - C++ XML Parsing Library

## Research Brief

The wedge is a standards-oriented C++17 XML parser that exposes DOM and SAX APIs without mandatory runtime dependencies. The target user is a C++ application or platform team that needs XML parsing, namespace handling, validation diagnostics, and CMake packaging without adopting a heavy external runtime dependency.

Research questions:

1. Which XML and namespace specifications define the conformance target?
2. Which conformance fixtures should drive implementation?
3. What do established XML parser libraries prove about feature expectations and gaps?
4. Which security failures are common enough to shape parser defaults?
5. Which build and test tooling choices fit the C++17 and CMake requirements?

## Market

The direct market is mostly open-source parser libraries with zero license price, so the practical buyer cost is engineering integration, security maintenance, conformance confidence, and build portability rather than per-seat pricing. Established choices split by use case: validating DOM/SAX capability in Xerces-C++, stream-oriented parsing in Expat, broad C toolkit coverage in libxml2, lightweight DOM convenience in pugixml and TinyXML-2.

## Direct Competitors

| Name | Price | Key Feature | Gap vs Our Wedge |
|---|---|---|---|
| Xerces-C++ | Free, Apache License 2.0 | Validating C++ XML parser with DOM, SAX, SAX2, generation, manipulation, validation, XML 1.0/1.1, namespace, and schema-related features. | Larger surface and project page warns it currently lacks active maintainers; our wedge should be smaller, C++17-native, and easier to embed. |
| Expat | Free, MIT/X Consortium style license | Stream-oriented C parser for XML 1.0, built around registered handlers and suitable for large files. | Not a C++ DOM/SAX library; our wedge adds C++17 API, DOM model, namespace-rich events, and CMake package polish. |
| libxml2 | Free, MIT License | C XML toolkit originally from GNOME with official releases and documentation. | C API/toolkit with external dependency footprint; our wedge is C++17-only with no mandatory runtime dependency. |
| pugixml | Free, MIT License | Lightweight C++ XML library with DOM-like traversal/modification, fast non-validating parsing, XPath, Unicode support, and package availability. | Non-validating parser and not fully W3C conformant per its manual; our wedge prioritizes standards conformance, validation separation, and SAX streaming. |
| TinyXML-2 | Free, zlib license | Small, efficient C++ DOM parser in one `.cpp` and one `.h`, easy to integrate and save DOM output. | Does not parse or use DTDs and assumes UTF-8; our wedge requires DTD validation, UTF-16 LE/BE, XML 1.1, namespaces, and SAX. |

## Adjacent Tools

| Name | Why Adjacent, Not Direct | Can They Pivot Into Us? |
|---|---|---|
| W3C Markup Validator | Validates web/XML documents, not an embeddable C++ library. | No, but its validation behavior reinforces the need for conformance fixtures and DTD clarity. |
| XML Schema validators | Validate XSD, which is out of scope for this v1 requirements set. | They could consume this parser later, but XPath/XQuery/XSD remain outside this runbook. |
| Package managers such as vcpkg, Conan, Homebrew, MacPorts, NuGet | Distribution channels rather than parser competitors. | They shape packaging expectations; installable CMake config and stable target names matter. |

## Technical Prior Art

- W3C XML 1.0 Fifth Edition is the primary XML 1.0 target; W3C notes the fifth edition incorporates errata and includes a maintained test suite for conformance assessment.
- W3C XML 1.1 Second Edition is a separate selectable parser mode because XML 1.1 was introduced largely due to Unicode character-model evolution beyond XML 1.0's original assumptions.
- Namespaces in XML 1.0 and 1.1 require namespace-well-formed processors to report namespace well-formedness violations; Namespaces 1.1 also adds prefix undeclaration and IRI namespace names.
- The W3C XML Conformance Test Suite covers XML and XML namespace recommendations, includes over 2000 test files, and explicitly encourages implementors to write a harness around the tests.
- Xerces-C++ demonstrates the full-featured end of the prior art: validating parser, DOM/SAX/SAX2, namespace support, encodings, and schema-related facilities.
- Expat demonstrates the streaming-prior-art end: registered handlers, no full tree requirement, and explicit XML security guidance around XXE and entity-expansion denial of service.
- CMake config-file package documentation shows that installable package configs should export imported targets and usage requirements for downstream `find_package`.
- Catch2 supports unit and BDD-style tests for C++14 and later, which fits the C++17 requirement and the runbook's BDD-first execution model.

## Regulatory / Legal

- No direct domain regulation applies to a general XML parser library by itself, because it does not define a business process or store personal data. Downstream applications may parse regulated data, so the library must not log raw XML or leak payload snippets through diagnostics.
- License risk is real: test fixtures, competitor code, and examples must not be copied unless their licenses permit it. The implementation must be original; third-party conformance fixtures need source and license tracking.
- XML-specific security obligations matter even without direct regulation. Expat's security documentation identifies XXE and entity-expansion attacks as parser-relevant issues and notes that external URL access should not be configured for untrusted XML.

## Design Implications

- XML 1.0 Fifth Edition should be the default parser mode, with XML 1.1 selected explicitly through `ParserOptions` or compile-time configuration.
- Namespace resolution must be implemented as a first-class layer, not as a DOM-only convenience, because SAX events require URI, local name, and qualified name.
- The test harness must be a milestone-one artifact, not a release cleanup task, because conformance breadth is too large to validate manually.
- External DTD fetching must remain disabled by default and mediated through a caller-owned resolver if implemented.
- Entity expansion, recursion/depth, attribute count, token length, document byte size, and DOM node count require explicit bounds before malformed-input support can be considered safe.

## Open Questions That Research Did Not Fully Answer

- Exact license terms and redistribution requirements for the selected W3C XML Test Suite release must be checked before fixtures are committed.
- The default resource-limit values should be calibrated from implementation benchmarks and real sample documents.
- The requirements ask for validity errors and internal DTD validation, but do not require XML Schema validation; this boundary should remain explicit in public docs.
