#pragma once

#include <cstddef>

namespace xmlparser::v1 {

struct SourceLocation {
  std::size_t line = 1;
  std::size_t column = 1;
  std::size_t byte_offset = 0;
};

}  // namespace xmlparser::v1
