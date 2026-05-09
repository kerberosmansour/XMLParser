# Verification Report - xmlparser Milestone 1

## What Was Exercised

| Scenario | Category | How exercised | Result | Evidence |
|---|---|---|---|---|
| M1 API include compiles | happy path | `ctest --test-dir build --output-on-failure -L bdd` compiled and ran the aggregate include scenario. | pass | `xmlparser_bdd` passed. |
| M1 CMake consumer works | compatibility | `ctest --test-dir build --output-on-failure -L e2e` installed XMLParser to a temp prefix, configured a separate CMake project, linked `xmlparser::xmlparser`, and ran it. | pass | `xmlparser_e2e` passed. |
| M1 unsupported parser is visible | invalid input | BDD scenario called `xmlparser::v1::parse("<root/>")`. | pass | Threw `XmlParseException` with `ErrorKind::Unsupported`. |
| M1 empty input is defined | empty state | BDD scenario called `xmlparser::v1::parse("")`. | pass | Threw `XmlParseException` with `ErrorKind::EmptyInput` at line 1, column 1, byte offset 0. |
| M1 options are bounded | resource bound | BDD scenario inspected every default parser-controlled limit. | pass | All defaults were positive and finite. |
| M1 raw XML is not logged | abuse case | BDD scenario parsed secret-looking XML and inspected the thrown message. | pass | Error message omitted the full payload, `hunter2`, and `secret-token`. |
| M1 fixture provenance exists | compliance | BDD scenario read `docs/conformance-fixtures.md`. | pass | Source URL, license check, and import gate are documented. |
| M1 public header compiles as CXX17 | E2E | E2E scenario generated a separate CMake project using only `include/xmlparser`. | pass | Header-only compile project built successfully. |
| M1 install tree consumer can find package | E2E | E2E scenario ran `cmake --install`, `find_package(XmlParser CONFIG REQUIRED)`, linked the imported target, and executed the consumer. | pass | Consumer returned success after observing the unsupported typed error. |

## Bugs Found

| id | severity | scenario | regression test | status |
|---|---|---|---|---|
| N/A | N/A | No product bugs found during verification. | N/A | N/A |

## Security Pass

| Check | Scope | Result | Evidence |
|---|---|---|---|
| Supply-chain manifest detection | Rust, Node, Python, Go manifests | N/A | `rg --files` found no `Cargo.toml`, `package.json`, `pyproject.toml`, `requirements.txt`, or `go.mod`. |
| Test-only dependency review | Catch2 acquisition | pass | `CMakeLists.txt` pins Catch2 `v3.5.4`; README documents no runtime dependency. |
| DAST smoke-service gate | Runtime service surface | N/A | No service, OpenAPI spec, or `docker-compose.yml` exists; this is a compiled library milestone. |
| AI tolerance | AI/LLM behavior | N/A | Contract block says no AI component. |

## Environment

- OS: Darwin 25.4.0 arm64.
- Compiler: Apple clang 21.0.0.
- CMake: 4.0.3.
- Browser/UI: N/A, no UI surface in M1.

## Coverage Gaps

- Full XML tokenization, SAX event streaming, DOM construction, namespace handling, serialization, and DTD validation are intentionally out of scope for M1 and covered by later milestones.
- Formatter and linter are placeholder CMake targets in M1; real tool selection remains deferred.
