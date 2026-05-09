#pragma once

#include <iosfwd>
#include <string>

#include <xmlparser/dom.h>
#include <xmlparser/options.h>

namespace xmlparser::v1 {

std::string serialize(const Document& document,
                      const SerializeOptions& options = SerializeOptions{});

void serialize(const Document& document,
               std::ostream& output,
               const SerializeOptions& options = SerializeOptions{});

}  // namespace xmlparser::v1
