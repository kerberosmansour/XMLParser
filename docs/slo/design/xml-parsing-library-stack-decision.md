# Stack Decision - xml-parsing-library

## Chosen Stack

- C++17 library.
- CMake >= 3.20 build system.
- Catch2 for tests only, disabled from runtime and installed-library dependencies.
- Standard library ownership types in public interfaces: `std::string`, `std::string_view`, `std::unique_ptr`, `std::optional`, and standard containers where needed.

## Reason

The requirements demand cross-platform C++17, installable CMake targets, and no mandatory runtime dependencies beyond the C++ standard library. External research shows the competitor landscape splits between full-featured validating parsers and lighter DOM or streaming parsers, so a single native C++ parser core avoids impedance between DOM and SAX APIs while keeping the public surface dependency-light.

## Rejected Alternatives

- C++20 or newer - rejected because REQ-PLAT-02 requires C++17.
- Wrapper around libxml2 or Expat - rejected because REQ-PLAT-04 forbids mandatory runtime dependencies beyond the standard library.
- Header-only implementation - rejected because parser internals are large and should remain ABI/build manageable while still exposing one aggregate public include.

## Non-Negotiables

- Public symbols live in `xmlparser::v1`.
- Public headers use no compiler-specific extensions.
- Runtime library depends only on the C++ standard library.
- XML well-formedness behavior is shared by DOM and SAX paths.
- External DTD fetching is optional and disabled by default.
- W3C XML Conformance Test Suite integration is required before core parser completion claims.
- CMake exports an installable `XmlParserConfig.cmake` and namespaced target.
