# XML Parsing Library - XMLParser (SLO Runbook v4)

> **Purpose**: Build a C++17 XML parsing library that satisfies the supplied requirements for standards compliance, DOM, SAX, error handling, validation, portability, and public API design.
> **Audience**: AI coding agents first, humans second.
> **Prerequisite reading**: [ARCHITECTURE.md](../ARCHITECTURE.md), [SECURITY.md](../SECURITY.md), [docs/slo/idea/xml-parsing-library.md](slo/idea/xml-parsing-library.md), [docs/slo/research/xml-parsing-library/synthesis.md](slo/research/xml-parsing-library/synthesis.md), [docs/slo/design/xml-parsing-library-interfaces.md](slo/design/xml-parsing-library-interfaces.md).

---

## 1. Runbook Metadata

| Field | Value |
|---|---|
| Runbook ID | `xml-parsing-library` |
| Project name | `XMLParser` |
| Primary stack | C++17 + CMake >= 3.20 |
| Primary package/app names | `xmlparser` |
| Prefix for tests and lesson files | `xmlparser` |
| Default unit test command | `cmake --build build --target xmlparser_tests && ctest --test-dir build --output-on-failure` |
| Default integration/BDD test command | `ctest --test-dir build --output-on-failure -L bdd` |
| Default E2E/runtime validation command | `ctest --test-dir build --output-on-failure -L e2e` |
| Default build/boot command | `cmake -S . -B build -DXMLPARSER_BUILD_TESTS=ON && cmake --build build` |
| Default formatter command | `cmake --build build --target format` once target exists |
| Default static analysis / lint command | `cmake --build build --target lint` once target exists |
| Default dependency / security audit command | `cmake --build build --target audit` once target exists |
| Default debugger or state-inspection tool | `lldb`, `gdb`, or Visual Studio debugger for failing tests |
| Allowed new dependencies by default | none at runtime; Catch2 allowed for tests after M1 pins acquisition |
| Schema/config migration allowed by default | no |
| Public interfaces stable by default | yes |

### Public Interfaces That Must Remain Stable Unless Explicitly Listed Otherwise

- `#include <xmlparser/xmlparser.h>`
- `namespace xmlparser::v1`
- CMake target `xmlparser::xmlparser`
- CMake package config `XmlParserConfig.cmake`
- `ParserOptions`, `XmlVersion`, `SourceLocation`, `XmlParseException`
- SAX handler and incremental parser method names after M3 lands
- DOM `Document`, `Node`, `Element`, `Attribute`, and serializer method names after M4 lands

---

## 2. Milestone Tracker

| # | Milestone | Status | Started | Completed | Lessons File | Completion Summary |
|---|---|---|---|---|---|---|
| 1 | Project Skeleton, Public API Frame, And Conformance Harness | `done` | 2026-05-09 | 2026-05-09 | `docs/slo/lessons/xmlparser-m1.md` | `docs/slo/completion/xmlparser-m1.md` |
| 2 | Encoding, Tokenizer, And XML 1.0 Well-Formed Parsing | `not_started` | | | `docs/slo/lessons/xmlparser-m2.md` | `docs/slo/completion/xmlparser-m2.md` |
| 3 | Namespaces And Incremental SAX API | `not_started` | | | `docs/slo/lessons/xmlparser-m3.md` | `docs/slo/completion/xmlparser-m3.md` |
| 4 | DOM Model, Mutation, Traversal, And Serialization | `not_started` | | | `docs/slo/lessons/xmlparser-m4.md` | `docs/slo/completion/xmlparser-m4.md` |
| 5 | DTD Validation, XML 1.1, Coverage, And Release Packaging | `not_started` | | | `docs/slo/lessons/xmlparser-m5.md` | `docs/slo/completion/xmlparser-m5.md` |

Status values: `not_started | in_progress | blocked | done`.

---

## 3. Architecture Diagram

See [ARCHITECTURE.md](../ARCHITECTURE.md). The end-state flow is:

```text
Caller -> Public API -> EncodingDetector -> Tokenizer -> Entity/DTD layer
       -> Namespace/Event layer -> SAX callbacks
                              \-> DOM builder -> DOM APIs -> Serializer
```

### Component Summary

| Component | Responsibility | Milestone |
|---|---|---|
| Build/package/test harness | CMake target, install config, tests, conformance fixture integration | M1 |
| Input and tokenizer core | Encodings, XML declaration, comments, CDATA, PIs, text, entity references, source locations | M2 |
| Namespace and SAX layer | URI/local/qname resolution and incremental event streaming | M3 |
| DOM and serializer layer | Tree ownership, mutation, traversal, attribute APIs, XML output | M4 |
| Validation/release hardening | Internal DTD validation, XML 1.1, optional external resolver contract, coverage and cross-platform evidence | M5 |

---

## 4. Global Execution Rules

- Work milestones sequentially.
- Write BDD tests before production code.
- Do not add runtime dependencies beyond the C++ standard library.
- Keep public API inside `xmlparser::v1`.
- Do not copy code from competitor projects.
- Do not enable external DTD fetching by default.
- Record every command result in the current milestone Evidence Log.
- Run formatter, build/typecheck, static analysis, full tests, E2E/runtime tests, artifact cleanup, and `.gitignore` review before marking any milestone done.
- Treat malformed XML as untrusted input: typed error or recoverable callback only, never undefined behavior.

---

## 5. High-Level State Modeling / Formal Verification

`tla_required: false`.

The parser is a single-process deterministic state machine rather than a distributed or concurrent protocol. Model parser correctness with conformance tests, state-machine unit tests, fuzz/sanitizer targets where supported, and invariants for depth, namespace stack balance, entity expansion limits, byte offset monotonicity, and DOM ownership.

---

## 6. Background Context

### Current State

The repository currently contains a minimal [README.md](../README.md), [LICENSE](../LICENSE), `.gitignore`, and generated SLO planning artifacts. No implementation, build system, public headers, or tests exist yet.

### Problem

1. **No build/package contract**: There is no CMake target, install config, or consumer integration test for REQ-PLAT-03.
2. **No parser core**: XML declarations, encodings, tokens, entity references, CDATA, comments, PIs, and error locations are not implemented.
3. **No SAX or DOM API**: Public APIs required by REQ-SAX-* and REQ-DOM-* do not exist.
4. **No validation/error separation**: Well-formedness and validity errors have no typed model.
5. **No conformance evidence**: Requirement coverage and W3C conformance fixtures are not wired into tests.

### Global Red Lines

- No runtime dependency beyond the C++ standard library.
- No public headers with compiler-specific extensions.
- No unbounded parser-controlled stack, token, entity, attribute, or DOM node growth.
- No raw XML payload logging in default diagnostics.
- No external DTD network or filesystem access unless explicitly configured by caller.
- No XPath, XQuery, XSLT, XML Schema validation, or binary XML work in this runbook.

---

## 7. Milestone Plan

### Milestone 1 - Project Skeleton, Public API Frame, And Conformance Harness

**Goal**: Establish the build, packaging, public API skeleton, test framework, requirement traceability, and external conformance harness needed before parser implementation.

**Context**: This is a greenfield repository with no CMake or source layout. Research identified W3C conformance fixtures, CMake package exports, and Catch2 BDD tests as design-critical infrastructure. This milestone creates the scaffolding and failing behavior-first tests that later parser milestones must satisfy.

**Carmack-style reliability goal**: Make compatibility, static analysis, source provenance, and requirement traceability executable from the start.

**Important design rule**: M1 may define API shapes and build/test infrastructure, but it must not fake XML parsing behavior beyond explicit unsupported-operation errors.

**Refactor budget**: `No refactor permitted beyond direct implementation`.

#### Contract Block

| Field | Value |
|---|---|
| Inputs | Requirements document, SLO design docs, W3C/source URLs from research |
| Outputs | CMake project, installable library target, aggregate public header, typed placeholder API, test harness, conformance fixture manifest |
| Interfaces touched | `#include <xmlparser/xmlparser.h>`, `xmlparser::v1`, `xmlparser::xmlparser`, `XMLPARSER_BUILD_TESTS`, `BUILD_SHARED_LIBS` |
| Files allowed to change | `CMakeLists.txt`, `cmake/XmlParserConfig.cmake.in`, `include/xmlparser/*.h`, `src/*.cpp`, `tests/**`, `docs/slo/**`, `README.md`, `.gitignore` |
| Files to read before changing anything | `ARCHITECTURE.md`, `SECURITY.md`, `docs/slo/research/xml-parsing-library/synthesis.md`, `docs/slo/design/xml-parsing-library-interfaces.md`, `.gitignore`, `README.md` |
| New files allowed | `CMakeLists.txt`, `cmake/**`, `include/xmlparser/**`, `src/**`, `tests/**`, `docs/requirements-traceability.md`, `docs/conformance-fixtures.md`, lessons/completion files |
| New dependencies allowed | Catch2 for tests only, pinned and documented; no runtime dependency |
| Migration allowed | no |
| Compatibility commitments | `README.md` still renders; MIT license preserved; installed CMake consumer can compile a minimal include/link program |
| Resource bounds introduced/changed | `ParserOptions` declares defaults for max document bytes, max depth, max token bytes, max attributes per element, max entity expansions, max DOM nodes; implementation may return unsupported until M2+ |
| Invariants/assertions required | public API compiles on C++17; aggregate include includes all public API; options defaults are positive and finite; version namespace exists |
| Debugger / inspection expectation | A failing Catch2 test can be launched under `lldb`, `gdb`, or Visual Studio debugger; document command in README or developer docs |
| Static analysis gates | CMake configure/build, `ctest`, formatter target placeholder or documented absence, lint target placeholder or documented absence |
| Exemplar code to copy | N/A - no brownfield exemplar; establish style from this milestone |
| Anti-exemplar code not to copy | Do not copy implementation code from Xerces-C++, Expat, libxml2, pugixml, TinyXML-2, or W3C fixtures |
| Refactoring discipline | N/A - no refactoring performed; greenfield skeleton only |
| AI tolerance contract | N/A - no AI component |
| Forbidden shortcuts | No parser fake that accepts real XML; no unchecked TODO paths in production API; no downloaded fixture blobs without license/provenance; no global CMake side effects for consumers |
| Data classification | Public |
| Proactive controls in play | OWASP C1 Define Security Requirements; C5 Validate All Inputs; C10 Handle All Errors and Exceptions |
| Abuse acceptance scenarios | `tm-xml-parsing-library-abuse-1`, `tm-xml-parsing-library-abuse-3`, `tm-xml-parsing-library-abuse-22`, `tm-xml-parsing-library-abuse-23`, `tm-xml-parsing-library-abuse-24` covered by option/default/error skeleton and provenance tests |

#### Out Of Scope / Must Not Do

- Do not implement full tokenizer, DOM, SAX, namespace resolution, or DTD validation.
- Do not vendor W3C test fixtures until license and redistribution terms are documented.
- Do not introduce runtime dependencies.
- Do not define final DOM ownership semantics beyond public type placeholders and documented intent.

#### Files Allowed To Change

| File | Planned Change |
|---|---|
| `CMakeLists.txt` | NEW: project, library target, options, install/export/test wiring |
| `cmake/XmlParserConfig.cmake.in` | NEW: package config template |
| `include/xmlparser/xmlparser.h` | NEW: aggregate include |
| `include/xmlparser/*.h` | NEW: version, options, errors, forward DOM/SAX API headers |
| `src/*.cpp` | NEW: minimal compiled library implementation |
| `tests/**` | NEW: Catch2 BDD/unit/e2e/consumer tests |
| `docs/requirements-traceability.md` | NEW: requirement-to-test matrix |
| `docs/conformance-fixtures.md` | NEW: fixture source/license/provenance plan |
| `README.md` | Add build, test, and include examples |
| `.gitignore` | Add CMake, coverage, test output, and local artifact patterns |
| `docs/slo/lessons/xmlparser-m1.md` | NEW after milestone completion |
| `docs/slo/completion/xmlparser-m1.md` | NEW after milestone completion |

#### Step By Step

1. Create failing BDD tests for project metadata, API compile, option defaults, unsupported parse behavior, and CMake consumer integration.
2. Add CMake skeleton with static/shared support through `BUILD_SHARED_LIBS`.
3. Add test-only Catch2 acquisition and CTest labels `bdd` and `e2e`.
4. Add public headers in `xmlparser::v1` with C++17 standard library types only.
5. Add typed exceptions and `SourceLocation` with line, column, and byte offset fields.
6. Add placeholder parse APIs that fail visibly with typed unsupported errors until M2+.
7. Add docs for requirement traceability and conformance fixture provenance.
8. Add install/export package config and an install-tree consumer test.
9. Add `.gitignore` patterns for generated build, coverage, and test artifacts.
10. Run build, tests, smoke consumer, static-analysis placeholders, cleanup checks, and write lessons/completion docs.

#### BDD Acceptance Scenarios

**Feature: project skeleton and public API frame**

| Scenario | Category | Given | When | Then |
|---|---|---|---|---|
| M1 API include compiles | happy path | a C++17 translation unit includes `<xmlparser/xmlparser.h>` | it references `xmlparser::v1::ParserOptions` and `XmlVersion` | compilation succeeds without non-standard extensions |
| M1 CMake consumer works | compatibility | XMLParser is installed to a temp prefix | a separate CMake project calls `find_package(XmlParser CONFIG REQUIRED)` | it links `xmlparser::xmlparser` and runs a minimal program |
| M1 unsupported parser is visible | invalid input | the skeleton parse API receives `<root/>` before M2 | the caller invokes parse | it throws a typed exception/error indicating unsupported implementation, not success |
| M1 empty input is defined | empty state | the skeleton parse API receives empty input | parse is invoked | it returns a typed error path, not a crash |
| M1 options are bounded | resource bound | default `ParserOptions` is constructed | tests inspect limit fields | all parser-controlled bounds are positive and finite |
| M1 raw XML is not logged | abuse case | input contains secret-looking XML text | parse fails in skeleton mode | the error message omits the raw XML payload |
| M1 fixture provenance exists | compliance | conformance fixtures are not yet vendored | docs are generated | `docs/conformance-fixtures.md` lists source URL, license check, and import gate |

#### Regression Tests

- `ctest --test-dir build --output-on-failure` must pass after M1 implementation.
- C++17 compile check must pass with the default compiler.
- Install-tree consumer test must pass for at least one build type locally.

#### Compatibility Checklist

- [ ] `README.md` still names the project and documents build/test usage.
- [ ] `LICENSE` is unchanged.
- [ ] Public API is in `xmlparser::v1`.
- [ ] `BUILD_SHARED_LIBS` controls static/shared library mode.
- [ ] Default build has no runtime dependency beyond the standard library.

#### E2E Runtime Validation

**File**: `tests/e2e_m1_cmake_consumer.cpp` plus a CMake-driven temp consumer test.

| E2E Test | What It Proves | Pass Criteria |
|---|---|---|
| `m1_install_tree_consumer_can_find_package` | Installed package can be consumed externally | temp project configures, builds, and runs |
| `m1_public_header_compiles_as_cxx17` | Public header is portable C++17 | translation unit compiles under project settings |

#### Smoke Tests

- [ ] `cmake -S . -B build -DXMLPARSER_BUILD_TESTS=ON` configures.
- [ ] `cmake --build build` passes.
- [ ] `ctest --test-dir build --output-on-failure` passes.
- [ ] Install-tree consumer test passes.
- [ ] `git status` shows no untracked test artifacts except intentional new source/docs.
- [ ] `.gitignore` covers build and generated coverage/test artifacts.

#### Evidence Log

| Step | Command / Check | Expected Result | Actual Result | Pass/Fail | Notes |
|---|---|---|---|---|---|
| Repo hygiene | `git status --short --branch`; `git rev-parse --abbrev-ref HEAD`; `git symbolic-ref --short refs/remotes/origin/HEAD`; `git switch -c slo/xml-parsing-library-m1` | execution occurs on a task branch with existing user work preserved | Before: `main...origin/main` with untracked SLO docs; default: `origin/main`; after: `slo/xml-parsing-library-m1` | Pass | No carry-forward from prior retros because this is M1. |
| Baseline tests | `ctest --test-dir build --output-on-failure` if build exists | no existing tests or green | No `build` directory existed before M1 scaffold, so no baseline suite was present to run. | Pass | Greenfield baseline. |
| BDD tests created | `tests/**` | fail for expected missing implementation | Created `tests/bdd_m1_public_api.cpp`; pre-scaffold `cmake -S . -B build -DXMLPARSER_BUILD_TESTS=ON` failed because `CMakeLists.txt` did not exist; after implementation `ctest --test-dir build --output-on-failure -L bdd` passed 1 test in 0.01s. | Pass | Failure reason was expected missing scaffold, not a production-code success. |
| E2E stubs created | `tests/e2e_m1_cmake_consumer.cpp` | fail before CMake/package config exists | Created install-tree and public-header compile E2E scenarios; pre-scaffold configure failed due missing `CMakeLists.txt`; after implementation `ctest --test-dir build --output-on-failure -L e2e` passed 1 test in 1.14s. | Pass | The E2E test installs to a temp prefix and consumes `find_package(XmlParser CONFIG REQUIRED)`. |
| Implementation | skeleton files | contract satisfied | Added CMake project, `xmlparser::xmlparser` target, install package config, public headers under `include/xmlparser`, and `src/xmlparser.cpp` typed placeholder behavior. | Pass | No runtime dependency introduced; parser accepts no real XML in M1. |
| Formatter | `cmake --build build --target format` | clean or documented target absence | Placeholder target passed after removing a semicolon from the echo text that CMake treated as a list separator. | Pass | Output documents no formatter configured in M1. |
| Typecheck / build check | `cmake --build build` | clean | Build passed with `xmlparser`, Catch2, and `xmlparser_tests` targets up to date. | Pass | AppleClang 21.0.0.21000099 detected locally. |
| Static analyzer / linter | `cmake --build build --target lint` | clean or documented target absence | Placeholder target passed after the same CMake echo text fix as `format`. | Pass | Output documents static analysis setup deferred. |
| Dependency audit | test-only dependency provenance review | pass or documented exception | `rg` review confirmed Catch2 is pinned as test-only in `CMakeLists.txt`, README documents no runtime dependency, and fixture license/import gate docs exist. | Pass | Catch2 tag is `v3.5.4`; W3C fixtures are documented only and not vendored. |
| Full tests | `ctest --test-dir build --output-on-failure` | green | Passed 2 of 2 tests in 1.18s after sequential rerun. | Pass | One earlier parallel rerun raced two install-tree E2E invocations; sequential verification passed. |
| E2E runtime | `ctest --test-dir build --output-on-failure -L e2e` | green | Passed 1 of 1 E2E test in 1.14s. | Pass | Covers header compile and install-tree consumer runtime. |
| Smoke tests | checklist above | all checked | Configure, build, full tests, install-tree consumer, artifact cleanup, and `.gitignore` checks all passed. | Pass | Commands recorded in adjacent evidence rows. |
| Resource-bound verification | default options test | finite positive bounds | `M1 options are bounded` passed under BDD label. | Pass | All parser-controlled default limits are positive and finite. |
| Invariant/assertion verification | API namespace/header compile tests | invariant encoded | BDD and E2E compile tests passed using `xmlparser::v1`, `<xmlparser/xmlparser.h>`, and C++17 compile features. | Pass | Public namespace and aggregate include are encoded in tests. |
| Debugger / state inspection | debugger command documented | test can be debugged | README documents `lldb -- build/xmlparser_tests "[bdd]"` and a `gdb --args` equivalent. | Pass | Debugger availability itself is environment-specific. |
| Test artifact cleanup | `git status` | no generated artifact residue | `git status --short` shows only intentional source/docs changes; generated `build/` content is ignored. | Pass | `git check-ignore -v build build/CMakeCache.txt build/_deps` confirmed ignore coverage. |
| .gitignore review | review `.gitignore` | patterns current | Added CMake build, generated metadata, coverage, sanitizer, and local test artifact patterns; `git check-ignore` confirmed `build/` coverage. | Pass | No generated test artifacts are tracked. |
| Compatibility checks | include/link/install tests | no regressions | README still names XMLParser, LICENSE unchanged, public API remains in `xmlparser::v1`, `BUILD_SHARED_LIBS` controls library type, and install consumer links `xmlparser::xmlparser`. | Pass | Default build has no runtime dependency beyond the C++17 standard library. |

#### Definition Of Done

- All listed BDD scenarios pass.
- E2E install-tree consumer validation passes.
- Full test suite passes.
- Formatter/build/static-analysis gates pass or absences are explicitly documented with follow-up target creation.
- No runtime dependency is introduced.
- Requirement traceability and conformance fixture provenance docs exist.
- `.gitignore` is updated for generated artifacts.
- Lessons and completion summary files are written.
- Milestone Tracker is updated.

#### Post-Flight

- Update [ARCHITECTURE.md](../ARCHITECTURE.md) if file layout differs from the diagram.
- Update [README.md](../README.md) with current build/test/install instructions.
- Update [docs/slo/design/xml-parsing-library-interfaces.md](slo/design/xml-parsing-library-interfaces.md) if public names change before they are locked.

---

### Milestone 2 - Encoding, Tokenizer, And XML 1.0 Well-Formed Parsing

**Status**: scope proposed; full contract must be authored after M1 is confirmed.

**Goal**: Implement XML 1.0 well-formed parsing core, UTF-8/UTF-16 BOM handling, source locations, declarations, comments, CDATA, processing instructions, character data, predefined entities, and character references.

**Primary requirements**: REQ-STD-01, REQ-STD-04, REQ-STD-05, REQ-ERR-02, REQ-ERR-05, REQ-ERR-06.

**Planned abuse coverage**: malformed encodings, oversized tokens, excessive depth, empty/truncated/null-equivalent input, raw-payload-safe diagnostics.

---

### Milestone 3 - Namespaces And Incremental SAX API

**Status**: scope proposed; full contract must be authored after M2 is confirmed.

**Goal**: Add XML Namespaces 1.0/1.1 resolution and SAX streaming parser with incremental chunked input and event handlers.

**Primary requirements**: REQ-STD-03, REQ-SAX-01 through REQ-SAX-05, REQ-ERR-03.

**Planned abuse coverage**: arbitrary chunk boundaries, namespace stack imbalance, callback exceptions, attribute namespace collisions, recoverable validity callback behavior.

---

### Milestone 4 - DOM Model, Mutation, Traversal, And Serialization

**Status**: scope proposed; full contract must be authored after M3 is confirmed.

**Goal**: Add DOM tree construction, mutation APIs, traversal, namespace-aware and unaware attribute access, and serialization to string/stream.

**Primary requirements**: REQ-DOM-01 through REQ-DOM-05, REQ-ERR-05.

**Planned abuse coverage**: DOM ownership cycles, node-count/depth limits, failed stream writes, namespace declaration preservation, exception-safe mutation rollback.

---

### Milestone 5 - DTD Validation, XML 1.1, Coverage, And Release Packaging

**Status**: scope proposed; full contract must be authored after M4 is confirmed.

**Goal**: Complete internal DTD validation, XML 1.1 behavior, optional external DTD resolver contract, cross-platform packaging evidence, and requirement-level coverage target.

**Primary requirements**: REQ-STD-02, REQ-ERR-01, REQ-ERR-04, REQ-PLAT-01 through REQ-PLAT-06, REQ-API-01 through REQ-API-04.

**Planned abuse coverage**: entity expansion limits, duplicate/invalid DTD declarations, external resolver opt-in only, 32/64-bit assumptions, shared/static builds, coverage threshold enforcement.

**Release gate**: M5 cannot be marked done until Section 8's Final Requirements Validation Gate is complete and every `REQ-*` row is `pass` or has an explicitly approved scope decision.

---

## 8. Final Requirements Validation Gate

This gate is the end-of-runbook proof that the requirements document has been met. It is intentionally separate from the milestone summaries: a milestone can pass its local tests while the product still fails the full requirements contract.

### Validation Rule

Before M5 is marked done:

1. `docs/requirements-traceability.md` must contain every `REQ-*` item below.
2. Each `REQ-*` item must point to at least one named automated test file and test case prefix from the Required Test Inventory below.
3. The parser-core coverage report must show at least 90 percent line coverage.
4. The W3C XML conformance fixture result summary must be generated and linked.
5. Linux, macOS, Windows, static-library, shared-library, C++17 public-header, and install-tree consumer checks must pass.
6. Any unsupported interpretation must be written as an approved scope decision, not silently skipped.

### Required Test Inventory

Each milestone contract must create or extend the test files listed here. A milestone is incomplete if it implements a capability but leaves the matching requirement test absent, skipped, or only manually verified.

| Test File / Validation | CTest Label | Test Case Prefix | Requirements Covered |
|---|---|---|---|
| `tests/req/std_xml10_conformance_tests.cpp` | `req;conformance` | `REQ_STD_01_*` | REQ-STD-01 |
| `tests/req/std_xml11_tests.cpp` | `req;conformance` | `REQ_STD_02_*` | REQ-STD-02 |
| `tests/req/std_namespaces_tests.cpp` | `req;bdd` | `REQ_STD_03_*` | REQ-STD-03 |
| `tests/req/std_encoding_tests.cpp` | `req;bdd` | `REQ_STD_04_*` | REQ-STD-04 |
| `tests/req/std_markup_constructs_tests.cpp` | `req;bdd` | `REQ_STD_05_*` | REQ-STD-05 |
| `tests/req/dom_model_tests.cpp` | `req;bdd` | `REQ_DOM_01_*` | REQ-DOM-01 |
| `tests/req/dom_mutation_tests.cpp` | `req;bdd` | `REQ_DOM_02_*` | REQ-DOM-02, REQ-ERR-05 |
| `tests/req/dom_traversal_tests.cpp` | `req;bdd` | `REQ_DOM_03_*` | REQ-DOM-03 |
| `tests/req/dom_serialization_tests.cpp` | `req;bdd` | `REQ_DOM_04_*` | REQ-DOM-04 |
| `tests/req/dom_attribute_access_tests.cpp` | `req;bdd` | `REQ_DOM_05_*` | REQ-DOM-05 |
| `tests/req/sax_streaming_tests.cpp` | `req;bdd` | `REQ_SAX_01_*` | REQ-SAX-01 |
| `tests/req/sax_event_coverage_tests.cpp` | `req;bdd` | `REQ_SAX_02_*` | REQ-SAX-02 |
| `tests/req/sax_callback_registration_tests.cpp` | `req;bdd` | `REQ_SAX_03_*` | REQ-SAX-03 |
| `tests/req/sax_incremental_tests.cpp` | `req;bdd` | `REQ_SAX_04_*` | REQ-SAX-04 |
| `tests/req/sax_namespace_event_tests.cpp` | `req;bdd` | `REQ_SAX_05_*` | REQ-SAX-05 |
| `tests/req/error_validation_distinction_tests.cpp` | `req;bdd` | `REQ_ERR_01_*` | REQ-ERR-01 |
| `tests/req/error_location_exception_tests.cpp` | `req;bdd` | `REQ_ERR_02_*` | REQ-ERR-02 |
| `tests/req/error_recoverable_callback_tests.cpp` | `req;bdd` | `REQ_ERR_03_*` | REQ-ERR-03 |
| `tests/req/error_dtd_validation_tests.cpp` | `req;bdd` | `REQ_ERR_04_*` | REQ-ERR-04 |
| `tests/req/error_exception_safety_tests.cpp` | `req;bdd;sanitizer` | `REQ_ERR_05_*` | REQ-ERR-05 |
| `tests/req/error_malformed_input_tests.cpp` | `req;bdd;sanitizer` | `REQ_ERR_06_*` | REQ-ERR-06 |
| `tests/e2e/platform_cmake_consumer_tests.cpp` | `req;e2e` | `REQ_PLAT_03_*` | REQ-PLAT-03 |
| `tests/e2e/platform_static_shared_tests.cmake` | `req;e2e` | `REQ_PLAT_05_*` | REQ-PLAT-05 |
| `tests/e2e/platform_public_header_tests.cpp` | `req;e2e` | `REQ_PLAT_02_*`, `REQ_PLAT_06_*`, `REQ_API_01_*`, `REQ_API_02_*`, `REQ_API_03_*` | REQ-PLAT-02, REQ-PLAT-06, REQ-API-01, REQ-API-02, REQ-API-03 |
| `tests/e2e/platform_dependency_audit.cmake` | `req;e2e` | `REQ_PLAT_04_*` | REQ-PLAT-04 |
| `.github/workflows/ci.yml` or equivalent CI matrix | `req;ci` | `REQ_PLAT_01_*` | REQ-PLAT-01 |
| `tests/coverage/parser_core_coverage.cmake` | `req;coverage` | `REQ_API_04_*` | REQ-API-04 |
| `tests/fixtures/w3c/xmlconf/manifest.*` plus harness output | `req;conformance` | `W3C_XMLCONF_*` | REQ-STD-01, REQ-STD-02, REQ-STD-03 |

### Required Test Case Detail

The required test files above must include at least these concrete cases. Add more when implementation discoveries demand it; do not remove a case without updating this runbook and the traceability matrix.

| Requirement | Required Test Cases |
|---|---|
| REQ-STD-01 | `REQ_STD_01_accepts_well_formed_xml10_documents`; `REQ_STD_01_rejects_xml10_well_formedness_violations`; `REQ_STD_01_runs_w3c_xml10_conformance_manifest` |
| REQ-STD-02 | `REQ_STD_02_selects_xml11_at_runtime`; `REQ_STD_02_distinguishes_xml10_and_xml11_character_rules`; `REQ_STD_02_runs_xml11_conformance_subset` |
| REQ-STD-03 | `REQ_STD_03_resolves_default_namespace_scope`; `REQ_STD_03_resolves_prefixed_names_across_nested_scopes`; `REQ_STD_03_rejects_duplicate_expanded_attribute_names`; `REQ_STD_03_handles_namespace_11_undeclaration` |
| REQ-STD-04 | `REQ_STD_04_detects_utf8_bom`; `REQ_STD_04_detects_utf16le_bom`; `REQ_STD_04_detects_utf16be_bom`; `REQ_STD_04_rejects_truncated_multibyte_sequence_with_byte_offset` |
| REQ-STD-05 | `REQ_STD_05_parses_xml_declaration`; `REQ_STD_05_emits_processing_instruction`; `REQ_STD_05_preserves_cdata_boundaries_in_events`; `REQ_STD_05_parses_comments`; `REQ_STD_05_resolves_predefined_and_character_entities` |
| REQ-DOM-01 | `REQ_DOM_01_builds_document_element_attribute_text_comment_pi_cdata_nodes`; `REQ_DOM_01_preserves_node_type_identity_after_parse` |
| REQ-DOM-02 | `REQ_DOM_02_creates_inserts_modifies_and_removes_nodes`; `REQ_DOM_02_rejects_cycle_creating_insert`; `REQ_DOM_02_rolls_back_failed_mutation` |
| REQ-DOM-03 | `REQ_DOM_03_returns_parent_children_and_siblings`; `REQ_DOM_03_depth_first_iterator_visits_nodes_in_document_order` |
| REQ-DOM-04 | `REQ_DOM_04_serializes_to_string`; `REQ_DOM_04_serializes_to_stream`; `REQ_DOM_04_preserves_namespace_declarations`; `REQ_DOM_04_reports_stream_failure` |
| REQ-DOM-05 | `REQ_DOM_05_get_attribute_namespace_unaware`; `REQ_DOM_05_get_attribute_ns_distinguishes_same_local_name_different_uri` |
| REQ-SAX-01 | `REQ_SAX_01_parses_without_dom_allocation`; `REQ_SAX_01_streams_large_document_with_bounded_memory` |
| REQ-SAX-02 | `REQ_SAX_02_emits_document_start_end`; `REQ_SAX_02_emits_element_start_end`; `REQ_SAX_02_emits_character_pi_comment_and_cdata_events` |
| REQ-SAX-03 | `REQ_SAX_03_registers_virtual_handler`; `REQ_SAX_03_registers_std_function_callbacks`; `REQ_SAX_03_allows_selective_callbacks` |
| REQ-SAX-04 | `REQ_SAX_04_accepts_one_byte_chunks`; `REQ_SAX_04_accepts_chunks_split_inside_markup`; `REQ_SAX_04_accepts_chunks_split_inside_multibyte_sequence`; `REQ_SAX_04_finish_rejects_truncated_document` |
| REQ-SAX-05 | `REQ_SAX_05_element_events_include_uri_local_and_qname`; `REQ_SAX_05_attribute_events_include_uri_local_and_qname` |
| REQ-ERR-01 | `REQ_ERR_01_reports_well_formedness_error_kind`; `REQ_ERR_01_reports_dtd_validity_error_kind`; `REQ_ERR_01_does_not_confuse_validity_with_parse_failure` |
| REQ-ERR-02 | `REQ_ERR_02_exception_contains_message_line_column_and_byte_offset`; `REQ_ERR_02_location_tracks_utf16_input_offsets` |
| REQ-ERR-03 | `REQ_ERR_03_recoverable_callback_observes_validity_error`; `REQ_ERR_03_non_recoverable_well_formedness_error_stops_parse` |
| REQ-ERR-04 | `REQ_ERR_04_validates_internal_dtd_subset`; `REQ_ERR_04_external_dtd_resolver_is_not_called_by_default`; `REQ_ERR_04_external_dtd_resolver_is_called_only_when_configured` |
| REQ-ERR-05 | `REQ_ERR_05_callback_throw_does_not_leak`; `REQ_ERR_05_dom_mutation_throw_does_not_leak_or_corrupt_tree`; `REQ_ERR_05_sanitizers_pass_exception_paths` |
| REQ-ERR-06 | `REQ_ERR_06_empty_input_returns_defined_error`; `REQ_ERR_06_null_equivalent_input_returns_defined_error`; `REQ_ERR_06_truncated_input_returns_defined_error`; `REQ_ERR_06_fuzz_corpus_has_no_ub_or_sanitizer_findings` |
| REQ-PLAT-01 | `REQ_PLAT_01_ci_matrix_linux_gcc11`; `REQ_PLAT_01_ci_matrix_linux_clang14`; `REQ_PLAT_01_ci_matrix_macos_apple_clang14`; `REQ_PLAT_01_ci_matrix_windows_msvc_v142` |
| REQ-PLAT-02 | `REQ_PLAT_02_public_headers_compile_as_cxx17`; `REQ_PLAT_02_public_headers_reject_compiler_extension_dependency` |
| REQ-PLAT-03 | `REQ_PLAT_03_find_package_imported_target_builds_consumer`; `REQ_PLAT_03_installed_config_version_file_is_generated` |
| REQ-PLAT-04 | `REQ_PLAT_04_default_link_has_no_non_std_runtime_dependency`; `REQ_PLAT_04_optional_feature_dependencies_are_disabled_by_default` |
| REQ-PLAT-05 | `REQ_PLAT_05_builds_static_library`; `REQ_PLAT_05_builds_shared_library`; `REQ_PLAT_05_consumers_link_both_modes` |
| REQ-PLAT-06 | `REQ_PLAT_06_public_headers_compile_for_32_bit_when_toolchain_available`; `REQ_PLAT_06_public_api_uses_fixed_width_or_standard_size_types_intentionally` |
| REQ-API-01 | `REQ_API_01_public_symbols_are_in_xmlparser_v1` |
| REQ-API-02 | `REQ_API_02_public_api_uses_standard_string_view_unique_ptr_optional`; `REQ_API_02_public_api_exposes_no_custom_string_or_smart_pointer` |
| REQ-API-03 | `REQ_API_03_single_aggregate_header_exposes_full_api` |
| REQ-API-04 | `REQ_API_04_all_requirements_have_tests`; `REQ_API_04_parser_core_line_coverage_is_at_least_90_percent` |

### Validation Scope Decision Required

`REQ-ERR-01` mentions "DTD/schema violations", while `REQ-ERR-04` specifically requires internal DTD validation and makes external DTD validation optional. This runbook treats **validity** as XML DTD validity for v1. XML Schema validation is not implemented by this runbook. If the requirements owner intends XSD/XML Schema validation to be mandatory, M5 is not enough and a follow-up runbook or an expanded M5 contract is required before release.

### Requirement Coverage Matrix

| Requirement | Milestone | Required End-State Evidence |
|---|---|---|
| REQ-STD-01 XML 1.0 5th Edition | M2, M5 | `tests/req/std_xml10_conformance_tests.cpp`; W3C XML 1.0 conformance fixtures pass or documented approved exceptions |
| REQ-STD-02 XML 1.1 selectable | M5 | `tests/req/std_xml11_tests.cpp`; runtime or compile-time selection tests pass |
| REQ-STD-03 Namespaces 1.0/1.1 | M3, M5 | `tests/req/std_namespaces_tests.cpp`; namespace scope and Namespaces 1.1 behavior tests pass |
| REQ-STD-04 UTF-8/UTF-16 LE/BE and BOM | M2 | `tests/req/std_encoding_tests.cpp`; BOM and malformed/truncated encoding tests pass |
| REQ-STD-05 declarations, PIs, CDATA, comments, entities | M2, M3, M4 | `tests/req/std_markup_constructs_tests.cpp`; SAX and DOM parity cases pass |
| REQ-DOM-01 DOM node tree | M4 | `tests/req/dom_model_tests.cpp` passes |
| REQ-DOM-02 DOM read/write mutation | M4 | `tests/req/dom_mutation_tests.cpp` passes |
| REQ-DOM-03 traversal | M4 | `tests/req/dom_traversal_tests.cpp` passes |
| REQ-DOM-04 serialization | M4, M5 | `tests/req/dom_serialization_tests.cpp` passes |
| REQ-DOM-05 attribute access modes | M4 | `tests/req/dom_attribute_access_tests.cpp` passes |
| REQ-SAX-01 streaming parser | M3 | `tests/req/sax_streaming_tests.cpp` passes |
| REQ-SAX-02 event coverage | M3 | `tests/req/sax_event_coverage_tests.cpp` passes |
| REQ-SAX-03 registerable handlers | M3 | `tests/req/sax_callback_registration_tests.cpp` passes |
| REQ-SAX-04 incremental chunking | M3 | `tests/req/sax_incremental_tests.cpp` passes |
| REQ-SAX-05 namespace event data | M3 | `tests/req/sax_namespace_event_tests.cpp` passes |
| REQ-ERR-01 well-formedness vs validity | M5 | `tests/req/error_validation_distinction_tests.cpp` passes; approved decision on XML Schema scope exists |
| REQ-ERR-02 typed parse exception location | M2 | `tests/req/error_location_exception_tests.cpp` passes |
| REQ-ERR-03 recoverable SAX error callback | M3, M5 | `tests/req/error_recoverable_callback_tests.cpp` passes |
| REQ-ERR-04 internal DTD and optional external DTD | M5 | `tests/req/error_dtd_validation_tests.cpp` passes |
| REQ-ERR-05 exception safety / RAII | M2, M3, M4, M5 | `tests/req/error_exception_safety_tests.cpp` and sanitizer/leak checks pass |
| REQ-ERR-06 malformed input no UB | M2-M5 | `tests/req/error_malformed_input_tests.cpp` and fuzz/sanitizer checks pass |
| REQ-PLAT-01 compiler/platform matrix | M5 | CI matrix jobs with `REQ_PLAT_01_*` pass |
| REQ-PLAT-02 C++17 public headers | M1, M5 | `tests/e2e/platform_public_header_tests.cpp` passes under C++17 |
| REQ-PLAT-03 CMake install package | M1, M5 | `tests/e2e/platform_cmake_consumer_tests.cpp` passes |
| REQ-PLAT-04 no mandatory runtime dependencies | M1-M5 | `tests/e2e/platform_dependency_audit.cmake` passes |
| REQ-PLAT-05 static/shared build | M1, M5 | `tests/e2e/platform_static_shared_tests.cmake` passes |
| REQ-PLAT-06 32-bit/64-bit headers | M5 | `tests/e2e/platform_public_header_tests.cpp` 32-bit/64-bit cases pass or approved limitation exists |
| REQ-API-01 versioned namespace | M1-M5 | `tests/e2e/platform_public_header_tests.cpp` `REQ_API_01_*` cases pass |
| REQ-API-02 standard C++ public types | M1-M5 | `tests/e2e/platform_public_header_tests.cpp` `REQ_API_02_*` cases pass |
| REQ-API-03 aggregate include | M1-M5 | `tests/e2e/platform_public_header_tests.cpp` `REQ_API_03_*` cases pass |
| REQ-API-04 test suite and 90 percent coverage | M1-M5 | `tests/coverage/parser_core_coverage.cmake` passes and every `REQ_*` case above is present |

### Final Release Checklist

- [ ] Every row in the Requirement Coverage Matrix has passing evidence.
- [ ] Every file in the Required Test Inventory exists and is registered with the listed CTest label.
- [ ] Every Required Test Case Detail entry exists as an automated test or CI validation.
- [ ] `docs/requirements-traceability.md` links each requirement to tests and evidence artifacts.
- [ ] XML Schema/XSD scope is explicitly approved as out of scope or added to a new runbook.
- [ ] W3C conformance fixture results are attached or linked.
- [ ] Parser-core line coverage is at least 90 percent.
- [ ] Static and shared library builds pass.
- [ ] Install-tree consumer test passes.
- [ ] Cross-platform/compiler matrix passes or any missing lane has an approved release-blocking exception.
- [ ] No default runtime dependency beyond the C++ standard library is present.
- [ ] Security review checklist in [docs/slo/design/xml-parsing-library-threat-model.md](slo/design/xml-parsing-library-threat-model.md) is complete.

---

## 9. Documentation Update Table

| Milestone | ARCHITECTURE.md Update | README.md Update | .gitignore Update | Other Docs |
|---|---|---|---|---|
| 1 | Confirm file layout and build/package surfaces | Build/test/install/header examples | CMake, coverage, test artifacts | requirements traceability, conformance fixtures |
| 2 | Add tokenizer and encoding details | Basic parsing examples and error examples | sanitizer/fuzz artifacts if introduced | parser core notes |
| 3 | Add namespace/SAX data flow | SAX and incremental feed examples | streaming fixture outputs if any | SAX API docs |
| 4 | Add DOM/serializer flow | DOM mutation/traversal/serialization examples | serializer test outputs if any | DOM API docs |
| 5 | Add validation/release packaging details | release support matrix and validation options | coverage/release artifacts | coverage report, release checklist |

---

## 10. Handoff

Next SLO step: review the critique in [docs/slo/critique/xml-parsing-library.md](slo/critique/xml-parsing-library.md), approve or expand the XML Schema/XSD scope decision in Section 8, then confirm Milestone 1 scope, allow-list, and BDD specificity. After confirmation, run `/slo-execute M1`.
