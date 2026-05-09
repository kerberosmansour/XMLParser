# Conformance Fixture Provenance - XMLParser

M1-M4 did not vendor external XML fixtures. M5 keeps the upstream W3C fixture
payload out of the repository and records a data-only manifest with curated
inline XML cases plus source/license provenance.

## Candidate Source

| Fixture Source | URL | Planned Use | M5 Status |
|---|---|---|---|
| W3C XML Conformance Test Suites | https://www.w3.org/XML/Test/ | XML 1.0, XML 1.1, and namespace conformance reference | Source reviewed on 2026-05-09; upstream payload not vendored |

The W3C XML Test Suite page describes the suite as a conformance metric for XML
and namespace recommendations, lists XML 1.0, XML 1.1, and namespace releases,
and points implementors toward building a harness around the tests.

## License And Redistribution Check

Before import, complete all rows:

| Check | Required Evidence | Status |
|---|---|---|
| Fixture archive/version selected | Exact release URL and retrieval date | M5 uses metadata-only curated manifest at `tests/fixtures/w3c/xmlconf/manifest.xml`; no archive imported |
| license terms reviewed | W3C test suite license terms at https://www.w3.org/copyright/test-suites-licenses/ | Reviewed on 2026-05-09; W3C offers BSD-style and W3C test-suite license paths depending on use |
| Attribution text captured | Required copyright/license notice copied into fixture manifest | Manifest records W3C source and license URLs; no upstream payload copied |
| Redistribution allowed | Written decision recorded with reviewer and date | Approved for metadata-only curated manifest; do not redistribute W3C payload or claim W3C suite performance |
| No implementation code copied | Confirm fixtures only; no parser implementation source imported | Confirmed: no third-party parser implementation code or W3C harness code copied |

## Import Gate

Full upstream fixture import remains blocked until a future milestone records:

1. Exact source URL and release identifier.
2. license and attribution requirements.
3. Hash of the downloaded fixture archive.
4. Local directory layout for fixture metadata and expected outcomes.
5. Requirement IDs covered by each fixture group.
6. Review note confirming no competitor parser implementation code was copied.

Until then, tests may assert this provenance plan exists, but must not depend on
vendored W3C fixture files.

## M5 Conformance Evidence

| Evidence | Path | Result |
|---|---|---|
| XML 1.1 curated manifest exists | `tests/fixtures/w3c/xmlconf/manifest.xml` | `REQ_STD_02_runs_xml11_conformance_subset` passes |
| Data-only fixture rule | `tests/fixtures/w3c/xmlconf/README.md` | Fixture entries are parsed as XML data; scripts and generated code are not executed |
| Upstream payload status | N/A | Not vendored in M5 |

## Approved Scope Decision

M5 may use a curated manifest with inline XML strings to prove the conformance
harness path and XML 1.1 behavior. This is not a claim of passing the complete
W3C XML Test Suite. A complete upstream import remains a future milestone that
must perform archive hash capture, license review, and exception reporting.
