# Conformance Fixture Provenance - XMLParser

M1 does not vendor external XML fixtures. This document is the import gate for
future milestones so source, license, and redistribution checks happen before
any fixture bytes enter the repository.

## Candidate Source

| Fixture Source | URL | Planned Use | M1 Status |
|---|---|---|---|
| W3C XML Conformance Test Suites | https://www.w3.org/XML/Test/ | XML 1.0, XML 1.1, and namespace conformance harness inputs | Documented only; not vendored |

The W3C page describes the XML Test Suite as the conformance baseline for XML
and namespace recommendations and notes that implementors are encouraged to
build a harness around the tests.

## License And Redistribution Check

Before import, complete all rows:

| Check | Required Evidence | Status |
|---|---|---|
| Fixture archive/version selected | Exact release URL and retrieval date | Pending |
| license terms reviewed | W3C test suite license terms at https://www.w3.org/copyright/test-suites-licenses/ | Pending |
| Attribution text captured | Required copyright/license notice copied into fixture manifest | Pending |
| Redistribution allowed | Written decision recorded with reviewer and date | Pending |
| No implementation code copied | Confirm fixtures only; no parser implementation source imported | Pending |

## Import Gate

Fixture import is blocked until a future milestone records:

1. Exact source URL and release identifier.
2. license and attribution requirements.
3. Hash of the downloaded fixture archive.
4. Local directory layout for fixture metadata and expected outcomes.
5. Requirement IDs covered by each fixture group.
6. Review note confirming no competitor parser implementation code was copied.

Until then, tests may assert this provenance plan exists, but must not depend on
vendored W3C fixture files.
