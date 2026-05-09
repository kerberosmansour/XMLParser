# Lessons Learned - xmlparser Milestone 4

## What changed

- Added parser-built DOM documents, document-owned node storage, mutation and traversal APIs, namespace-aware attribute access, and string/stream serialization.
- Updated README, architecture notes, traceability, verification, and M4 evidence for the DOM and serializer layer.

## Design decisions and why

- DOM construction consumes the M3 namespace-aware SAX event stream so one-shot DOM and SAX behavior share parser-core semantics.
- `Document` owns all created nodes with `std::unique_ptr`, while parent/child links are non-owning pointers; this keeps caller handles stable while centralizing lifetime.
- Mutations reject cycles, cross-document insertion, document-as-child insertion, and multiple document elements to protect traversal and serialization invariants.
- Namespace-aware attributes are unique by expanded name (`namespace_uri`, `local_name`) while qualified-name lookup remains available for simple callers.
- The serializer emits namespace declarations derived from node and attribute namespace URIs because namespace declaration attributes are intentionally filtered out of ordinary SAX/DOM attributes.

## Mistakes made

- The first M4 test set did not explicitly name the `max_dom_nodes` and expanded-attribute uniqueness checks even though the contract listed those controls.
- The initial `DomBuilder` implementation briefly used a helper-template shape that linked poorly once moved into ordinary virtual callback code.

## Root causes

- The BDD table covered broad DOM behavior, but the abuse/control rows had finer-grained expectations that needed one named test each.
- Template helpers added abstraction before the DOM builder actually needed it; direct node creation calls were simpler and clearer.

## What was harder than expected

- Move-only `Document` ownership needed parent and owner pointers rewritten after moves.
- Serializer namespace declaration output needed to cooperate with the M3 decision that `xmlns` declarations are not ordinary attributes.
- Stream failure behavior needed both pre-write and post-write checks to make partial output failures observable.

## Naming conventions established

- DOM implementation lives in `src/dom.cpp`.
- DOM model tests live in `tests/req/dom_model_tests.cpp`.
- DOM mutation tests live in `tests/req/dom_mutation_tests.cpp`.
- DOM traversal tests live in `tests/req/dom_traversal_tests.cpp`.
- DOM serialization tests live in `tests/req/dom_serialization_tests.cpp`.
- DOM attribute tests live in `tests/req/dom_attribute_access_tests.cpp`.
- M4 verification report path is `docs/slo/verify/xmlparser-m4.md`.

## Test patterns that worked well

- Public API tests that parse real XML and inspect node types proved DOM/SAX parity without duplicating parser internals.
- Mutation abuse tests assert both the typed exception and unchanged tree state.
- Serialization tests check concrete output for simple cases and substring presence for namespace declaration cases where attribute order may grow later.
- Contract-audit tests should be added whenever a resource bound or abuse ID is named but not directly visible in test names.

## Missing tests that should exist now

- Serializer option behavior for pretty printing and non-UTF-8 encodings should be specified before release.
- Additional serializer invalid-character, comment, PI, and CDATA edge cases should be added with the M5 conformance work.
- A DOM round-trip fixture set should be added when W3C/conformance fixture import lands.

## Rules for the next milestone

- M5 must keep DOM construction on the shared parser core; do not introduce a separate validation parser.
- Add one named test for every M5 abuse ID, resource bound, and compatibility promise before implementation.
- Keep M1-M4 DOM, SAX, install-tree, and diagnostics tests green while adding DTD/XML 1.1 behavior.
- Decide whether `SerializeOptions::pretty_print` and serializer encoding behavior are in M5 release scope or explicitly deferred.
- Treat namespace declaration handling as a DOM/serializer invariant: declarations are not ordinary attributes, but serialized output must still contain required namespace bindings.

## Template improvements suggested

- Runbook contracts should require a named test row for every abuse acceptance scenario and resource-bound control, not just every BDD row.
- Evidence logs should have a dedicated "supplemental coverage found during audit" row so late-added tests are documented without hiding the timing.
