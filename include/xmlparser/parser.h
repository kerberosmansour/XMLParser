#pragma once

#include <string_view>

#include <xmlparser/dom.h>
#include <xmlparser/options.h>
#include <xmlparser/sax.h>

namespace xmlparser::v1 {

Document parse(std::string_view xml, const ParserOptions& options = ParserOptions{});

void parse(std::string_view xml,
           SaxHandler& handler,
           const ParserOptions& options = ParserOptions{});

void parse(std::string_view xml,
           SaxCallbacks& callbacks,
           const ParserOptions& options = ParserOptions{});

}  // namespace xmlparser::v1
