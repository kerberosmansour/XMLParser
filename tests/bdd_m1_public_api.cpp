#include <catch2/catch_test_macros.hpp>

#include <xmlparser/xmlparser.h>

#include <cstddef>
#include <fstream>
#include <iterator>
#include <limits>
#include <string>
#include <string_view>

namespace {

bool contains(std::string_view haystack, std::string_view needle) {
  return haystack.find(needle) != std::string_view::npos;
}

std::string read_file(const std::string& path) {
  std::ifstream input(path);
  REQUIRE(input.is_open());
  return std::string(std::istreambuf_iterator<char>(input),
                     std::istreambuf_iterator<char>());
}

}  // namespace

SCENARIO("M1 API include compiles", "[bdd][m1][REQ-API-01]") {
  GIVEN("a C++17 translation unit includes <xmlparser/xmlparser.h>") {
    WHEN("it references xmlparser::v1::ParserOptions and XmlVersion") {
      xmlparser::v1::ParserOptions options;
      options.version = xmlparser::v1::XmlVersion::Xml10;

      THEN("compilation succeeds without non-standard extensions") {
        REQUIRE(options.version == xmlparser::v1::XmlVersion::Xml10);
      }
    }
  }
}

SCENARIO("M1 unsupported parser is visible", "[bdd][m1][REQ-ERR-01]") {
  GIVEN("the skeleton parse API receives <root/> before M2") {
    const std::string xml = "<root/>";

    WHEN("the caller invokes parse") {
      THEN("it throws a typed exception indicating unsupported implementation") {
        try {
          (void)xmlparser::v1::parse(xml);
          FAIL("skeleton parser unexpectedly accepted XML before M2");
        } catch (const xmlparser::v1::XmlParseException& error) {
          REQUIRE(error.kind() == xmlparser::v1::ErrorKind::Unsupported);
          REQUIRE(contains(error.what(), "not implemented"));
        }
      }
    }
  }
}

SCENARIO("M1 empty input is defined", "[bdd][m1][REQ-ERR-02]") {
  GIVEN("the skeleton parse API receives empty input") {
    WHEN("parse is invoked") {
      THEN("it returns a typed error path, not a crash") {
        try {
          (void)xmlparser::v1::parse("");
          FAIL("skeleton parser unexpectedly accepted empty XML");
        } catch (const xmlparser::v1::XmlParseException& error) {
          REQUIRE(error.kind() == xmlparser::v1::ErrorKind::EmptyInput);
          REQUIRE(error.location().line == 1U);
          REQUIRE(error.location().column == 1U);
          REQUIRE(error.location().byte_offset == 0U);
        }
      }
    }
  }
}

SCENARIO("M1 options are bounded", "[bdd][m1][REQ-API-02][REQ-SEC-01]") {
  GIVEN("default ParserOptions is constructed") {
    const xmlparser::v1::ParserOptions options;

    WHEN("tests inspect limit fields") {
      THEN("all parser-controlled bounds are positive and finite") {
        REQUIRE(options.max_document_bytes > 0U);
        REQUIRE(options.max_depth > 0U);
        REQUIRE(options.max_token_bytes > 0U);
        REQUIRE(options.max_attributes_per_element > 0U);
        REQUIRE(options.max_entity_expansions > 0U);
        REQUIRE(options.max_dom_nodes > 0U);

        REQUIRE(options.max_document_bytes < std::numeric_limits<std::size_t>::max());
        REQUIRE(options.max_depth < std::numeric_limits<std::size_t>::max());
        REQUIRE(options.max_token_bytes < std::numeric_limits<std::size_t>::max());
        REQUIRE(options.max_attributes_per_element <
                std::numeric_limits<std::size_t>::max());
        REQUIRE(options.max_entity_expansions < std::numeric_limits<std::size_t>::max());
        REQUIRE(options.max_dom_nodes < std::numeric_limits<std::size_t>::max());
      }
    }
  }
}

SCENARIO("M1 raw XML is not logged", "[bdd][m1][REQ-ERR-03][REQ-SEC-02]") {
  GIVEN("input contains secret-looking XML text") {
    const std::string xml =
        "<root><password>hunter2</password><token>secret-token</token></root>";

    WHEN("parse fails in skeleton mode") {
      THEN("the error message omits the raw XML payload") {
        try {
          (void)xmlparser::v1::parse(xml);
          FAIL("skeleton parser unexpectedly accepted XML before M2");
        } catch (const xmlparser::v1::XmlParseException& error) {
          const std::string message = error.what();
          REQUIRE_FALSE(contains(message, xml));
          REQUIRE_FALSE(contains(message, "hunter2"));
          REQUIRE_FALSE(contains(message, "secret-token"));
        }
      }
    }
  }
}

SCENARIO("M1 fixture provenance exists", "[bdd][m1][REQ-STD-01][REQ-PLAT-01]") {
  GIVEN("conformance fixtures are not yet vendored") {
    WHEN("docs are generated") {
      const std::string docs =
          read_file(std::string(XMLPARSER_SOURCE_DIR) + "/docs/conformance-fixtures.md");

      THEN("docs list source URL, license check, and import gate") {
        REQUIRE(contains(docs, "https://www.w3.org/XML/Test/"));
        REQUIRE(contains(docs, "license"));
        REQUIRE(contains(docs, "import gate"));
      }
    }
  }
}
