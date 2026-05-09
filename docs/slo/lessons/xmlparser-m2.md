# Lessons Learned - xmlparser Milestone 2

## What changed

- Replaced M1 one-shot parser placeholders with an XML 1.0 parser core for UTF-8, UTF-8 BOM, UTF-16LE BOM, UTF-16BE BOM, source-location errors, elements, attributes, comments, CDATA, processing instructions, text, predefined entities, character references, and parser resource limits.

## Design decisions and why

- Kept the parser core internal under `src/parser_core.*` so M3 can add incremental state without exposing unstable implementation details.
- Used the existing `SaxHandler` as the M2 observation surface because final DOM ownership belongs to M4.
- Left `SaxParser::feed` and `finish` unsupported so M3 can design chunk carryover deliberately.
- Kept diagnostics structural and payload-free by using generic error messages plus source locations.

## Mistakes made

- `<?xml-stylesheet ...?>` was initially treated as an XML declaration because detection checked only the `<?xml` prefix.
- The traceability matrix still had one row describing M1 unsupported parser behavior after M2 made one-shot parsing real.

## Root causes

- XML declaration recognition needs to check the grammar boundary after `xml`, not only a string prefix.
- Historical milestone tests/docs need deliberate migration when a later milestone replaces placeholder behavior.

## What was harder than expected

- Keeping byte offsets correct across UTF-16 input was more delicate than the ASCII parser path.
- Attribute values needed stable string storage while `AttributeView` exposes `std::string_view` during callbacks.

## Naming conventions established

- Internal parser implementation lives in `src/parser_core.h` and `src/parser_core.cpp`.
- Requirement tests for parser behavior live under `tests/req/`.
- Shared requirement-test helpers live in `tests/req/parser_event_recorder.h`.
- M2 verification report path is `docs/slo/verify/xmlparser-m2.md`.

## Test patterns that worked well

- A small recording SAX handler made public API tests precise without exposing internal tokens.
- Tiny parser limits make resource-bound behavior easy to assert.
- UTF-16 tests build byte strings explicitly instead of relying on platform encoding.

## Missing tests that should exist now

- More XML name grammar edge cases, including non-ASCII names, should be added when conformance fixtures land.
- Sanitizer and fuzz smoke tests should be added before parser-core changes grow much further.
- Attribute duplicate-name and malformed comment edge cases need broader coverage.

## Rules for the next milestone

- M3 must preserve the M2 one-shot parser behavior while adding incremental `feed`/`finish`.
- Incremental parsing must share decoder/tokenizer behavior with M2 rather than creating a separate SAX parser.
- Add tests for chunks split inside UTF-8/UTF-16 multibyte sequences before implementing carryover logic.
- Keep `AttributeView` string-view lifetimes valid only during callbacks unless M3 explicitly documents a longer-lived view contract.
- Namespace resolution must build on raw QName parsing without changing M2 well-formedness error locations.

## Template improvements suggested

- Later milestone contracts should explicitly include "historical placeholder tests/docs may need migration" when a placeholder becomes real behavior.
- Parser milestones should have a standard sanitizer/fuzz evidence row once tooling is available.
