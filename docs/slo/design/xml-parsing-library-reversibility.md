# Reversibility Matrix - xml-parsing-library

| Decision | Why Hard To Change | Reversibility Tactic | Rollback / Migration Path | Proof Required |
|---|---|---|---|---|
| Public namespace `xmlparser::v1` | Consumers compile against names and ABI/API shape. | Keep versioned namespace and add future `v2` instead of breaking `v1`. | Deprecate old APIs, provide adapter headers, retain aggregate include. | Compile compatibility tests using old examples. |
| Shared parser core for DOM and SAX | Splitting later could create divergent XML behavior. | Keep tokenizer/event layer internal and build DOM from SAX-like events. | If split is unavoidable, create conformance suite that must pass identically for both paths first. | Shared fixture tests proving DOM and SAX observe same structure. |
| No mandatory runtime dependencies | Adding dependencies affects packaging and platform guarantees. | Keep extension points for optional resolver/features. | Move dependency behind CMake option and public feature macro. | `find_package` consumer test with default options links without extra libs. |
| DOM ownership model | Node references and mutation semantics shape all callers. | Prefer owning `Document` with stable handles/views and no raw owning pointers. | Add new handle type in `v2`; keep `v1` adapter. | Mutation, traversal, and exception-safety tests. |
| Error model with exceptions | Callers structure error handling around exception types. | Keep typed exceptions and optionally add non-throwing result adapters later. | Add overloads rather than changing existing signatures. | API compile tests and source-location assertions. |
| External DTD disabled by default | Enabling by default changes trust boundary and downstream risk. | Keep resolver opt-in and document risk. | Disable feature flag and retain internal DTD validation. | Tests prove external resolver is not called unless explicitly configured. |
| CMake package target name | Consumers write `target_link_libraries(... xmlparser::xmlparser)`. | Lock target naming now. | Add alias target if name changes. | Install-tree consumer project in CI. |
