#pragma once

#include <string>
#include <string_view>
#include <vector>

#include <xmlparser/options.h>
#include <xmlparser/source_location.h>

namespace xmlparser::v1 {

struct QualifiedName {
  std::string uri;
  std::string local_name;
  std::string qualified_name;
};

struct AttributeView {
  QualifiedName name;
  std::string_view value;
  SourceLocation location;
};

class SaxHandler {
 public:
  virtual ~SaxHandler() = default;

  virtual void start_document() {}
  virtual void end_document() {}
  virtual void start_element(const QualifiedName&,
                             const std::vector<AttributeView>&) {}
  virtual void end_element(const QualifiedName&) {}
  virtual void characters(std::string_view) {}
  virtual void processing_instruction(std::string_view, std::string_view) {}
  virtual void comment(std::string_view) {}
  virtual void cdata(std::string_view) {}
};

class SaxParser {
 public:
  explicit SaxParser(SaxHandler& handler, ParserOptions options = ParserOptions{});

  void parse(std::string_view xml);
  void feed(std::string_view chunk);
  void finish();

  const ParserOptions& options() const noexcept;

 private:
  SaxHandler* handler_;
  ParserOptions options_;
};

}  // namespace xmlparser::v1
