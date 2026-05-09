# XMLParser

XMLParser is a C++17 XML parsing library under active SLO-driven
development. The current milestone provides XML 1.0 well-formedness parsing,
UTF-8/UTF-16 BOM handling, source-location diagnostics, namespace-aware SAX
events, and an incremental `feed` / `finish` API. DOM ownership,
serialization, XML 1.1, and DTD validation land in later milestones.

## Build

```sh
cmake -S . -B build -DXMLPARSER_BUILD_TESTS=ON
cmake --build build
```

`BUILD_SHARED_LIBS` controls whether CMake builds a static or shared
`xmlparser` library. The public imported target is `xmlparser::xmlparser`.

## Test

```sh
ctest --test-dir build --output-on-failure
ctest --test-dir build --output-on-failure -L bdd
ctest --test-dir build --output-on-failure -L e2e
```

Tests use Catch2 as a test-only dependency, pinned in `CMakeLists.txt`.
XMLParser has no runtime dependency beyond the C++17 standard library.

Formatter and lint targets exist as M1 placeholders:

```sh
cmake --build build --target format
cmake --build build --target lint
```

## Install And Consume

```sh
cmake --install build --prefix /tmp/xmlparser-install
```

A consuming CMake project can use:

```cmake
find_package(XmlParser CONFIG REQUIRED)
target_link_libraries(app PRIVATE xmlparser::xmlparser)
```

## Public API Sketch

```cpp
#include <xmlparser/xmlparser.h>

int main() {
  xmlparser::v1::ParserOptions options;
  options.version = xmlparser::v1::XmlVersion::Xml10;

  try {
    (void)xmlparser::v1::parse("<root/>", options);
  } catch (const xmlparser::v1::XmlParseException& error) {
    return 1;
  }

  return 0;
}
```

The one-shot parser accepts well-formed XML 1.0 and reports malformed input
through `XmlParseException` with `ErrorKind`, line, column, and byte offset.
Error messages omit raw XML payloads by default.

## SAX-Style Events

```cpp
#include <xmlparser/xmlparser.h>

#include <string_view>
#include <vector>

class Handler : public xmlparser::v1::SaxHandler {
 public:
  void start_element(
      const xmlparser::v1::QualifiedName& name,
      const std::vector<xmlparser::v1::AttributeView>& attributes) override {
    (void)name;
    (void)attributes;
  }

  void characters(std::string_view text) override {
    (void)text;
  }
};

int main() {
  Handler handler;
  xmlparser::v1::parse("<root>Hello &amp; goodbye</root>", handler);
}
```

## Incremental SAX

```cpp
#include <xmlparser/xmlparser.h>

class Handler : public xmlparser::v1::SaxHandler {};

int main() {
  Handler handler;
  xmlparser::v1::SaxParser parser(handler);

  parser.feed("<root xmlns=\"urn:example\">");
  parser.feed("<child/>");
  parser.feed("</root>");
  parser.finish();
}
```

Incremental input is buffered up to `ParserOptions::max_document_bytes` and
then parsed by the shared parser core on `finish()`.

## Selective Callbacks

```cpp
#include <xmlparser/xmlparser.h>

#include <vector>

int main() {
  std::vector<std::string> element_names;
  xmlparser::v1::SaxCallbacks callbacks;

  callbacks.on_start_element =
      [&](const xmlparser::v1::QualifiedName& name,
          const std::vector<xmlparser::v1::AttributeView>&) {
        element_names.push_back(name.qualified_name);
      };

  xmlparser::v1::parse("<root><child/></root>", callbacks);
}
```

When namespace processing is enabled, element and attribute events include URI,
local name, and qualified name.

## Debugging Tests

A failing Catch2 scenario can be launched under a debugger after building:

```sh
lldb -- build/xmlparser_tests "[bdd]"
```

Use `gdb --args build/xmlparser_tests "[bdd]"` on systems where `gdb` is the
available debugger.
