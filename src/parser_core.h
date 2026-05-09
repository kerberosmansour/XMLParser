#pragma once

#include <string_view>

#include <xmlparser/xmlparser.h>

namespace xmlparser::v1::detail {

void parse_xml_document(std::string_view xml,
                        SaxHandler& handler,
                        const ParserOptions& options);

}  // namespace xmlparser::v1::detail
