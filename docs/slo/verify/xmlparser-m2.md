# Verification Report - xmlparser Milestone 2

## What Was Exercised

| Scenario | Category | How exercised | Result | Evidence |
|---|---|---|---|---|
| M2 accepts simple XML 1.0 document | happy path | `REQ_STD_01_accepts_well_formed_xml10_documents` parsed nested elements, an attribute, and text through `SaxHandler`. | pass | `ctest --test-dir build --output-on-failure -L req` passed. |
| M2 parses XML declaration | happy path | `REQ_STD_05_parses_xml_declaration` parsed `<?xml version="1.0" encoding="UTF-8"?>`. | pass | Requirement tests passed. |
| M2 detects UTF-8 BOM | compatibility | `REQ_STD_04_detects_utf8_bom` parsed BOM-prefixed UTF-8 input. | pass | Requirement tests passed. |
| M2 detects UTF-16LE and UTF-16BE BOM | compatibility | `REQ_STD_04_detects_utf16le_bom` and `REQ_STD_04_detects_utf16be_bom` parsed byte-order-marked UTF-16 input. | pass | Requirement tests passed. |
| M2 rejects malformed UTF-8 | invalid input / abuse | `REQ_STD_04_rejects_truncated_multibyte_sequence_with_byte_offset` parsed a truncated UTF-8 sequence. | pass | Threw `ErrorKind::Encoding` at byte offset 6. |
| M2 handles comments, CDATA, and PIs | happy path | `REQ_STD_05_emits_processing_instruction`, `REQ_STD_05_allows_xml_stylesheet_processing_instruction`, `REQ_STD_05_parses_comments`, and `REQ_STD_05_preserves_cdata_boundaries_in_events` exercised markup callbacks. | pass | Requirement tests passed. |
| M2 resolves predefined and character entities | happy path | `REQ_STD_05_resolves_predefined_and_character_entities` decoded text and attribute values. | pass | Handler received decoded values. |
| M2 rejects mismatched tags | invalid input | `REQ_STD_01_rejects_xml10_well_formedness_violations` and `REQ_ERR_02_exception_contains_message_line_column_and_byte_offset` parsed mismatched tags. | pass | Threw `ErrorKind::WellFormedness` with source locations. |
| M2 rejects truncated input | empty/degraded state | `REQ_ERR_06_truncated_input_returns_defined_error` parsed unfinished CDATA. | pass | Threw typed well-formedness error. |
| M2 enforces resource limits | abuse | Depth, token, and entity-expansion limit tests set tiny parser limits. | pass | Each threw `ErrorKind::ResourceLimit`. |
| M2 diagnostics omit raw XML | abuse | M1 and M2 payload-safe diagnostics tests parsed secret-looking malformed XML. | pass | Error messages omitted full XML and secret substrings. |
| M2 keeps fixture provenance gate closed | dependency/provenance abuse | M1 provenance doc test remained green. | pass | W3C fixtures are still documented only, not vendored. |
| Install-tree consumer still works | E2E | `ctest --test-dir build --output-on-failure -L e2e` installed XMLParser and ran a downstream CMake consumer. | pass | E2E label passed. |

## Bugs Found

| id | severity | scenario | regression test | status |
|---|---|---|---|---|
| M2-V-1 | low | Prolog `<?xml-stylesheet ...?>` was initially misclassified as an XML declaration. | `REQ_STD_05_allows_xml_stylesheet_processing_instruction` | fixed and verified |

## Security Pass

| Check | Scope | Result | Evidence |
|---|---|---|---|
| Supply-chain manifest detection | Rust, Node, Python, Go manifests | N/A | `rg --files` found no `Cargo.toml`, `package.json`, `pyproject.toml`, `requirements.txt`, or `go.mod`. |
| Runtime dependency review | CMake target link graph | pass | `xmlparser` links only `src/xmlparser.cpp` and `src/parser_core.cpp`; Catch2 remains test-only and pinned to `v3.5.4`. |
| DAST smoke-service gate | Runtime service surface | N/A | No service, OpenAPI spec, or `docker-compose.yml` exists; this is a compiled library milestone. |
| AI tolerance | AI/LLM behavior | N/A | Contract block says no AI component. |

## Environment

- OS: Darwin 25.4.0 arm64.
- Compiler: Apple clang 21.0.0.
- CMake: 4.0.3.
- Browser/UI: N/A, no UI surface in M2.

## Coverage Gaps

- XML conformance fixtures are still not vendored or executed; M5 owns the W3C harness import after license/hash review.
- Namespace resolution, duplicate expanded attribute names, incremental chunking, final DOM semantics, serialization, XML 1.1, DTD validity, and external resolver behavior remain out of scope for M2.
- Real formatter, linter, sanitizer, and fuzz targets remain deferred.
