# Verification Report - xmlparser Milestone 5

## What was exercised

| Scenario | Category | How exercised | Result | Evidence |
|---|---|---|---|---|
| XML 1.1 selection succeeds only in XML 1.1 mode | happy path / invalid input | `ctest --test-dir build --output-on-failure -L req` | pass | `REQ_STD_02_selects_xml11_at_runtime`, `REQ_STD_02_distinguishes_xml10_and_xml11_character_rules`, `REQ_STD_02_rejects_version_declaration_option_mismatch` |
| Internal DTD validation accepts declared content | happy path | `ctest --test-dir build --output-on-failure -L req` | pass | `REQ_ERR_04_validates_internal_dtd_subset` |
| Well-formedness errors stay distinct from validity errors | invalid input | `ctest --test-dir build --output-on-failure -L req` | pass | `REQ_ERR_01_reports_well_formedness_error_kind`, `REQ_ERR_01_reports_dtd_validity_error_kind`, `REQ_ERR_01_does_not_confuse_validity_with_parse_failure` |
| Entity recursion, oversize replacement, duplicate declarations, and undeclared nested entities are bounded | abuse / boundary | `ctest --test-dir build --output-on-failure -L req` | pass | `tests/req/error_dtd_validation_tests.cpp` |
| External DTD resolution is disabled by default and bounded when caller opt-in is configured | abuse / dependency failure | `ctest --test-dir build --output-on-failure -L req` | pass | `REQ_ERR_04_external_dtd_resolver_is_not_called_by_default`, `REQ_ERR_04_external_dtd_resolver_is_called_only_when_configured`, `REQ_ERR_04_external_dtd_resolver_is_bounded` |
| W3C fixture intake is data-only and provenance is recorded | supply-chain / conformance | `ctest --test-dir build --output-on-failure -L conformance` | pass | `tests/fixtures/w3c/xmlconf/manifest.xml`, `docs/conformance-fixtures.md` |
| Public headers, install package, static/shared builds, and dependency audit remain consumer-safe | compatibility | `ctest --test-dir build --output-on-failure -L e2e`; `cmake -DXMLPARSER_SOURCE_DIR="$PWD" -P tests/e2e/platform_dependency_audit.cmake` | pass | `tests/e2e/platform_public_header_tests.cpp`, `tests/e2e/platform_cmake_consumer_tests.cpp`, `tests/e2e/platform_release_evidence_tests.cpp` |
| Parser-core coverage meets release threshold | release gate | `cmake -DXMLPARSER_SOURCE_DIR="$PWD" -DXMLPARSER_BINARY_DIR="$PWD/build" -P tests/coverage/parser_core_coverage.cmake` | pass | parser-core line coverage `95.009416195856872%` |
| M1-M4 regression suite remains green | regression | `ctest --test-dir build --output-on-failure` | pass after fix | 4/4 CTest tests passed in 15.45s |

## Bugs found

| id | severity | scenario | regression test | status |
|---|---|---|---|---|
| M5-VFY-1 | low | M5 conformance-doc rewrite preserved the policy but dropped the exact lowercase phrase expected by the M1 BDD fixture-provenance check | Existing `M1 fixture provenance exists` scenario in `tests/bdd_m1_public_api.cpp` | fixed and verified by rerunning full CTest |

## Environment

- OS: Darwin 25.4.0 arm64.
- CMake: 4.0.3.
- Compiler: Apple clang 21.0.0.
- UI / browser: N/A - library/runtime milestone, no UI surface.
- AI tolerance: N/A - no AI component.

## Coverage gaps

- Remote GitHub-hosted Linux/macOS/Windows matrix execution is left to the PR ship gate; M5 locally verifies the workflow declaration and all local build, requirement, e2e, conformance, dependency, and coverage gates.
- Full upstream W3C XML Test Suite payload import remains a future milestone. M5 uses a curated data-only manifest and documents that it is not a complete W3C conformance claim.
