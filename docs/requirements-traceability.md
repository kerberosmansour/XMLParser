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
| `REQ-DOM-01` DOM node tree and parser-built node identity | `tests/req/dom_model_tests.cpp` | `REQ_DOM_01_builds_document_element_attribute_text_comment_pi_cdata_nodes`; `REQ_DOM_01_preserves_node_type_identity_after_parse`; `REQ_DOM_01_enforces_configured_dom_node_limit` |
| `REQ-DOM-02` DOM read/write mutation and invariant enforcement | `tests/req/dom_mutation_tests.cpp` | `REQ_DOM_02_creates_inserts_modifies_and_removes_nodes`; `REQ_DOM_02_rejects_cycle_creating_insert`; `REQ_DOM_02_rolls_back_failed_mutation` |
| `REQ-DOM-03` DOM traversal | `tests/req/dom_traversal_tests.cpp` | `REQ_DOM_03_returns_parent_children_and_siblings`; `REQ_DOM_03_depth_first_iterator_visits_nodes_in_document_order` |
| `REQ-DOM-04` DOM serialization | `tests/req/dom_serialization_tests.cpp` | `REQ_DOM_04_serializes_to_string`; `REQ_DOM_04_serializes_to_stream`; `REQ_DOM_04_preserves_namespace_declarations`; `REQ_DOM_04_reports_stream_failure` |
| `REQ-DOM-05` qualified-name and namespace-aware attribute access | `tests/req/dom_attribute_access_tests.cpp` | `REQ_DOM_05_get_attribute_namespace_unaware`; `REQ_DOM_05_get_attribute_ns_distinguishes_same_local_name_different_uri`; `REQ_DOM_05_set_attribute_ns_keeps_expanded_names_unique` |
| `REQ-ERR-03` recoverable validity callback and non-recoverable well-formedness errors | `tests/req/error_recoverable_callback_tests.cpp` | `REQ_ERR_03_recoverable_callback_observes_validity_error`; `REQ_ERR_03_recoverable_callback_throw_stops_parse`; `REQ_ERR_03_non_recoverable_well_formedness_error_stops_parse` |
| `REQ-ERR-05` DOM resource and mutation failures leave trees consistent | `tests/req/dom_model_tests.cpp`; `tests/req/dom_mutation_tests.cpp` | `REQ_DOM_01_enforces_configured_dom_node_limit`; `REQ_DOM_02_rejects_cycle_creating_insert`; `REQ_DOM_02_rolls_back_failed_mutation` |
| `REQ-STD-02` XML 1.1 selectable | `tests/req/std_xml11_tests.cpp`; `tests/fixtures/w3c/xmlconf/manifest.xml` | `REQ_STD_02_selects_xml11_at_runtime`; `REQ_STD_02_distinguishes_xml10_and_xml11_character_rules`; `REQ_STD_02_runs_xml11_conformance_subset` |
| `REQ-ERR-01` well-formedness vs DTD validity distinction | `tests/req/error_validation_distinction_tests.cpp` | `REQ_ERR_01_reports_well_formedness_error_kind`; `REQ_ERR_01_reports_dtd_validity_error_kind`; `REQ_ERR_01_does_not_confuse_validity_with_parse_failure` |
| `REQ-ERR-04` internal DTD and external resolver contract | `tests/req/error_dtd_validation_tests.cpp` | `REQ_ERR_04_validates_internal_dtd_subset`; `REQ_ERR_04_external_dtd_resolver_is_not_called_by_default`; `REQ_ERR_04_external_dtd_resolver_is_called_only_when_configured` |
| `REQ-PLAT-01` compiler/platform CI matrix declared | `.github/workflows/ci.yml`; `tests/e2e/platform_release_evidence_tests.cpp` | `REQ_PLAT_01_ci_matrix_linux_gcc11`; `REQ_PLAT_01_ci_matrix_linux_clang14`; `REQ_PLAT_01_ci_matrix_macos_apple_clang14`; `REQ_PLAT_01_ci_matrix_windows_msvc_v142` |
| `REQ-PLAT-02` public headers compile as C++17 | `tests/e2e/platform_public_header_tests.cpp` | `REQ_PLAT_02_public_headers_compile_as_cxx17` |
| `REQ-PLAT-03` installed package consumer | `tests/e2e/platform_cmake_consumer_tests.cpp`; `tests/e2e_m1_cmake_consumer.cpp` | `REQ_PLAT_03_find_package_imported_target_builds_consumer`; existing M1 install-tree consumer |
| `REQ-PLAT-04` no mandatory runtime dependencies | `tests/e2e/platform_dependency_audit.cmake`; `tests/e2e/platform_release_evidence_tests.cpp` | `REQ_PLAT_04_default_link_has_no_non_std_runtime_dependency` |
| `REQ-PLAT-05` static and shared builds | `tests/e2e/platform_cmake_consumer_tests.cpp` | `REQ_PLAT_05_builds_static_library`; `REQ_PLAT_05_builds_shared_library` |
| `REQ-PLAT-06` standard public integer/size types | `tests/e2e/platform_public_header_tests.cpp` | `REQ_PLAT_06_public_api_uses_fixed_width_or_standard_size_types_intentionally` |
| `REQ-API-04` parser-core coverage gate | `tests/coverage/parser_core_coverage.cmake`; `tests/e2e/platform_release_evidence_tests.cpp` | `REQ_API_04_parser_core_line_coverage_is_at_least_90_percent` |

## Future Coverage Hooks

- `REQ-SAX-*` expands again in M5 only if conformance fixtures reveal extra
  namespace/SAX cases.
- `REQ-DOM-*` expands again in M5 only if release/conformance work reveals
  additional DOM round-trip cases.
- Full upstream W3C fixture import remains future work; M5 records provenance
  and a curated data-only manifest.
