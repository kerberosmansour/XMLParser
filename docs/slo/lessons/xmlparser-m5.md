# Lessons Learned - xmlparser Milestone 5

## What changed

- Added XML 1.1 selection, version-specific character checks, internal DTD validation, bounded entity expansion, and caller-provided external DTD resolver support.
- Added release hardening evidence: CI workflow declaration, public-header/install/static/shared consumer checks, dependency audit, curated W3C fixture provenance, and parser-core coverage gate.
- Updated README, architecture notes, requirements traceability, conformance fixture documentation, and the final runbook release gate.

## Design decisions and why

- DTD validation extends the shared parser core instead of adding a second parser so one-shot, SAX, and DOM parse paths continue to share diagnostics and namespace behavior.
- External DTD resolution is a caller-provided callback and remains disabled by default; the library itself does not perform network or filesystem fetches.
- XML Schema/XSD validation is explicitly out of scope for this v1 runbook because the requirement set only mandated DTD validity behavior for the implemented release gate.
- M5 records a curated W3C-style data manifest rather than vendoring the upstream payload, avoiding accidental code execution or redistribution ambiguity.

## Mistakes made

- The first M5 verification run exposed that the conformance fixture doc rewrite dropped the exact lowercase phrase `import gate` required by an M1 BDD compatibility scenario.
- Initial parser-core coverage after the first M5 implementation was below the 90 percent release threshold, so the branch needed additional targeted tests for parser error and DTD branches.

## Root causes

- Compatibility checks can depend on precise documentation wording, not only semantic equivalence.
- DTD parsing introduced many small failure branches; broad happy-path tests did not cover enough of that new parser-core surface.

## What was harder than expected

- Keeping DTD validation on the existing event path while preserving SAX, DOM, namespace, and diagnostic behavior.
- Making external subset behavior testable without giving the library any default I/O authority.
- Building a local coverage gate that works with the available Apple clang coverage tooling and remains explicit when unsupported elsewhere.

## Naming conventions established

- Public external DTD opt-in type: `xmlparser::v1::ExternalDtdResolver`.
- M5 XML 1.1 tests: `tests/req/std_xml11_tests.cpp`.
- M5 DTD tests: `tests/req/error_dtd_validation_tests.cpp`.
- M5 validity distinction tests: `tests/req/error_validation_distinction_tests.cpp`.
- M5 release evidence tests: `tests/e2e/platform_*` and `tests/coverage/parser_core_coverage.cmake`.
- M5 verification report path: `docs/slo/verify/xmlparser-m5.md`.

## Test patterns that worked well

- Requirement-named Catch2 cases made the Section 8 release checklist auditable.
- External resolver tests used counters and oversized callback returns to prove both no-call default behavior and bounded opt-in behavior.
- Coverage-focused parser tests were most useful when they targeted one parser-core branch per case.
- The data-only fixture manifest gave conformance harness evidence without taking on upstream archive import risk.

## Missing tests that should exist now

- The complete upstream W3C XML Test Suite import, archive hash, exception report, and pass/fail summary should be handled in a later conformance runbook.
- Remote GitHub Actions matrix results should be attached when `/slo-ship` opens the PR.
- Sanitizer and 32-bit toolchain lanes should remain CI/nightly evidence unless the local machine has those toolchains available.

## Rules for the next milestone

- The next step is shipping, not another implementation milestone; run `/slo-ship` and treat remote CI results as the final merge gate.
- Do not relax the external DTD default: any resolver must remain caller-provided and opt-in.
- Preserve the exact requirement test names when updating docs, traceability, or release checklists.
- If full W3C fixture import is attempted later, keep fixtures data-only and record license, hash, and exception evidence before claiming conformance.

## Template improvements suggested

- Evidence logs should distinguish "CI workflow declared and locally inspected" from "remote hosted CI has run" so release packaging milestones do not overclaim.
- Documentation compatibility tests should name phrase-sensitive assertions in the runbook's compatibility checklist.
