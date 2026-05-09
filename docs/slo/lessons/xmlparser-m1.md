# Lessons Learned - xmlparser Milestone 1

## What changed

- Added the CMake project, exported `xmlparser::xmlparser` target, install package config, public API skeleton, typed placeholder parser behavior, BDD tests, E2E install-tree consumer test, traceability docs, fixture provenance docs, README guidance, and `.gitignore` coverage.

## Design decisions and why

- Keep runtime dependencies at zero by putting Catch2 behind `XMLPARSER_BUILD_TESTS` only.
- Pin Catch2 to `v3.5.4` in FetchContent so test acquisition is reproducible enough for M1 and visibly test-only.
- Make `parse("")` throw `ErrorKind::EmptyInput` and non-empty XML throw `ErrorKind::Unsupported` so empty input is defined without pretending XML parsing exists.
- Use a generated external CMake consumer inside the E2E test because package installation is part of the public compatibility surface.
- Use a CMake 3.20-compatible top-level test default instead of relying on newer `PROJECT_IS_TOP_LEVEL` behavior.

## Mistakes made

- The first `format` and `lint` target messages used semicolons, which CMake treated as list separators in generated build rules.
- Full CTest and the E2E-labelled CTest were briefly run in parallel, causing two install-tree validations to race against the same build tree.
- The runbook control file `docs/RUNBOOK-xml-parsing-library.md` is required by the SLO execute/retro flow but is not listed in the M1 file allow-list.

## Root causes

- CMake command arguments preserve semicolon list semantics even when the text looks like a harmless shell echo string.
- The install-tree E2E test performs `cmake --install` from the shared build tree, so verification commands that invoke it should be serialized.
- The runbook template separates implementation files from control-artifact updates, but the milestone allow-list did not explicitly call that out.

## What was harder than expected

- The install-tree test is small but exercises several moving parts: package config generation, exported targets, install prefix layout, compiler features, and typed placeholder behavior.
- Keeping M1 honest required resisting a tempting mini-parser; typed unsupported errors are the correct behavior until M2.

## Naming conventions established

- Public headers live under `include/xmlparser/` and are aggregated by `include/xmlparser/xmlparser.h`.
- Public API lives in `xmlparser::v1`.
- M1 tests are `tests/bdd_m1_public_api.cpp` and `tests/e2e_m1_cmake_consumer.cpp`.
- CTest labels are `bdd` and `e2e`.
- Verification report path is `docs/slo/verify/xmlparser-m1.md`.

## Test patterns that worked well

- BDD tests assert behavior directly against public API types and error kinds.
- E2E tests create temporary external CMake projects instead of relying on source-tree assumptions.
- Diagnostics tests inspect error messages for absence of raw XML payload fragments.

## Missing tests that should exist now

- A `BUILD_SHARED_LIBS=ON` install-tree consumer check should be added once CI matrix support exists.
- Real formatter and linter targets should replace M1 placeholders.
- A sanitizer/fuzz smoke target should be introduced when tokenizer work begins in M2 or shortly after.

## Rules for the next milestone

- Do not run E2E install-tree CTest invocations in parallel against the same build directory.
- Avoid semicolons in CMake custom target echo text unless they are intentionally escaped.
- Preserve `ErrorKind::EmptyInput`, `ErrorKind::Unsupported`, and payload-free diagnostics while M2 replaces unsupported parsing with real tokenizer behavior.
- Keep every new parse failure typed with `SourceLocation` populated from the first byte that proves the error.
- If the runbook itself must be edited, list it explicitly in future milestone allow-lists or document it as an SLO control artifact.

## Template improvements suggested

- Add the active runbook path to each milestone's allowed control-artifact edits.
- Add guidance that install-tree E2E tests should not be run in parallel from the same build tree.
- Distinguish "placeholder target exists" from "real formatter/linter configured" in evidence templates.
