#pragma once

#include <stdexcept>
#include <string>

#include <xmlparser/source_location.h>

namespace xmlparser::v1 {

enum class ErrorKind {
  Unsupported,
  EmptyInput,
  WellFormedness,
  Validity,
  Encoding,
  ResourceLimit,
  Io,
};

class XmlParseException : public std::runtime_error {
 public:
  XmlParseException(ErrorKind kind, SourceLocation location, std::string message);

  ErrorKind kind() const noexcept;
  SourceLocation location() const noexcept;

 private:
  ErrorKind kind_;
  SourceLocation location_;
};

class XmlValidityException : public std::runtime_error {
 public:
  XmlValidityException(SourceLocation location, std::string message);

  ErrorKind kind() const noexcept;
  SourceLocation location() const noexcept;

 private:
  SourceLocation location_;
};

}  // namespace xmlparser::v1
