# Requirements Traceability - XMLParser

This matrix starts the executable coverage trail for the XMLParser runbook.
Requirement identifiers are intentionally stable labels for tests, docs, and
future conformance fixtures.

| Requirement | Coverage In M1 | Evidence |
|---|---|---|
| `REQ-API-01` aggregate public include compiles as C++17 | `tests/bdd_m1_public_api.cpp`, `tests/e2e_m1_cmake_consumer.cpp` | `M1 API include compiles`; `M1 public header compiles as CXX17` |
| `REQ-API-02` parser options expose bounded defaults | `tests/bdd_m1_public_api.cpp` | `M1 options are bounded` |
| `REQ-ERR-01` parser errors are typed and public parser entry point remains stable | `tests/bdd_m1_public_api.cpp`, `tests/req/std_xml10_conformance_tests.cpp`, `tests/e2e_m1_cmake_consumer.cpp` | `M1 parser API remains visible after M2`; `REQ_STD_01_rejects_xml10_well_formedness_violations`; install-tree consumer |
| `REQ-ERR-02` empty input is a typed error path | `tests/bdd_m1_public_api.cpp` | `M1 empty input is defined` |
| `REQ-ERR-03` errors omit raw XML payload by default | `tests/bdd_m1_public_api.cpp` | `M1 raw XML is not logged` |
| `REQ-PLAT-01` public header is portable C++17 | `tests/e2e_m1_cmake_consumer.cpp` | `M1 public header compiles as CXX17` |
| `REQ-PLAT-03` installed package can be consumed with CMake | `tests/e2e_m1_cmake_consumer.cpp` | `M1 install tree consumer can find package` |
| `REQ-SEC-01` resource defaults are finite and positive | `tests/bdd_m1_public_api.cpp` | `M1 options are bounded` |
| `REQ-SEC-02` secret-looking XML is not echoed in diagnostics | `tests/bdd_m1_public_api.cpp` | `M1 raw XML is not logged` |
| `REQ-STD-01` W3C fixture provenance is documented before import | `tests/bdd_m1_public_api.cpp`, `docs/conformance-fixtures.md` | `M1 fixture provenance exists` |
| `REQ-STD-01` XML 1.0 well-formed documents parse and malformed documents fail | `tests/req/std_xml10_conformance_tests.cpp` | `REQ_STD_01_accepts_well_formed_xml10_documents`; `REQ_STD_01_rejects_xml10_well_formedness_violations` |
| `REQ-STD-04` UTF-8/UTF-16 BOM and malformed encoding handling | `tests/req/std_encoding_tests.cpp` | `REQ_STD_04_detects_utf8_bom`; `REQ_STD_04_detects_utf16le_bom`; `REQ_STD_04_detects_utf16be_bom`; `REQ_STD_04_rejects_truncated_multibyte_sequence_with_byte_offset` |
| `REQ-STD-05` XML declarations, processing instructions, comments, CDATA, and entity references | `tests/req/std_markup_constructs_tests.cpp` | `REQ_STD_05_parses_xml_declaration`; `REQ_STD_05_emits_processing_instruction`; `REQ_STD_05_allows_xml_stylesheet_processing_instruction`; `REQ_STD_05_preserves_cdata_boundaries_in_events`; `REQ_STD_05_parses_comments`; `REQ_STD_05_resolves_predefined_and_character_entities` |
| `REQ-ERR-02` parse exceptions include source locations | `tests/req/error_location_exception_tests.cpp` | `REQ_ERR_02_exception_contains_message_line_column_and_byte_offset`; `REQ_ERR_02_location_tracks_utf16_input_offsets` |
| `REQ-ERR-05` parser-core resource limits fail safely | `tests/req/error_exception_safety_tests.cpp` | `REQ_ERR_05_depth_limit_returns_resource_error`; `REQ_ERR_05_token_limit_returns_resource_error`; `REQ_ERR_05_entity_expansion_limit_returns_resource_error` |
| `REQ-ERR-06` empty, truncated, malformed, and secret-looking input has defined failure behavior | `tests/req/error_malformed_input_tests.cpp`; `tests/req/std_encoding_tests.cpp` | `REQ_ERR_06_empty_input_returns_defined_error`; `REQ_ERR_06_truncated_input_returns_defined_error`; `REQ_ERR_06_secret_payload_is_not_echoed`; `REQ_STD_04_rejects_truncated_multibyte_sequence_with_byte_offset` |
| `REQ-STD-03` namespace scope and duplicate expanded-name behavior | `tests/req/std_namespaces_tests.cpp` | `REQ_STD_03_resolves_default_namespace_scope`; `REQ_STD_03_resolves_prefixed_names_across_nested_scopes`; `REQ_STD_03_rejects_duplicate_expanded_attribute_names`; `REQ_STD_03_handles_namespace_11_undeclaration` |
| `REQ-SAX-01` SAX parser parses without DOM and enforces bounded incremental input | `tests/req/sax_streaming_tests.cpp` | `REQ_SAX_01_parses_without_dom_allocation`; `REQ_SAX_01_streams_large_document_with_bounded_memory` |
| `REQ-SAX-02` SAX event coverage | `tests/req/sax_event_coverage_tests.cpp` | `REQ_SAX_02_emits_document_start_end`; `REQ_SAX_02_emits_element_start_end`; `REQ_SAX_02_emits_character_pi_comment_and_cdata_events` |
| `REQ-SAX-03` virtual handlers and std::function callback adapter | `tests/req/sax_callback_registration_tests.cpp` | `REQ_SAX_03_registers_virtual_handler`; `REQ_SAX_03_registers_std_function_callbacks`; `REQ_SAX_03_allows_selective_callbacks` |
| `REQ-SAX-04` incremental chunking | `tests/req/sax_incremental_tests.cpp` | `REQ_SAX_04_accepts_one_byte_chunks`; `REQ_SAX_04_accepts_chunks_split_inside_markup`; `REQ_SAX_04_accepts_chunks_split_inside_multibyte_sequence`; `REQ_SAX_04_finish_rejects_truncated_document` |
| `REQ-SAX-05` namespace-aware SAX event data | `tests/req/sax_namespace_event_tests.cpp` | `REQ_SAX_05_element_events_include_uri_local_and_qname`; `REQ_SAX_05_attribute_events_include_uri_local_and_qname` |
| `REQ-ERR-03` non-recoverable well-formedness errors stop parse before validity recovery exists | `tests/req/error_recoverable_callback_tests.cpp` | `REQ_ERR_03_non_recoverable_well_formedness_error_stops_parse` |

## Future Coverage Hooks

- `REQ-STD-*` expands again in M5 when XML 1.1 and conformance fixture
  ingestion are implemented.
- `REQ-SAX-*` expands again in M5 only if conformance fixtures reveal extra
  namespace/SAX cases.
- `REQ-DOM-*` expands in M4 when DOM mutation, traversal, and serialization
  are implemented.
- `REQ-VALID-*` expands in M5 when DTD validation is implemented.
