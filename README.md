# XMLParser

XMLParser is a C++17 XML parsing library under active SLO-driven
development. The current milestone provides the build, packaging, test
harness, and public API frame. Full XML tokenization, SAX, DOM, namespace, and
DTD behavior lands in later milestones.

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
    return error.kind() == xmlparser::v1::ErrorKind::Unsupported ? 0 : 1;
  }

  return 1;
}
```

In M1, parse and serialize entry points throw typed unsupported errors instead
of accepting XML. Empty input is a separate typed error path.

## Debugging Tests

A failing Catch2 scenario can be launched under a debugger after building:

```sh
lldb -- build/xmlparser_tests "[bdd]"
```

Use `gdb --args build/xmlparser_tests "[bdd]"` on systems where `gdb` is the
available debugger.
