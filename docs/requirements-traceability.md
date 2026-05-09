# Requirements Traceability - XMLParser

This matrix starts the executable coverage trail for the XMLParser runbook.
Requirement identifiers are intentionally stable labels for tests, docs, and
future conformance fixtures.

| Requirement | Coverage In M1 | Evidence |
|---|---|---|
| `REQ-API-01` aggregate public include compiles as C++17 | `tests/bdd_m1_public_api.cpp`, `tests/e2e_m1_cmake_consumer.cpp` | `M1 API include compiles`; `M1 public header compiles as CXX17` |
| `REQ-API-02` parser options expose bounded defaults | `tests/bdd_m1_public_api.cpp` | `M1 options are bounded` |
| `REQ-ERR-01` unsupported skeleton parsing is typed and visible | `tests/bdd_m1_public_api.cpp`, `tests/e2e_m1_cmake_consumer.cpp` | `M1 unsupported parser is visible`; install-tree consumer |
| `REQ-ERR-02` empty input is a typed error path | `tests/bdd_m1_public_api.cpp` | `M1 empty input is defined` |
| `REQ-ERR-03` errors omit raw XML payload by default | `tests/bdd_m1_public_api.cpp` | `M1 raw XML is not logged` |
| `REQ-PLAT-01` public header is portable C++17 | `tests/e2e_m1_cmake_consumer.cpp` | `M1 public header compiles as CXX17` |
| `REQ-PLAT-03` installed package can be consumed with CMake | `tests/e2e_m1_cmake_consumer.cpp` | `M1 install tree consumer can find package` |
| `REQ-SEC-01` resource defaults are finite and positive | `tests/bdd_m1_public_api.cpp` | `M1 options are bounded` |
| `REQ-SEC-02` secret-looking XML is not echoed in diagnostics | `tests/bdd_m1_public_api.cpp` | `M1 raw XML is not logged` |
| `REQ-STD-01` W3C fixture provenance is documented before import | `tests/bdd_m1_public_api.cpp`, `docs/conformance-fixtures.md` | `M1 fixture provenance exists` |

## Future Coverage Hooks

- `REQ-STD-*` expands in M2 and M5 when tokenizer, XML 1.0, XML 1.1, and
  conformance fixture ingestion are implemented.
- `REQ-SAX-*` expands in M3 when incremental SAX callbacks are implemented.
- `REQ-DOM-*` expands in M4 when DOM mutation, traversal, and serialization
  are implemented.
- `REQ-VALID-*` expands in M5 when DTD validation is implemented.
