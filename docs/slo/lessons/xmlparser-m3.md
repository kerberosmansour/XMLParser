# Lessons Learned - xmlparser Milestone 3

## What changed

- Added namespace-aware SAX event data, duplicate expanded attribute-name rejection, `SaxCallbacks`, and bounded incremental `SaxParser::feed` / `finish` behavior over the shared parser core.

## Design decisions and why

- Incremental parsing buffers chunks up to `ParserOptions::max_document_bytes` and reuses the M2 parser core on `finish()` so one-shot and incremental behavior cannot drift.
- Namespace declarations are scoped with the element stack and are not emitted as ordinary attributes.
- Attribute namespace defaulting follows XML Namespaces rules: default namespaces apply to elements, not unprefixed attributes.
- `SaxCallbacks` is a thin adapter over `SaxHandler` so virtual and std::function callback paths share the same event stream.

## Mistakes made

- The first prefixed namespace test used brittle event indices and had to be reshaped to collect matching start-element events.
- M3 still uses whole-document buffering for incremental parse, so the word "streaming" needs careful qualification until a true streaming tokenizer exists.

## Root causes

- SAX event streams include both start and end events, so index-based assertions are fragile once nesting grows.
- The M2 parser was designed as a complete-document parser; M3 intentionally reused it for correctness consistency, accepting bounded buffering as the tradeoff.

## What was harder than expected

- Attribute `std::string_view` lifetimes needed careful storage while filtering out namespace declarations.
- End-element callbacks need the start tag's resolved `QualifiedName`, not a re-resolution of the raw end-tag name after scopes change.

## Naming conventions established

- Namespace tests live in `tests/req/std_namespaces_tests.cpp`.
- Incremental SAX tests live in `tests/req/sax_incremental_tests.cpp`.
- SAX event coverage tests use `REQ_SAX_02_*` names.
- Callback registration tests use `REQ_SAX_03_*` names.
- M3 verification report path is `docs/slo/verify/xmlparser-m3.md`.

## Test patterns that worked well

- Recording URI/local/qname on `RecordedEvent` and `RecordedAttribute` made namespace assertions readable.
- Feeding one byte at a time catches both buffer logic and accidental one-shot-only assumptions.
- Callback exception tests should always include a fresh parser success check afterward.

## Missing tests that should exist now

- A future streaming-tokenizer milestone should prove events can be emitted before the entire document is buffered.
- More namespace edge cases should be added with W3C fixtures, especially reserved prefixes and non-ASCII names.
- Prefix undeclaration edge cases should be revisited when XML 1.1 support lands.

## Rules for the next milestone

- M4 DOM building must consume the same namespace-aware event semantics as SAX, not re-parse or re-resolve names independently.
- DOM attribute APIs must preserve the M3 distinction between qname, local name, and URI.
- Serializer work must not emit namespace declaration attributes as ordinary attributes by accident.
- Keep incremental SAX tests green while adding DOM behavior.
- Avoid event-index brittle tests when a helper can search by event type/name.

## Template improvements suggested

- Contracts should distinguish "incremental API" from "true streaming tokenizer" when a milestone uses bounded buffering.
- Namespace milestones should require at least one test that proves namespace declarations are not ordinary attributes.
