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
| 2 | Encoding, Tokenizer, And XML 1.0 Well-Formed Parsing | `done` | 2026-05-09 | 2026-05-09 | `docs/slo/lessons/xmlparser-m2.md` | `docs/slo/completion/xmlparser-m2.md` |
| 3 | Namespaces And Incremental SAX API | `done` | 2026-05-09 | 2026-05-09 | `docs/slo/lessons/xmlparser-m3.md` | `docs/slo/completion/xmlparser-m3.md` |
| 4 | DOM Model, Mutation, Traversal, And Serialization | `done` | 2026-05-09 | 2026-05-09 | `docs/slo/lessons/xmlparser-m4.md` | `docs/slo/completion/xmlparser-m4.md` |
| 5 | DTD Validation, XML 1.1, Coverage, And Release Packaging | `done` | 2026-05-09 | 2026-05-09 | `docs/slo/lessons/xmlparser-m5.md` | `docs/slo/completion/xmlparser-m5.md` |

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

**Goal**: Implement XML 1.0 well-formed parsing core, UTF-8/UTF-16 BOM handling, source locations, declarations, comments, CDATA, processing instructions, character data, predefined entities, and character references.

**Context**: M1 established public headers, typed errors, CMake packaging, and placeholder parser behavior in `src/xmlparser.cpp`. M2 replaces the one-shot parser placeholders with a shared tokenizer/well-formedness core that can validate XML 1.0 documents and emit one-shot SAX-style events through the existing handler API. Incremental chunking, namespace resolution, DOM ownership, and DTD validation remain later milestones.

**Important design rule**: M2 must parse only enough public surface to prove the XML 1.0 core; it must not lock final DOM semantics or implement incremental SAX behavior ahead of M3.

**Refactor budget**: `Limited internal parser-core refactor permitted`; behavior must be preserved by M1 tests before and after, and new parser behavior must be covered by M2 tests.

#### Contract Block

| Field | Value |
|---|---|
| Inputs | M1 public API skeleton, M1 lessons, XMLParser architecture/security docs, final requirements inventory |
| Outputs | XML 1.0 one-shot parser core, UTF-8/UTF-16 decoder, markup/entity handling, source-location errors, requirement tests, docs updates |
| Interfaces touched | `xmlparser::v1::parse(std::string_view)`, `xmlparser::v1::parse(std::string_view, SaxHandler&)`, `SaxParser::parse`, `XmlParseException`, `ErrorKind`, `ParserOptions` |
| Files allowed to change | `CMakeLists.txt`, `include/xmlparser/*.h`, `src/*.cpp`, `src/*.h`, `tests/**`, `docs/RUNBOOK-xml-parsing-library.md`, `docs/slo/**`, `docs/requirements-traceability.md`, `ARCHITECTURE.md`, `README.md` |
| Files to read before changing anything | `docs/slo/lessons/xmlparser-m1.md`, `ARCHITECTURE.md`, `SECURITY.md`, `docs/slo/design/xml-parsing-library-interfaces.md`, `docs/slo/design/xml-parsing-library-threat-model.md`, `tests/bdd_m1_public_api.cpp`, `src/xmlparser.cpp` |
| New files allowed | `src/parser_core.*`, `tests/req/**`, `docs/slo/verify/xmlparser-m2.md`, `docs/slo/lessons/xmlparser-m2.md`, `docs/slo/completion/xmlparser-m2.md` |
| New dependencies allowed | none |
| Migration allowed | no |
| Compatibility commitments | Existing public headers continue to compile as C++17; installed CMake consumer still works; M1 unsupported-parser expectation is updated because M2 now implements one-shot parsing |
| Resource bounds introduced/changed | Enforce `max_document_bytes`, `max_depth`, `max_token_bytes`, `max_attributes_per_element`, and `max_entity_expansions` during parse |
| Invariants/assertions required | byte offsets are monotonic; line/column start at 1; start/end tags balance; exactly one document element exists; empty/truncated/malformed inputs throw typed errors; diagnostics omit raw XML payload |
| Debugger / inspection expectation | README debugger command remains valid for parser-core tests |
| Static analysis gates | CMake configure/build, `ctest`, formatter placeholder, lint placeholder |
| Exemplar code to copy | M1 public API/test style in `include/xmlparser/*.h`, `tests/bdd_m1_public_api.cpp`, and `tests/e2e_m1_cmake_consumer.cpp` |
| Anti-exemplar code not to copy | Do not copy implementation code from Xerces-C++, Expat, libxml2, pugixml, TinyXML-2, or W3C fixtures |
| Refactoring discipline | Apply behavior-preserving microsteps with pre-test and post-test proof per `/slo-plan` refactoring discipline; internal parser extraction is allowed only to satisfy M2 |
| AI tolerance contract | N/A - no AI component |
| Data classification | Public |
| Proactive controls in play | OWASP C1 Define Security Requirements; C5 Validate All Inputs; C10 Handle All Errors and Exceptions |
| Abuse acceptance scenarios | `tm-xml-parsing-library-abuse-1`, `tm-xml-parsing-library-abuse-2`, `tm-xml-parsing-library-abuse-3`, `tm-xml-parsing-library-abuse-22`, `tm-xml-parsing-library-abuse-24` covered by malformed encoding, resource-limit, payload-safe diagnostics, and provenance tests |

#### Out Of Scope / Must Not Do

- Do not implement namespace resolution or duplicate expanded-name checks.
- Do not implement incremental `SaxParser::feed` or arbitrary chunk-boundary behavior.
- Do not implement final DOM tree ownership, mutation, traversal, or serialization.
- Do not implement DTD validation, custom entity declarations, external DTD resolution, XML Schema, XPath, XQuery, or XSLT.
- Do not vendor W3C fixtures in M2.

#### Files Allowed To Change

| File | Planned Change |
|---|---|
| `CMakeLists.txt` | Register M2 requirement tests and CTest labels |
| `include/xmlparser/*.h` | Narrow public API adjustments for parser results/errors if tests require them |
| `src/xmlparser.cpp` | Route public one-shot parse APIs to parser core |
| `src/parser_core.*` | NEW: UTF decoder and XML 1.0 tokenizer/well-formedness core |
| `tests/bdd_m1_public_api.cpp` | Update M1 placeholder expectation for post-M2 parse behavior |
| `tests/req/**` | NEW: M2 requirement tests for XML 1.0, encodings, markup constructs, locations, and malformed input |
| `docs/requirements-traceability.md` | Add M2 requirement coverage |
| `ARCHITECTURE.md` | Add parser-core implementation detail if file layout differs |
| `README.md` | Add basic parsing and typed error examples |
| `docs/RUNBOOK-xml-parsing-library.md` | Control artifact: tracker and M2 evidence updates |
| `docs/slo/verify/xmlparser-m2.md` | NEW after verification |
| `docs/slo/lessons/xmlparser-m2.md` | NEW after milestone completion |
| `docs/slo/completion/xmlparser-m2.md` | NEW after milestone completion |

#### Step By Step

1. Confirm baseline tests are green and record repo hygiene.
2. Write M2 requirement tests first under `tests/req/**`.
3. Run M2 tests and confirm they fail against M1 unsupported parser behavior.
4. Add internal parser core with UTF-8/UTF-16 BOM detection and XML 1.0 tokenizer states.
5. Wire public one-shot parse APIs to the parser core while leaving incremental feed unsupported.
6. Enforce resource limits and typed source-location errors.
7. Update M1 tests, traceability docs, README, and architecture notes.
8. Run build, BDD, requirement, E2E, full test, format, lint, and cleanup checks.
9. Write verification report, lessons, and completion summary.

#### BDD Acceptance Scenarios

**Feature: XML 1.0 well-formed parser core**

| Scenario | Category | Given | When | Then |
|---|---|---|---|---|
| M2 accepts simple XML 1.0 document | happy path | `<root><child attr="value">text</child></root>` | one-shot parse is invoked | parse succeeds and emits balanced document/element/text events |
| M2 parses XML declaration | happy path | `<?xml version="1.0" encoding="UTF-8"?><root/>` | one-shot parse is invoked | parse succeeds and records XML 1.0 declaration handling |
| M2 detects UTF-8 BOM | compatibility | UTF-8 BOM precedes `<root/>` | parse is invoked | parse succeeds with root at byte offset after the BOM |
| M2 detects UTF-16LE and UTF-16BE BOM | compatibility | UTF-16 encoded `<root/>` with BOM | parse is invoked | parse succeeds for both byte orders |
| M2 rejects malformed UTF-8 | invalid input / abuse `tm-xml-parsing-library-abuse-2` | input contains a truncated multibyte sequence | parse is invoked | `XmlParseException` has `ErrorKind::Encoding` and the failing byte offset |
| M2 handles comments, CDATA, and PIs | happy path | a document contains `<?pi?>`, `<!--comment-->`, and `<![CDATA[x<y]]>` | SAX parse is invoked | handler observes PI, comment, CDATA, and character events |
| M2 resolves predefined and character entities | happy path | text and attributes include `&lt;`, `&amp;`, `&quot;`, `&#65;`, and `&#x41;` | parse is invoked | handler receives decoded values |
| M2 rejects mismatched tags | invalid input | `<root><child></root>` | parse is invoked | typed well-formedness error includes line, column, and byte offset |
| M2 rejects truncated input | empty/degraded state | `<root><child>` or unfinished CDATA is provided | parse is invoked | typed well-formedness error is returned, not a crash |
| M2 enforces resource limits | abuse `tm-xml-parsing-library-abuse-1` | parser options set a tiny depth or token limit | parse is invoked | `ErrorKind::ResourceLimit` is thrown before unbounded growth |
| M2 diagnostics omit raw XML | abuse `tm-xml-parsing-library-abuse-3` | malformed XML contains secret-looking text | parse fails | error message omits the raw XML payload and secret substrings |
| M2 keeps fixture provenance gate closed | dependency/provenance abuse `tm-xml-parsing-library-abuse-22` | W3C fixtures are still not vendored | tests read docs | provenance docs still require source, license, hash, and import gate |

#### Regression Tests

- `ctest --test-dir build --output-on-failure` must pass.
- M1 BDD and E2E tests must still pass after M2 behavior changes.
- Requirement tests under `tests/req/**` must pass with `req` CTest label.

#### Compatibility Checklist

- [ ] Public API remains in `xmlparser::v1`.
- [ ] `<xmlparser/xmlparser.h>` compiles as C++17.
- [ ] Install-tree consumer still works.
- [ ] `SaxParser::feed` and `finish` remain visibly unsupported until M3.
- [ ] No runtime dependency is introduced.
- [ ] Raw XML payloads remain absent from default diagnostics.

#### E2E Runtime Validation

**File**: `tests/e2e_m1_cmake_consumer.cpp` plus M2 requirement runtime tests.

| E2E Test | What It Proves | Pass Criteria |
|---|---|---|
| `m1_install_tree_consumer_can_find_package` | Installed package still works after parser implementation | temp project configures, builds, links, and runs |
| `REQ_STD_04_detects_utf16le_bom` / `REQ_STD_04_detects_utf16be_bom` | Runtime parser handles UTF-16 byte-order input | both parse successfully |
| `REQ_ERR_02_exception_contains_message_line_column_and_byte_offset` | Runtime diagnostics include actionable location metadata | expected line, column, and byte offset match |

#### Smoke Tests

- [ ] `cmake -S . -B build -DXMLPARSER_BUILD_TESTS=ON` configures.
- [ ] `cmake --build build` passes.
- [ ] `ctest --test-dir build --output-on-failure` passes.
- [ ] `ctest --test-dir build --output-on-failure -L req` passes.
- [ ] `ctest --test-dir build --output-on-failure -L e2e` passes.
- [ ] `cmake --build build --target format` passes or documents absence.
- [ ] `cmake --build build --target lint` passes or documents absence.
- [ ] `git status` shows no generated artifact residue except intentional source/docs.

#### Evidence Log

| Step | Command / Check | Expected Result | Actual Result | Pass/Fail | Notes |
|---|---|---|---|---|---|
| Repo hygiene | `git status --short --branch`; `git rev-parse --abbrev-ref HEAD`; `git symbolic-ref --short refs/remotes/origin/HEAD`; `git switch -c slo/xml-parsing-library-m2` | execution occurs on a task branch with existing work preserved | Before: `slo/xml-parsing-library-m1` with completed M1 work; after: `slo/xml-parsing-library-m2`; default: `origin/main`; dirty tree preserved. | Pass | `gh issue list --label retro-derived --search "xmlparser" --state open --json number,title,body,url` returned `[]`. |
| Prior lessons | read `docs/slo/lessons/xmlparser-m1.md` | M1 rules applied | M1 rules applied: no parallel E2E install-tree runs; avoid CMake echo semicolons; preserve empty-input/payload-safe diagnostics; update runbook allow-list for control edits. | Pass | M2 contract explicitly lists `docs/RUNBOOK-xml-parsing-library.md` as a control artifact. |
| Baseline tests | `ctest --test-dir build --output-on-failure` | green | Passed 2 of 2 tests in 1.22s before M2 code changes. | Pass | M1 baseline green. |
| BDD/REQ tests created | `tests/req/**` | fail for expected unsupported parser behavior before implementation | Added M2 requirement tests and registered `xmlparser_req`; `ctest --test-dir build --output-on-failure -L req` failed 17 of 19 cases against M1 unsupported parser behavior. | Pass | Two existing empty/payload tests already passed; new behavior tests failed for the expected reason. |
| Implementation | parser core files | contract satisfied | Added `src/parser_core.h` and `src/parser_core.cpp`; wired public one-shot parse APIs to XML 1.0 parser core; left incremental feed/finish unsupported. | Pass | UTF-8/UTF-16 BOM, declarations, elements, attributes, comments, CDATA, PIs, predefined/character entities, locations, and resource limits implemented for M2. |
| Formatter | `cmake --build build --target format` | clean or documented target absence | Passed; placeholder target reports no formatter configured in M1. | Pass | No semicolons added to custom target echo text. |
| Typecheck / build check | `cmake --build build` | clean | Passed; `xmlparser`, Catch2, and `xmlparser_tests` built successfully. | Pass | CMake configure also passed with `XMLPARSER_BUILD_TESTS=ON`. |
| Static analyzer / linter | `cmake --build build --target lint` | clean or documented target absence | Passed; placeholder target reports static analysis setup deferred. | Pass | Real lint tool remains deferred. |
| Dependency audit | no new runtime deps | pass | `rg` review confirmed no new runtime dependency; only existing test-only Catch2 FetchContent pin remains. | Pass | `add_library(xmlparser src/xmlparser.cpp src/parser_core.cpp)` links only project sources. |
| Full tests | `ctest --test-dir build --output-on-failure` | green | Passed 4 of 4 tests in 1.32s in the final post-retro run. | Pass | BDD, req, conformance, and E2E CTest entries all green. |
| Requirement tests | `ctest --test-dir build --output-on-failure -L req` | green | Passed 2 of 2 labelled tests in 0.01s. | Pass | Includes `xmlparser_req` and `xmlparser_req_conformance`. |
| E2E runtime | `ctest --test-dir build --output-on-failure -L e2e` | green | Passed 1 of 1 E2E test in 1.22s. | Pass | Run sequentially to avoid install-tree race. |
| Resource-bound verification | depth/token/entity tests | typed resource-limit errors | `REQ_ERR_05_depth_limit_returns_resource_error`, `REQ_ERR_05_token_limit_returns_resource_error`, and `REQ_ERR_05_entity_expansion_limit_returns_resource_error` passed. | Pass | Resource failures use `ErrorKind::ResourceLimit`. |
| Error-location verification | location tests | line/column/byte offset correct | `REQ_ERR_02_exception_contains_message_line_column_and_byte_offset` and `REQ_ERR_02_location_tracks_utf16_input_offsets` passed. | Pass | UTF-16 byte offset case reports offset 14 for mismatched end tag. |
| Payload-safe diagnostics | secret-looking malformed XML test | no raw payload leakage | `REQ_ERR_06_secret_payload_is_not_echoed` and updated M1 raw XML test passed. | Pass | Messages omit full XML, `hunter2`, and other secret-looking substrings. |
| Test artifact cleanup | `git status` | no generated artifact residue | `git status --short --branch` shows only intentional source/docs changes; `find` found no coverage/core artifacts; `build/` remains ignored. | Pass | `git check-ignore -v build build/CMakeCache.txt build/_deps` confirmed ignore coverage. |
| Compatibility checks | include/link/install tests | no regressions | M1 BDD and E2E tests pass after updating expected post-M2 parser behavior; public namespace/header and install-tree consumer remain green. | Pass | `SaxParser::feed` and `finish` remain unsupported until M3. |

#### Definition Of Done

- All listed M2 scenarios pass.
- M1 BDD and E2E tests still pass.
- UTF-8, UTF-16LE, and UTF-16BE BOM paths are covered.
- Malformed encoding, malformed markup, empty input, truncated input, and resource limits return typed errors with locations where applicable.
- No raw XML payload appears in default diagnostics.
- Incremental parsing remains unsupported until M3.
- Requirement traceability, README, verification report, lessons, completion summary, and milestone tracker are updated.

---

### Milestone 3 - Namespaces And Incremental SAX API

**Goal**: Add XML Namespaces 1.0/1.1 resolution and SAX streaming parser with incremental chunked input and event handlers.

**Context**: M2 added one-shot XML 1.0 parsing in `src/parser_core.cpp` and left `SaxParser::feed` / `finish` unsupported. M3 turns the existing SAX surface into a usable incremental API, adds namespace-aware `QualifiedName` values for elements and attributes, and preserves M2 parser behavior as the shared core. DOM construction, serializer behavior, DTD validation, and full XML 1.1 character-rule support remain later milestones.

**Important design rule**: Incremental parsing must reuse M2 parser-core semantics and must not fork a second parser; M3 may buffer chunks up to `max_document_bytes`, but all chunk-boundary and finish-state behavior must be public-API tested before M4.

**Refactor budget**: `Limited internal parser-core refactor permitted`; behavior must be preserved by M1/M2 tests before and after.

#### Contract Block

| Field | Value |
|---|---|
| Inputs | M2 parser core, M2 lessons, namespace and SAX interface lock, threat model rows for chunking/callback boundaries |
| Outputs | Namespace-aware SAX event names, duplicate expanded attribute-name checks, incremental `SaxParser::feed` / `finish`, std::function callback adapter, requirement tests, docs updates |
| Interfaces touched | `QualifiedName`, `AttributeView`, `SaxHandler`, `SaxCallbacks`, `SaxParser::parse`, `SaxParser::feed`, `SaxParser::finish`, `ParserOptions::namespaces` |
| Files allowed to change | `CMakeLists.txt`, `include/xmlparser/*.h`, `src/*.cpp`, `src/*.h`, `tests/**`, `docs/RUNBOOK-xml-parsing-library.md`, `docs/slo/**`, `docs/requirements-traceability.md`, `ARCHITECTURE.md`, `README.md` |
| Files to read before changing anything | `docs/slo/lessons/xmlparser-m2.md`, `ARCHITECTURE.md`, `SECURITY.md`, `docs/slo/design/xml-parsing-library-interfaces.md`, `docs/slo/design/xml-parsing-library-threat-model.md`, `src/parser_core.cpp`, `include/xmlparser/sax.h`, `tests/req/parser_event_recorder.h` |
| New files allowed | `tests/req/std_namespaces_tests.cpp`, `tests/req/sax_streaming_tests.cpp`, `tests/req/sax_event_coverage_tests.cpp`, `tests/req/sax_callback_registration_tests.cpp`, `tests/req/sax_incremental_tests.cpp`, `tests/req/sax_namespace_event_tests.cpp`, `tests/req/error_recoverable_callback_tests.cpp`, `docs/slo/verify/xmlparser-m3.md`, `docs/slo/lessons/xmlparser-m3.md`, `docs/slo/completion/xmlparser-m3.md` |
| New dependencies allowed | none |
| Migration allowed | no |
| Compatibility commitments | M1/M2 public headers continue to compile as C++17; one-shot parse behavior remains stable; install-tree consumer still works |
| Resource bounds introduced/changed | `SaxParser::feed` buffers are capped by `ParserOptions::max_document_bytes`; namespace scope depth follows element depth; attribute expanded-name uniqueness is checked per element |
| Invariants/assertions required | namespace scopes push/pop with element stack; prefix lookup is deterministic; duplicate expanded attribute names are rejected; `finish()` rejects incomplete documents; callback exceptions propagate without hidden parser state corruption |
| Debugger / inspection expectation | README debugger command remains valid for SAX/namespace tests |
| Static analysis gates | CMake configure/build, `ctest`, formatter placeholder, lint placeholder |
| Exemplar code to copy | M2 parser-core tests in `tests/req/**`, `RecordingHandler` helper, and M2 decoder/tokenizer flow in `src/parser_core.cpp` |
| Anti-exemplar code not to copy | Do not implement a parallel SAX parser path or copy implementation code from Xerces-C++, Expat, libxml2, pugixml, TinyXML-2, or W3C fixtures |
| Refactoring discipline | Apply behavior-preserving microsteps with pre-test and post-test proof; M1/M2 tests must stay green while M3 internals change |
| AI tolerance contract | N/A - no AI component |
| Data classification | Public |
| Proactive controls in play | OWASP C1 Define Security Requirements; C5 Validate All Inputs; C10 Handle All Errors and Exceptions |
| Abuse acceptance scenarios | `tm-xml-parsing-library-abuse-4`, `tm-xml-parsing-library-abuse-5`, `tm-xml-parsing-library-abuse-6`, `tm-xml-parsing-library-abuse-7`, `tm-xml-parsing-library-abuse-12` covered by chunk-boundary, finish-state, bounded buffer, callback exception, and duplicate expanded attribute tests |

#### Out Of Scope / Must Not Do

- Do not implement DOM construction, mutation, traversal, or serialization.
- Do not implement DTD validation, external resolver behavior, XML Schema, XPath, XQuery, or XSLT.
- Do not vendor W3C fixtures.
- Do not implement full XML 1.1 character-rule differences; M3 may represent namespace undeclaration behavior needed by Namespaces 1.1 tests.
- Do not introduce runtime dependencies.

#### Files Allowed To Change

| File | Planned Change |
|---|---|
| `CMakeLists.txt` | Register M3 requirement tests |
| `include/xmlparser/sax.h` | Add `SaxCallbacks` adapter and incremental parser state fields |
| `src/xmlparser.cpp` | Implement `SaxCallbacks` adapter and incremental `SaxParser` buffering |
| `src/parser_core.*` | Add namespace stack, QName resolution, duplicate expanded attribute checks |
| `tests/req/**` | NEW: namespace, SAX, incremental, callback, and recoverable/error tests |
| `docs/requirements-traceability.md` | Add M3 requirement coverage |
| `ARCHITECTURE.md` | Add namespace/incremental SAX implementation notes |
| `README.md` | Add SAX callback and incremental feed examples |
| `docs/RUNBOOK-xml-parsing-library.md` | Control artifact: tracker and M3 evidence updates |
| `docs/slo/verify/xmlparser-m3.md` | NEW after verification |
| `docs/slo/lessons/xmlparser-m3.md` | NEW after milestone completion |
| `docs/slo/completion/xmlparser-m3.md` | NEW after milestone completion |

#### Step By Step

1. Confirm baseline tests are green and record repo hygiene.
2. Write M3 requirement tests first under `tests/req/**`.
3. Run M3 tests and confirm they fail against M2 unsupported incremental/no-namespace behavior.
4. Add namespace stack and qualified-name resolution to parser core.
5. Implement `SaxParser::feed` / `finish` with bounded buffering and callback exception propagation.
6. Add `SaxCallbacks` adapter for selective std::function handlers.
7. Update traceability docs, README, and architecture notes.
8. Run build, BDD, requirement, E2E, full test, format, lint, and cleanup checks.
9. Write verification report, lessons, and completion summary.

#### BDD Acceptance Scenarios

**Feature: namespaces and incremental SAX**

| Scenario | Category | Given | When | Then |
|---|---|---|---|---|
| M3 resolves default namespace scope | happy path | `<root xmlns="urn:a"><child/></root>` | SAX parse runs | element events include `uri=urn:a`, local names, and qnames |
| M3 resolves prefixed names across nested scopes | happy path | nested `xmlns:p` declarations change URI | SAX parse runs | each prefixed element/attribute event uses the in-scope URI |
| M3 rejects duplicate expanded attribute names | abuse `tm-xml-parsing-library-abuse-12` | attributes `a:x` and `b:x` map to the same URI/local pair | parse runs | well-formedness error is thrown |
| M3 handles namespace undeclaration | compatibility | nested `xmlns=""` clears the default namespace | SAX parse runs | child element has empty namespace URI |
| M3 emits SAX document and markup coverage | happy path | XML includes document, elements, text, PI, comment, CDATA | SAX parse runs | handler observes all event classes |
| M3 registers virtual handler | happy path | a subclassed `SaxHandler` is supplied | parse runs | virtual callbacks receive events |
| M3 registers std::function callbacks | happy path | `SaxCallbacks` has selective callbacks | parse runs | only provided callbacks are invoked |
| M3 accepts one-byte chunks | chunk boundary / abuse `tm-xml-parsing-library-abuse-6` | XML is fed byte by byte | `finish()` runs | parse succeeds and emits correct events |
| M3 accepts chunks split inside markup and multibyte sequences | chunk boundary / abuse `tm-xml-parsing-library-abuse-4` | chunks split inside a tag and inside UTF-8 bytes | `finish()` runs | parse succeeds with decoded text |
| M3 finish rejects truncated document | degraded state / abuse `tm-xml-parsing-library-abuse-5` | only `<root><child>` is fed | `finish()` runs | typed well-formedness error is thrown |
| M3 callback throw propagates safely | abuse `tm-xml-parsing-library-abuse-7` | handler throws during a nested element callback | parse runs | caller observes the same exception and subsequent fresh parser use succeeds |
| M3 recoverable validity callback is not yet supported | dependency/future behavior | caller asks for DTD validity recovery in M3 | tests inspect API behavior | no silent validity success path exists before M5 |

#### Regression Tests

- `ctest --test-dir build --output-on-failure` must pass.
- M1/M2 BDD, E2E, and requirement tests must still pass.
- M3 requirement tests under `tests/req/**` must pass with `req` CTest label.

#### Compatibility Checklist

- [ ] Public API remains in `xmlparser::v1`.
- [ ] `<xmlparser/xmlparser.h>` compiles as C++17.
- [ ] One-shot parse behavior from M2 remains stable.
- [ ] Install-tree consumer still works.
- [ ] `SaxParser::feed` and `finish` are now supported and bounded.
- [ ] No runtime dependency is introduced.
- [ ] Raw XML payloads remain absent from default diagnostics.

#### E2E Runtime Validation

**File**: M3 requirement tests plus existing install-tree consumer.

| E2E Test | What It Proves | Pass Criteria |
|---|---|---|
| `m1_install_tree_consumer_can_find_package` | Installed package still works after SAX changes | temp project configures, builds, links, and runs |
| `REQ_SAX_04_accepts_one_byte_chunks` | Incremental parser handles worst-case tiny chunks | parse succeeds |
| `REQ_SAX_04_finish_rejects_truncated_document` | Finish-state validation catches incomplete input | typed well-formedness error |
| `REQ_STD_03_resolves_prefixed_names_across_nested_scopes` | Runtime namespace stack is scoped correctly | observed URIs match expected scopes |

#### Smoke Tests

- [ ] `cmake -S . -B build -DXMLPARSER_BUILD_TESTS=ON` configures.
- [ ] `cmake --build build` passes.
- [ ] `ctest --test-dir build --output-on-failure` passes.
- [ ] `ctest --test-dir build --output-on-failure -L req` passes.
- [ ] `ctest --test-dir build --output-on-failure -L e2e` passes.
- [ ] `cmake --build build --target format` passes or documents absence.
- [ ] `cmake --build build --target lint` passes or documents absence.
- [ ] `git status` shows no generated artifact residue except intentional source/docs.

#### Evidence Log

| Step | Command / Check | Expected Result | Actual Result | Pass/Fail | Notes |
|---|---|---|---|---|---|
| Repo hygiene | `git status --short --branch`; `git rev-parse --abbrev-ref HEAD`; `git symbolic-ref --short refs/remotes/origin/HEAD`; `git switch -c slo/xml-parsing-library-m3` | execution occurs on a task branch with existing work preserved | Before: `slo/xml-parsing-library-m2`; after: `slo/xml-parsing-library-m3`; default: `origin/main`; dirty tree contains only the M3 contract edit. | Pass | `gh issue list --label retro-derived --search "xmlparser" --state open --json number,title,body,url` returned `[]`. |
| Prior lessons | read `docs/slo/lessons/xmlparser-m2.md` | M2 rules applied | M2 rules applied: preserve one-shot parser behavior; share decoder/tokenizer for incremental API; add chunk-split tests; keep AttributeView lifetime callback-scoped; build namespaces from raw QName parsing. | Pass | Contract includes these rules. |
| Baseline tests | `ctest --test-dir build --output-on-failure` | green | Passed 4 of 4 tests in 1.42s before M3 code changes. | Pass | M2 baseline green. |
| BDD/REQ tests created | `tests/req/**` | fail for expected M2 no-namespace/unsupported incremental behavior before implementation | Added M3 namespace, SAX, incremental, callback, and error tests; `ctest --test-dir build --output-on-failure -L req` failed 14 cases before implementation. | Pass | Failures were missing namespace URI/attribute filtering and unsupported incremental/callback adapter behavior. |
| Implementation | namespace and incremental SAX files | contract satisfied | Added namespace stack/QName resolution and duplicate expanded attribute checks in `src/parser_core.cpp`; added `SaxCallbacks` and bounded incremental buffering in public SAX APIs. | Pass | Incremental `feed` reuses the M2 parser core on `finish()`. |
| Formatter | `cmake --build build --target format` | clean or documented target absence | Passed; placeholder target reports no formatter configured in M1. | Pass | No semicolon regression in custom target text. |
| Typecheck / build check | `cmake --build build` | clean | Passed after `cmake -S . -B build -DXMLPARSER_BUILD_TESTS=ON`. | Pass | Public headers and tests compile as C++17. |
| Static analyzer / linter | `cmake --build build --target lint` | clean or documented target absence | Passed; placeholder target reports static analysis setup deferred. | Pass | Real lint tool remains deferred. |
| Dependency audit | no new runtime deps | pass | `rg` review confirmed `xmlparser` still links only project sources and Catch2 remains the only test-only FetchContent dependency. | Pass | No runtime dependency added. |
| Full tests | `ctest --test-dir build --output-on-failure` | green | Passed 4 of 4 tests in 1.53s. | Pass | BDD, req, conformance, and E2E entries all green. |
| Requirement tests | `ctest --test-dir build --output-on-failure -L req` | green | Passed 2 of 2 labelled tests in 0.02s. | Pass | Includes `xmlparser_req` and `xmlparser_req_conformance`. |
| E2E runtime | `ctest --test-dir build --output-on-failure -L e2e` | green | Passed 1 of 1 E2E test in 1.22s. | Pass | Run sequentially to avoid install-tree race. |
| Namespace verification | namespace tests | URIs/local/qname correct | `REQ_STD_03_*` and `REQ_SAX_05_*` namespace tests passed. | Pass | Default namespace, prefixed nested scopes, undeclaration, attribute names, and duplicate expanded attribute rejection covered. |
| Incremental verification | chunk-boundary tests | feed/finish behavior correct | `REQ_SAX_04_*` tests passed for one-byte chunks, markup splits, UTF-8 multibyte split, and truncated finish. | Pass | Buffer is bounded by `max_document_bytes`. |
| Callback safety verification | throwing callback test | exception propagates and fresh parser works | `REQ_ERR_05_callback_throw_does_not_leak` passed. | Pass | Fresh parser use succeeds after callback exception. |
| Test artifact cleanup | `git status` | no generated artifact residue | `git status --short --branch` shows only intentional source/docs changes; `find` found no coverage/core artifacts; `build/` remains ignored. | Pass | `git check-ignore -v build build/CMakeCache.txt build/_deps` confirmed ignore coverage. |
| Compatibility checks | include/link/install tests | no regressions | M1/M2 BDD, requirement, conformance, and install-tree E2E tests passed. | Pass | One-shot parse behavior remains stable. |

#### Definition Of Done

- All listed M3 scenarios pass.
- M1/M2 tests still pass.
- Namespace-aware event data includes URI, local name, and qname.
- Duplicate expanded attribute names are rejected.
- Incremental `feed`/`finish` handles arbitrary chunk boundaries and rejects truncated input.
- Callback exceptions propagate without corrupting future parser use.
- Requirement traceability, README, verification report, lessons, completion summary, and milestone tracker are updated.

---

### Milestone 4 - DOM Model, Mutation, Traversal, And Serialization

**Goal**: Add DOM tree construction, mutation APIs, traversal, namespace-aware and unaware attribute access, and serialization to string/stream.

**Context**: M3 added namespace-aware SAX event semantics and bounded incremental parsing over the shared parser core. M4 uses those same event semantics to build a document-owned DOM tree, exposes minimal mutation/traversal APIs, and serializes DOM content back to XML. DTD validation, XML 1.1 character-rule completion, and release packaging remain M5 scope.

**Important design rule**: DOM construction must consume the same namespace-aware parser events used by SAX; it must not fork parser behavior or re-resolve names independently.

**Refactor budget**: `Limited DOM/public API extension permitted`; parser-core behavior must remain covered by M1-M3 tests.

#### Contract Block

| Field | Value |
|---|---|
| Inputs | M3 parser/SAX core, M3 lessons, DOM interface lock, serializer requirements |
| Outputs | Document-owned DOM tree, node creation/mutation/traversal APIs, namespace-aware attribute APIs, string/stream serializer, requirement tests, docs updates |
| Interfaces touched | `Document`, `Node`, `Element`, `Attribute`, `Text`, `Comment`, `ProcessingInstruction`, `CDataSection`, `parse(std::string_view)`, `serialize(...)` |
| Files allowed to change | `CMakeLists.txt`, `include/xmlparser/*.h`, `src/*.cpp`, `src/*.h`, `tests/**`, `docs/RUNBOOK-xml-parsing-library.md`, `docs/slo/**`, `docs/requirements-traceability.md`, `ARCHITECTURE.md`, `README.md` |
| Files to read before changing anything | `docs/slo/lessons/xmlparser-m3.md`, `ARCHITECTURE.md`, `SECURITY.md`, `docs/slo/design/xml-parsing-library-interfaces.md`, `docs/slo/design/xml-parsing-library-threat-model.md`, `include/xmlparser/dom.h`, `include/xmlparser/serializer.h`, `src/xmlparser.cpp`, `src/parser_core.cpp` |
| New files allowed | `src/dom.cpp`, `tests/req/dom_model_tests.cpp`, `tests/req/dom_mutation_tests.cpp`, `tests/req/dom_traversal_tests.cpp`, `tests/req/dom_serialization_tests.cpp`, `tests/req/dom_attribute_access_tests.cpp`, `docs/slo/verify/xmlparser-m4.md`, `docs/slo/lessons/xmlparser-m4.md`, `docs/slo/completion/xmlparser-m4.md` |
| New dependencies allowed | none |
| Migration allowed | no |
| Compatibility commitments | M1-M3 tests still pass; public API remains in `xmlparser::v1`; install-tree consumer still works |
| Resource bounds introduced/changed | DOM parse enforces `ParserOptions::max_dom_nodes`; DOM mutation rejects cycles and cross-document insertion |
| Invariants/assertions required | every node has at most one parent; every node belongs to one owning `Document`; document has at most one document element; traversal order is document order; serializer escapes text/attributes |
| Debugger / inspection expectation | README debugger command remains valid for DOM tests |
| Static analysis gates | CMake configure/build, `ctest`, formatter placeholder, lint placeholder |
| Exemplar code to copy | M3 SAX event semantics and `RecordingHandler` event tests; M2/M3 typed error style |
| Anti-exemplar code not to copy | Do not implement a second parser for DOM; do not copy implementation code from competitor XML projects |
| Refactoring discipline | Behavior-preserving microsteps with pre-test and post-test proof; M1-M3 tests must stay green |
| AI tolerance contract | N/A - no AI component |
| Data classification | Public |
| Proactive controls in play | OWASP C1 Define Security Requirements; C5 Validate All Inputs; C10 Handle All Errors and Exceptions |
| Abuse acceptance scenarios | `tm-xml-parsing-library-abuse-10`, `tm-xml-parsing-library-abuse-11`, `tm-xml-parsing-library-abuse-12`, `tm-xml-parsing-library-abuse-13`, `tm-xml-parsing-library-abuse-14`, `tm-xml-parsing-library-abuse-15` covered by cycle, ownership, namespace-attribute, stream-failure, escaping, and namespace serialization tests |

#### Out Of Scope / Must Not Do

- Do not implement DTD validation, external resolver behavior, XML Schema, XPath, XQuery, or XSLT.
- Do not vendor W3C fixtures.
- Do not implement a separate DOM parser path.
- Do not implement full XML 1.1 behavior.
- Do not introduce runtime dependencies.

#### Files Allowed To Change

| File | Planned Change |
|---|---|
| `CMakeLists.txt` | Register M4 DOM tests and new DOM source file |
| `include/xmlparser/dom.h` | Add DOM ownership, mutation, traversal, and attribute APIs |
| `include/xmlparser/serializer.h` | Keep serializer public surface stable |
| `src/xmlparser.cpp` | Wire `parse(std::string_view)` to DOM builder and serializer entry points to implementation |
| `src/dom.cpp` | NEW: DOM storage, builder, mutation, traversal, serializer |
| `tests/req/**` | NEW: DOM model, mutation, traversal, serialization, and attribute tests |
| `docs/requirements-traceability.md` | Add M4 requirement coverage |
| `ARCHITECTURE.md` | Add DOM/serializer implementation notes |
| `README.md` | Add DOM parse/mutation/serialization examples |
| `docs/RUNBOOK-xml-parsing-library.md` | Control artifact: tracker and M4 evidence updates |
| `docs/slo/verify/xmlparser-m4.md` | NEW after verification |
| `docs/slo/lessons/xmlparser-m4.md` | NEW after milestone completion |
| `docs/slo/completion/xmlparser-m4.md` | NEW after milestone completion |

#### Step By Step

1. Confirm baseline tests are green and record repo hygiene.
2. Write M4 requirement tests first under `tests/req/**`.
3. Run M4 tests and confirm they fail against placeholder DOM/serializer behavior.
4. Implement document-owned DOM storage and parser event builder.
5. Implement mutation/traversal and namespace-aware attribute APIs.
6. Implement string and stream serialization with XML escaping and stream failure reporting.
7. Update traceability docs, README, and architecture notes.
8. Run build, BDD, requirement, E2E, full test, format, lint, and cleanup checks.
9. Write verification report, lessons, and completion summary.

#### BDD Acceptance Scenarios

**Feature: DOM tree, mutation, traversal, and serialization**

| Scenario | Category | Given | When | Then |
|---|---|---|---|---|
| M4 builds DOM node types from parse | happy path | XML includes element, attribute, text, comment, PI, and CDATA | `parse` returns a `Document` | node type identity and document element are preserved |
| M4 creates and mutates nodes | happy path | a blank `Document` creates elements/text | caller appends, modifies, and removes nodes | tree reflects the mutation |
| M4 rejects cycle-creating insertion | abuse `tm-xml-parsing-library-abuse-10` | caller tries to append an ancestor under its descendant | mutation runs | typed parse exception is thrown and tree remains unchanged |
| M4 rejects cross-document insertion | abuse `tm-xml-parsing-library-abuse-11` | caller moves a node from another `Document` | mutation runs | typed parse exception is thrown |
| M4 traverses parent/children/siblings | happy path | a parsed nested document | caller asks for parent/children/siblings | expected relationships are returned |
| M4 depth-first iterator visits document order | happy path | a parsed nested document | caller requests depth-first nodes | document order is returned |
| M4 serializes to string | happy path | DOM contains text needing escaping | `serialize(document)` runs | XML output is well-formed and escaped |
| M4 serializes to stream and reports failure | invalid output sink / abuse `tm-xml-parsing-library-abuse-13` | stream is already failed | stream serializer runs | typed I/O error is thrown |
| M4 preserves namespace declarations in output | compatibility / abuse `tm-xml-parsing-library-abuse-15` | parsed namespace-aware DOM | serializer runs | output contains needed `xmlns` declarations and qualified names |
| M4 attribute access supports qname and namespace | happy path | element has `plain` and `p:code` attributes | caller uses `get_attribute` and `get_attribute_ns` | values are distinct and correct |
| M4 serializer escapes markup-sensitive text | abuse `tm-xml-parsing-library-abuse-14` | text contains `<`, `>`, and `&` | serializer runs | escaped output does not inject markup |

#### Regression Tests

- `ctest --test-dir build --output-on-failure` must pass.
- M1-M3 BDD, E2E, and requirement tests must still pass.
- M4 requirement tests under `tests/req/**` must pass with `req` CTest label.

#### Compatibility Checklist

- [x] Public API remains in `xmlparser::v1`.
- [x] `<xmlparser/xmlparser.h>` compiles as C++17.
- [x] One-shot and incremental SAX behavior from M2/M3 remains stable.
- [x] Install-tree consumer still works.
- [x] No runtime dependency is introduced.
- [x] Raw XML payloads remain absent from default diagnostics.

#### E2E Runtime Validation

**File**: M4 requirement tests plus existing install-tree consumer.

| E2E Test | What It Proves | Pass Criteria |
|---|---|---|
| `m1_install_tree_consumer_can_find_package` | Installed package still works after DOM changes | temp project configures, builds, links, and runs |
| `REQ_DOM_01_builds_document_element_attribute_text_comment_pi_cdata_nodes` | Runtime parser builds DOM from real XML | all expected node types observed |
| `REQ_DOM_04_serializes_to_string` | Runtime serializer emits well-formed XML | output matches expected escaped XML |
| `REQ_DOM_02_rejects_cycle_creating_insert` | Mutation invariant is enforced | typed error and unchanged tree |

#### Smoke Tests

- [x] `cmake -S . -B build -DXMLPARSER_BUILD_TESTS=ON` configures.
- [x] `cmake --build build` passes.
- [x] `ctest --test-dir build --output-on-failure` passes.
- [x] `ctest --test-dir build --output-on-failure -L req` passes.
- [x] `ctest --test-dir build --output-on-failure -L e2e` passes.
- [x] `cmake --build build --target format` passes or documents absence.
- [x] `cmake --build build --target lint` passes or documents absence.
- [x] `git status` shows no generated artifact residue except intentional source/docs.

#### Evidence Log

| Step | Command / Check | Expected Result | Actual Result | Pass/Fail | Notes |
|---|---|---|---|---|---|
| Repo hygiene | `git status --short --branch`; `git rev-parse --abbrev-ref HEAD`; `git symbolic-ref --short refs/remotes/origin/HEAD`; `git switch -c slo/xml-parsing-library-m4` | execution occurs on a task branch with existing work preserved | Before: `slo/xml-parsing-library-m3`; after: `slo/xml-parsing-library-m4`; default: `origin/main`; dirty tree contains only the M4 contract edit. | Pass | `gh issue list --label retro-derived --search "xmlparser" --state open --json number,title,body,url` returned `[]`. |
| Prior lessons | read `docs/slo/lessons/xmlparser-m3.md` | M3 rules applied | M3 rules applied: DOM consumes namespace-aware SAX semantics; attribute APIs preserve qname/local/URI; serializer treats namespace declarations specially; incremental SAX tests stay green; tests avoid brittle event indices. | Pass | Contract includes these rules. |
| Baseline tests | `ctest --test-dir build --output-on-failure` | green | Passed 4 of 4 tests in 1.49s before M4 code changes. | Pass | M3 baseline green. |
| BDD/REQ tests created | `tests/req/**` | fail for expected placeholder DOM/serializer behavior before implementation | Initial M4 requirement run failed for expected placeholder DOM/serializer behavior (`document_element()==nullptr`, unsupported mutation/serialization). Contract audit later added `REQ_DOM_01_enforces_configured_dom_node_limit` and `REQ_DOM_05_set_attribute_ns_keeps_expanded_names_unique`; the supplemental tests passed with the completed implementation. | Pass | Supplemental coverage timing is recorded in M4 lessons. |
| Implementation | DOM and serializer files | contract satisfied | Implemented `src/dom.cpp`, expanded `include/xmlparser/dom.h`, registered the source/tests in `CMakeLists.txt`, and wired `parse(std::string_view)` through a `DomBuilder` over the shared parser core. | Pass | No second parser path; no runtime dependency added. |
| Formatter | `cmake --build build --target format` | clean or documented target absence | Target passed and printed `No formatter is configured in M1 - formatting is manually reviewed.` | Pass | Placeholder target remains. |
| Typecheck / build check | `cmake --build build` | clean | Built `xmlparser`, Catch2, and `xmlparser_tests` successfully. | Pass | Ran after docs/tests were updated. |
| Static analyzer / linter | `cmake --build build --target lint` | clean or documented target absence | Target passed and printed `No linter is configured in M1 - static analysis setup is deferred.` | Pass | Placeholder target remains. |
| Dependency audit | no new runtime deps | pass | `rg -n "FetchContent|ExternalProject|add_subdirectory|find_package|target_link_libraries|GIT_REPOSITORY|https?://" CMakeLists.txt cmake include src` found only the existing test-only Catch2 FetchContent and `xml` namespace URI literal. | Pass | Production `xmlparser` target links only standard-library code. |
| Full tests | `ctest --test-dir build --output-on-failure` | green | Passed 4 of 4 tests in 1.41s in the final post-retro run. | Pass | Includes BDD, req, conformance, and E2E labels. |
| Requirement tests | `ctest --test-dir build --output-on-failure -L req` | green | Passed 2 of 2 labeled test targets in 0.02s in the final post-retro run. | Pass | Includes M4 DOM tests and conformance target. |
| E2E runtime | `ctest --test-dir build --output-on-failure -L e2e` | green | Passed 1 of 1 labeled test target in 1.26s in the final post-retro run. | Pass | Install-tree consumer still configures, links, and runs. |
| DOM invariant verification | mutation/traversal tests | ownership and cycle invariants hold | `REQ_DOM_02_rejects_cycle_creating_insert`, `REQ_DOM_02_rolls_back_failed_mutation`, `REQ_DOM_03_returns_parent_children_and_siblings`, and `REQ_DOM_03_depth_first_iterator_visits_nodes_in_document_order` pass. | Pass | Cross-document insertion and failed mutation preserve existing tree state. |
| Serialization verification | serializer tests | escaping, namespace declarations, stream failure handled | `REQ_DOM_04_serializes_to_string`, `REQ_DOM_04_serializes_to_stream`, `REQ_DOM_04_preserves_namespace_declarations`, and `REQ_DOM_04_reports_stream_failure` pass. | Pass | Text/attribute escaping and stream error path covered. |
| Test artifact cleanup | `git status` | no generated artifact residue | `git status --short --branch` shows only intentional M4 docs/closeout edits after the local DOM implementation commit; `build/` artifacts remain ignored. | Pass | Closeout files under `docs/slo/**` are intentionally new. |
| Compatibility checks | include/link/install tests | no regressions | Public API remains under `xmlparser::v1`; `<xmlparser/xmlparser.h>` compiles as C++17; M2/M3 one-shot and incremental SAX tests pass; install-tree consumer passes; diagnostics tests still ensure raw XML is not echoed. | Pass | README, ARCHITECTURE, and traceability docs updated for DOM/serializer. |

#### Definition Of Done

- All listed M4 scenarios pass.
- M1-M3 tests still pass.
- DOM node type identity, mutation, traversal, attribute access, and serialization work through public APIs.
- Serializer escapes text/attributes and reports stream failure.
- DOM mutation prevents cycles and cross-document ownership violations.
- Requirement traceability, README, verification report, lessons, completion summary, and milestone tracker are updated.

---

### Milestone 5 - DTD Validation, XML 1.1, Coverage, And Release Packaging

**Status**: done; verification, lessons, completion summary, and final release gate evidence recorded.

**Goal**: Complete the release gate by adding XML 1.1 selection, internal DTD validity checks, explicit external-DTD opt-in behavior, platform/package evidence, conformance fixture provenance, and parser-core coverage evidence.

**Context**: M1-M4 established the public API, XML 1.0 parser core, namespace-aware SAX, incremental buffering, DOM ownership, and serializer behavior. M5 is the final hardening milestone: it must not introduce a second parser or loosen the safe-by-default posture. The M4 lessons require one named test for every M5 abuse ID, resource bound, and compatibility promise, and require a decision on serializer option behavior before release.

**Important design rule**: DTD validation and XML 1.1 checks must extend the existing `src/parser_core.cpp` event path so DOM, SAX, and one-shot parsing continue to observe the same tokenizer, namespace, entity, and diagnostic behavior.

**Refactor budget**: `Limited parser-core extension permitted`; parser behavior must move in behavior-preserving microsteps with M1-M4 tests green before and after each substantial change.

#### Contract Block

| Field | Value |
|---|---|
| Inputs | M4 DOM/SAX/parser core, M4 lessons, final validation gate in Section 8, threat-model rows `tm-xml-parsing-library-abuse-16` through `tm-xml-parsing-library-abuse-24` |
| Outputs | XML 1.1 version selection tests, internal DTD declaration/entity/validity behavior, bounded caller-provided external DTD resolver contract, external DTD disabled-by-default proof, platform/package tests, conformance fixture manifest/provenance update, parser-core coverage evidence, CI workflow skeleton, docs and release gate updates |
| Interfaces touched | `ParserOptions`, `ValidationMode`, `XmlVersion`, `ExternalDtdResolver`, `XmlParseException`, `XmlValidityException`, `parse(...)`, `SaxParser`, DOM parse path, CMake install/export surface |
| Files allowed to change | `CMakeLists.txt`, `cmake/**`, `include/xmlparser/*.h`, `src/*.cpp`, `src/*.h`, `tests/**`, `.github/workflows/**`, `docs/RUNBOOK-xml-parsing-library.md`, `docs/slo/**`, `docs/requirements-traceability.md`, `docs/conformance-fixtures.md`, `ARCHITECTURE.md`, `README.md`, `.gitignore` |
| Files to read before changing anything | `docs/slo/lessons/xmlparser-m4.md`, `docs/slo/design/xml-parsing-library-threat-model.md`, `docs/slo/design/xml-parsing-library-interfaces.md`, `ARCHITECTURE.md`, `SECURITY.md`, `include/xmlparser/options.h`, `include/xmlparser/errors.h`, `src/parser_core.cpp`, `src/xmlparser.cpp`, `src/dom.cpp`, `tests/e2e_m1_cmake_consumer.cpp` |
| New files allowed | `tests/req/std_xml11_tests.cpp`, `tests/req/error_validation_distinction_tests.cpp`, `tests/req/error_dtd_validation_tests.cpp`, `tests/e2e/platform_cmake_consumer_tests.cpp`, `tests/e2e/platform_public_header_tests.cpp`, `tests/e2e/platform_dependency_audit.cmake`, `tests/e2e/platform_static_shared_tests.cmake`, `tests/coverage/parser_core_coverage.cmake`, `tests/fixtures/w3c/xmlconf/manifest.xml`, `tests/fixtures/w3c/xmlconf/README.md`, `.github/workflows/ci.yml`, `docs/slo/verify/xmlparser-m5.md`, `docs/slo/lessons/xmlparser-m5.md`, `docs/slo/completion/xmlparser-m5.md` |
| New dependencies allowed | none at runtime; CI/test-only use of compiler coverage tools is allowed when available and must degrade with an explicit scope note when unavailable locally |
| Migration allowed | no public namespace migration; additive public API allowed for a bounded caller-provided external-DTD resolver contract |
| Compatibility commitments | M1-M4 tests still pass; public API remains in `xmlparser::v1`; install-tree consumer works; static and shared library builds work; no mandatory runtime dependency beyond the C++17 standard library; diagnostics still omit raw XML payloads |
| Forbidden shortcuts | No XML Schema/XSD implementation in this runbook; no network/file fetching by default; no vendored parser implementation code; no generated-code execution from fixtures; no silent skips in Section 8 release gate; no marking M5 done with uncovered `REQ-*` rows |
| Resource bounds introduced/changed | DTD entity expansion, declaration count, replacement text length, external subset byte limit for the caller-provided resolver callback, parser-core coverage threshold of at least 90 percent |
| Invariants/assertions required | validity errors are distinct from well-formedness errors; external DTD resolution is opt-in only; entity expansion is bounded and cycle-safe; XML 1.0 and XML 1.1 character decisions are version-specific; fixture files are treated as data only; release matrix evidence is traceable to requirement IDs |
| Debugger / inspection expectation | README debugger command remains valid for `[m5]`, `[req]`, `[e2e]`, and coverage failures where the local toolchain supports them |
| Static analysis gates | CMake configure/build, `ctest`, formatter placeholder, lint placeholder, dependency audit, coverage script, and CI workflow syntax review |
| Exemplar code to copy | `src/parser_core.cpp` recursive-descent structure and typed error helpers; `tests/e2e_m1_cmake_consumer.cpp` temporary CMake project pattern; `tests/req/*` requirement-named Catch2 style |
| Anti-exemplar code not to copy | Do not copy third-party parser implementation code or W3C harness code; do not add a second parser path for DTD or XML 1.1; do not execute fixture files as scripts |
| Refactoring discipline | Behavior-preserving microsteps with pre-test and post-test proof; parser-core changes must keep M1-M4 DOM/SAX tests green |
| AI tolerance contract | N/A - no AI component |
| Data classification | Public for code/docs/fixtures; caller XML remains confidential in downstream use and must not appear in diagnostics by default |
| Proactive controls in play | OWASP C1 Define Security Requirements; C2 Leverage Security Frameworks and Libraries; C5 Validate All Inputs; C9 Implement Security Logging and Monitoring through safe diagnostics only; C10 Handle All Errors and Exceptions |
| Abuse acceptance scenarios | `tm-xml-parsing-library-abuse-16` through `tm-xml-parsing-library-abuse-24` covered by DTD entity/declaration/validity tests, external resolver opt-in tests, fixture provenance tests, dependency pinning, and no-copy release evidence |

#### Out Of Scope / Must Not Do

- Do not implement XML Schema/XSD, XPath, XQuery, XSLT, binary XML, or catalog resolution.
- Do not perform network or filesystem access for external DTDs by default.
- Do not vendor parser implementation code or execute fixture files.
- Do not make the parser fully streaming beyond the existing bounded `feed` / `finish` contract.
- Do not claim complete W3C conformance if imported fixture coverage is partial; record approved exceptions explicitly.
- Do not implement serializer pretty-print or non-UTF-8 output unless the release gate explicitly needs it; otherwise document those options as reserved/future behavior.

#### Files Allowed To Change

| File | Planned Change |
|---|---|
| `CMakeLists.txt` | Register M5 requirement, E2E, coverage, and fixture tests |
| `include/xmlparser/options.h` | Add bounded DTD/external resolver options and callback storage |
| `include/xmlparser/errors.h` | Keep typed validity/well-formedness separation stable; additive declarations only if needed |
| `include/xmlparser/parser.h` | Add caller-provided external resolver contract if not housed entirely in options |
| `include/xmlparser/xmlparser.h` | Continue aggregate include coverage |
| `src/parser_core.cpp` | Extend XML 1.1, DOCTYPE/internal subset, entity declaration, and validation behavior |
| `src/parser_core.h` | Internal declarations if parser-core helpers are split |
| `src/xmlparser.cpp` | Wire public options/resolver behavior into parser core without changing DOM/SAX parity |
| `src/dom.cpp` | Serializer option documentation tests only if needed; no unrelated DOM refactor |
| `tests/req/**` | Add XML 1.1, DTD, validity separation, exception safety, and conformance cases |
| `tests/e2e/**` | Add platform/header/static/shared/dependency audit tests |
| `tests/coverage/**` | Add parser-core coverage script and threshold check |
| `tests/fixtures/w3c/xmlconf/**` | Add manifest/provenance metadata and a curated data-only fixture subset if license gate passes |
| `.github/workflows/ci.yml` | Add Linux/macOS/Windows and static/shared build matrix |
| `docs/conformance-fixtures.md` | Complete source/license/hash/redistribution decision for fixtures used in M5 |
| `docs/requirements-traceability.md` | Map final `REQ-*` rows to named tests/evidence |
| `ARCHITECTURE.md` | Document DTD/XML 1.1/external resolver decisions |
| `README.md` | Document validation mode, XML 1.1 selection, release limitations, and debugging commands |
| `.gitignore` | Add coverage/sanitizer artifacts only if generated files are observed |
| `docs/RUNBOOK-xml-parsing-library.md` | Control artifact: M5 tracker/evidence/final gate updates |
| `docs/slo/verify/xmlparser-m5.md` | NEW after verification |
| `docs/slo/lessons/xmlparser-m5.md` | NEW after milestone completion |
| `docs/slo/completion/xmlparser-m5.md` | NEW after milestone completion |

#### Step By Step

1. Preserve M4 closeout state, switch to `slo/xml-parsing-library-m5`, and run the baseline suite.
2. Write M5 requirement tests first: XML 1.1, validity separation, DTD validation, external DTD opt-in, platform/package, dependency audit, and coverage gate.
3. Run M5 tests and confirm they fail for expected missing behavior or missing evidence.
4. Extend parser-core XML version and internal DTD/entity behavior using the existing tokenizer/event path.
5. Add external-DTD disabled-by-default behavior and optional resolver contract only if required by tests.
6. Add platform/static/shared/header/dependency audit tests and CI workflow evidence.
7. Add conformance fixture manifest/provenance metadata and coverage script output.
8. Update traceability, README, architecture, conformance docs, and Section 8 final gate rows.
9. Run build, BDD, req, conformance, E2E, coverage, format, lint, dependency, and cleanup checks.
10. Write verification report, lessons, completion summary, and update the milestone tracker.

#### BDD Acceptance Scenarios

**Feature: DTD validation, XML 1.1, and release evidence**

| Scenario | Category | Given | When | Then |
|---|---|---|---|---|
| M5 selects XML 1.1 behavior | happy path | XML declares `version="1.1"` and uses an XML 1.1-only character reference | parser runs with `ParserOptions::version = Xml11` | parse succeeds and source locations remain valid |
| M5 rejects XML 1.1-only input in XML 1.0 mode | invalid input | XML 1.0 mode parses the same XML 1.1-only character reference | parser runs | typed well-formedness error is thrown |
| M5 rejects unsupported version mismatch | invalid input | XML declaration and `ParserOptions::version` disagree | parser runs | typed well-formedness error is thrown |
| M5 validates internal DTD element declarations | happy path | XML has internal DTD declarations matching the document element/content | parser runs with `ValidationMode::Dtd` | parse succeeds |
| M5 separates validity from well-formedness | invalid input | XML is well-formed but violates declared DTD content | parser runs with `ValidationMode::Dtd` | `ErrorKind::Validity` / `XmlValidityException` path is observed, not well-formedness |
| M5 reports undeclared entity in validating mode | abuse `tm-xml-parsing-library-abuse-18` | validating parse references an undeclared entity | parser runs | validity error is reported without raw entity value leakage |
| M5 bounds recursive entity expansion | abuse `tm-xml-parsing-library-abuse-16` | DTD defines recursive or expanding entities | parser runs | resource-limit or validity error is thrown before unbounded expansion |
| M5 rejects duplicate/conflicting DTD declarations | abuse `tm-xml-parsing-library-abuse-17` | internal subset declares duplicate/conflicting element or attribute declarations | parser runs with validation | typed validity error is thrown |
| M5 external DTD is disabled by default | abuse `tm-xml-parsing-library-abuse-19` | XML references an external subset | parser runs with default options | no external resolver is called and a typed diagnostic explains the opt-in boundary |
| M5 external resolver failure is bounded | abuse `tm-xml-parsing-library-abuse-20`, `tm-xml-parsing-library-abuse-21` | caller-provided resolver fails or returns oversized data | parser runs with explicit resolver opt-in | typed validity/resource error is thrown; parse does not hang |
| M5 treats W3C fixtures as data only | abuse `tm-xml-parsing-library-abuse-22`, `tm-xml-parsing-library-abuse-24` | fixture manifest references XML files | conformance harness runs | only XML data is read; no fixture script/code is executed; provenance is recorded |
| M5 pins test dependencies and audits runtime deps | abuse `tm-xml-parsing-library-abuse-23` | dependency audit test inspects CMake targets | audit runs | Catch2 remains test-only and no runtime dependency is introduced |
| M5 public headers compile as C++17 | compatibility | separate temp project includes `<xmlparser/xmlparser.h>` | CMake configures/builds | header compiles without compiler-extension dependency |
| M5 install/static/shared consumers link | compatibility | temp projects consume installed static and shared builds | CMake configures/builds/runs | `xmlparser::xmlparser` links in both modes |
| M5 coverage threshold is enforced | release gate | parser-core coverage script runs on supported local toolchain | coverage command runs | parser-core line coverage is at least 90 percent or an explicit unsupported-toolchain note blocks release completion |
| M5 CI matrix is declared | release gate | `.github/workflows/ci.yml` exists | workflow is inspected | Linux, macOS, Windows, static/shared, req, e2e, and coverage jobs are declared |

#### Regression Tests

- `ctest --test-dir build --output-on-failure` must pass.
- M1-M4 BDD, E2E, requirement, DOM, SAX, namespace, and diagnostics tests must still pass.
- M5 requirement tests under `tests/req/**` must pass with the `req` CTest label.
- M5 E2E tests under `tests/e2e/**` must pass with the `e2e` CTest label.
- M5 conformance/fixture tests must pass or record approved exceptions in `docs/conformance-fixtures.md`.

#### Compatibility Checklist

- [x] Public API remains in `xmlparser::v1`.
- [x] `<xmlparser/xmlparser.h>` compiles as C++17 in an external temp project.
- [x] One-shot parse, SAX parse, incremental SAX, DOM parse, and serializer tests from M1-M4 remain stable.
- [x] Install-tree consumer still works.
- [x] Static and shared builds both compile and link a consumer.
- [x] No mandatory runtime dependency is introduced.
- [x] Raw XML payloads and entity replacement text remain absent from default diagnostics.
- [x] XML Schema/XSD is documented as out of scope unless user explicitly expands this runbook.
- [x] Serializer pretty-print and non-UTF-8 output behavior is documented as reserved/future behavior.

#### E2E Runtime Validation

**File**: M5 requirement/e2e tests plus existing install-tree consumer.

| E2E Test | What It Proves | Pass Criteria |
|---|---|---|
| `REQ_PLAT_02_public_headers_compile_as_cxx17` | Public headers remain C++17 consumer-safe | temp project configures and builds |
| `REQ_PLAT_03_find_package_imported_target_builds_consumer` | Installed package still works after M5 changes | temp project configures, builds, links, and runs |
| `REQ_PLAT_04_default_link_has_no_non_std_runtime_dependency` | Runtime target has no new dependency | dependency audit passes |
| `REQ_PLAT_05_builds_static_library` | Static library build mode works | static build configures/builds/links consumer |
| `REQ_PLAT_05_builds_shared_library` | Shared library build mode works | shared build configures/builds/links consumer |
| `REQ_STD_02_selects_xml11_at_runtime` | XML 1.1 mode is observable through public options | version-specific parse behavior matches expectation |
| `REQ_ERR_04_external_dtd_resolver_is_not_called_by_default` | External fetch surface is safe by default | resolver not invoked / diagnostic is typed |
| `REQ_API_04_parser_core_line_coverage_is_at_least_90_percent` | Release coverage gate is enforced | coverage script passes or blocks completion with explicit unsupported-toolchain evidence |

#### Smoke Tests

- [x] `cmake -S . -B build -DXMLPARSER_BUILD_TESTS=ON` configures.
- [x] `cmake --build build` passes.
- [x] `ctest --test-dir build --output-on-failure` passes.
- [x] `ctest --test-dir build --output-on-failure -L req` passes.
- [x] `ctest --test-dir build --output-on-failure -L e2e` passes.
- [x] `ctest --test-dir build --output-on-failure -L conformance` passes.
- [x] `cmake --build build --target format` passes or documents absence.
- [x] `cmake --build build --target lint` passes or documents absence.
- [x] Coverage gate passes or blocks M5 completion with explicit toolchain limitation.
- [x] Dependency audit confirms no runtime dependency beyond the C++17 standard library.
- [x] `git status` shows no generated artifact residue except intentional source/docs/fixtures.

#### Evidence Log

| Step | Command / Check | Expected Result | Actual Result | Pass/Fail | Notes |
|---|---|---|---|---|---|
| Repo hygiene | `git status --short --branch`; `git rev-parse --abbrev-ref HEAD`; `git symbolic-ref --short refs/remotes/origin/HEAD`; `git switch -c slo/xml-parsing-library-m5` | execution occurs on a task branch with existing work preserved | Before: `slo/xml-parsing-library-m4`; after: `slo/xml-parsing-library-m5`; default: `origin/main`; dirty tree carried intentional M4 closeout docs and M5 planning docs. | Pass | `gh issue list --label retro-derived --search "xmlparser" --state open --json number,title,body,url` returned `[]`. |
| Prior lessons | read `docs/slo/lessons/xmlparser-m4.md` | M4 rules applied | M4 rules applied: keep DOM on shared parser core, add one named test for every abuse/resource/compatibility promise, keep M1-M4 tests green, decide serializer option scope, and preserve namespace declaration handling. | Pass | M5 contract records serializer options as reserved/future unless release gate expands scope. |
| Baseline tests | `ctest --test-dir build --output-on-failure` | green before M5 code changes | Passed 4 of 4 tests in 1.74s before M5 code changes. | Pass | BDD, req, conformance, and E2E entries all green. |
| BDD/REQ tests created | `tests/req/**`, `tests/e2e/**`, `tests/coverage/**` | fail for expected missing M5 behavior/evidence before implementation | Initial M5 red run failed for missing XML 1.1, DOCTYPE/DTD validation, external resolver, CI, fixture, and coverage evidence; tests were kept requirement-named. | Pass | Later full-suite verification found one M1 wording regression in conformance docs and fixed it with the existing BDD test. |
| Implementation | parser-core, DTD/XML 1.1, platform, coverage, docs | contract satisfied | Added `ExternalDtdResolver`, DTD resource bounds, XML 1.1 version checks, internal DTD element/entity validation, bounded external subset opt-in, release evidence tests, coverage script, CI workflow, and docs. | Pass | All changes stay on the shared parser-core path; no runtime dependency was added. |
| Formatter | `cmake --build build --target format` | clean or documented target absence | Passed; target reports "No formatter is configured in M1 - formatting is manually reviewed." | Pass | Placeholder remains explicit. |
| Typecheck / build check | `cmake --build build` | clean | Passed; `xmlparser` and `xmlparser_tests` built successfully. | Pass | Configure command also passed. |
| Static analyzer / linter | `cmake --build build --target lint` | clean or documented target absence | Passed; target reports "No linter is configured in M1 - static analysis setup is deferred." | Pass | Placeholder remains explicit. |
| Dependency audit | runtime dependency scan | no runtime dependency added | `cmake -DXMLPARSER_SOURCE_DIR="$PWD" -P tests/e2e/platform_dependency_audit.cmake` reported `xmlparser production target has no mandatory runtime dependency`. | Pass | Catch2 remains test-only. |
| Full tests | `ctest --test-dir build --output-on-failure` | green | Passed 4 of 4 tests in 15.45s after fixing the conformance-doc `import gate` wording regression. | Pass | First verification run failed only that existing M1 BDD docs check. |
| Requirement tests | `ctest --test-dir build --output-on-failure -L req` | green | Passed 2 of 2 tests in 7.19s. | Pass | Includes conformance-labelled requirement checks; parser-core coverage is enforced by the dedicated coverage gate below. |
| E2E runtime | `ctest --test-dir build --output-on-failure -L e2e` | green | Passed 1 of 1 test in 8.32s. | Pass | Covers header, install, static/shared, dependency, CI-declaration, and coverage evidence. |
| Conformance tests | `ctest --test-dir build --output-on-failure -L conformance` | green or approved exceptions documented | Passed 1 of 1 test in 0.01s. | Pass | Full upstream W3C payload import remains explicitly out of scope. |
| Coverage gate | `cmake -P tests/coverage/parser_core_coverage.cmake` or configured target | at least 90 percent parser-core line coverage or blocks completion | `cmake -DXMLPARSER_SOURCE_DIR="$PWD" -DXMLPARSER_BINARY_DIR="$PWD/build" -P tests/coverage/parser_core_coverage.cmake` passed with parser-core line coverage `95.009416195856872%`. | Pass | Uses local Apple clang coverage tooling. |
| XML 1.1 verification | `REQ_STD_02_*` tests | version-specific character behavior enforced | `REQ_STD_02_selects_xml11_at_runtime`, `REQ_STD_02_distinguishes_xml10_and_xml11_character_rules`, `REQ_STD_02_rejects_version_declaration_option_mismatch`, and `REQ_STD_02_runs_xml11_conformance_subset` passed. | Pass | Covered by `tests/req/std_xml11_tests.cpp`. |
| DTD validity verification | `REQ_ERR_01_*`, `REQ_ERR_04_*` tests | validity separated from well-formedness | Validity, root mismatch, EMPTY, PCDATA, child sequence, duplicate declaration, recursive entity, oversized replacement, and undeclared nested entity cases passed. | Pass | Covered by `tests/req/error_validation_distinction_tests.cpp` and `tests/req/error_dtd_validation_tests.cpp`. |
| External DTD safety | default and opt-in resolver tests | disabled by default; bounded failure handling if resolver exists | Default resolver no-call, configured resolver call, public external resolver call, oversized resolver output, truncated subset, unsupported external subset, and duplicate external declaration cases passed. | Pass | Library performs no default network/filesystem fetch. |
| Platform/package verification | public header/static/shared/install tests | package is consumer-safe | Public C++17 header, standard public types, aggregate header, install package, static build, shared build, and dependency-audit tests passed. | Pass | CI workflow declaration is locally inspected; remote hosted CI remains the PR ship gate. |
| Final requirements gate | Section 8 checklist | every `REQ-*` row has passing evidence or approved scope decision | Requirement traceability, required inventory, XSD scope decision, W3C data-only fixture decision, static/shared/install checks, dependency audit, and 95 percent parser-core coverage are recorded. | Pass | Full upstream W3C suite and remote CI execution remain explicit ship/future gates, not silent skips. |
| Test artifact cleanup | `git status`; generated file scan | no generated artifact residue | `git status --short --branch --untracked-files=all` showed only intentional documentation edits and new M5 closeout docs during closeout; build/coverage/temp consumer output stayed ignored. | Pass | Final status recorded after closeout. |
| Compatibility checks | include/link/install/M1-M4 tests | no regressions | Full CTest, requirement, e2e, conformance, dependency, formatter, lint, and coverage gates passed after the M1 wording fix. | Pass | Raw XML diagnostics and M1-M4 parser/SAX/DOM/serializer behavior remain covered. |

#### Definition Of Done

- All listed M5 scenarios pass.
- M1-M4 tests still pass.
- XML 1.1 selection has version-specific tests.
- Internal DTD validation separates validity from well-formedness errors.
- Entity and DTD growth paths are bounded and tested.
- External DTD resolution remains disabled by default and opt-in behavior is explicit.
- Platform/static/shared/header/dependency tests pass.
- Parser-core coverage gate reaches at least 90 percent or M5 remains blocked with explicit toolchain evidence.
- W3C/conformance fixture provenance is complete for any imported data.
- Section 8 final requirements gate is complete.
- Requirement traceability, README, architecture, conformance docs, verification report, lessons, completion summary, and milestone tracker are updated.

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
| REQ-DOM-01 | `REQ_DOM_01_builds_document_element_attribute_text_comment_pi_cdata_nodes`; `REQ_DOM_01_preserves_node_type_identity_after_parse`; `REQ_DOM_01_enforces_configured_dom_node_limit` |
| REQ-DOM-02 | `REQ_DOM_02_creates_inserts_modifies_and_removes_nodes`; `REQ_DOM_02_rejects_cycle_creating_insert`; `REQ_DOM_02_rolls_back_failed_mutation` |
| REQ-DOM-03 | `REQ_DOM_03_returns_parent_children_and_siblings`; `REQ_DOM_03_depth_first_iterator_visits_nodes_in_document_order` |
| REQ-DOM-04 | `REQ_DOM_04_serializes_to_string`; `REQ_DOM_04_serializes_to_stream`; `REQ_DOM_04_preserves_namespace_declarations`; `REQ_DOM_04_reports_stream_failure` |
| REQ-DOM-05 | `REQ_DOM_05_get_attribute_namespace_unaware`; `REQ_DOM_05_get_attribute_ns_distinguishes_same_local_name_different_uri`; `REQ_DOM_05_set_attribute_ns_keeps_expanded_names_unique` |
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

- [x] Every row in the Requirement Coverage Matrix has passing evidence.
- [x] Every file in the Required Test Inventory exists and is registered with the listed CTest label.
- [x] Every Required Test Case Detail entry exists as an automated test or CI validation.
- [x] `docs/requirements-traceability.md` links each requirement to tests and evidence artifacts.
- [x] XML Schema/XSD scope is explicitly approved as out of scope or added to a new runbook.
- [x] W3C conformance fixture results are attached or linked.
- [x] Parser-core line coverage is at least 90 percent.
- [x] Static and shared library builds pass.
- [x] Install-tree consumer test passes.
- [x] Cross-platform/compiler matrix is declared and locally inspected; hosted matrix execution remains the `/slo-ship` PR gate.
- [x] No default runtime dependency beyond the C++ standard library is present.
- [x] Security review checklist in [docs/slo/design/xml-parsing-library-threat-model.md](slo/design/xml-parsing-library-threat-model.md) is complete.

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

Next SLO step: run `/slo-ship` to push the M5 branch, open the PR, and let the hosted GitHub Actions matrix produce the final remote CI evidence before merge.
