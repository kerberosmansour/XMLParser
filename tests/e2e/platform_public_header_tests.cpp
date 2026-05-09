#include <catch2/catch_test_macros.hpp>

#include <xmlparser/xmlparser.h>

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>

namespace {

namespace fs = std::filesystem;

std::string path_for_cmake(const fs::path& path) {
  std::string value = path.string();
  for (char& ch : value) {
    if (ch == '\\') {
      ch = '/';
    }
  }
  return value;
}

std::string quote(const fs::path& path) {
  const std::string value = path.string();
#ifdef _WIN32
  std::string quoted = "\"";
  for (char ch : value) {
    quoted += ch == '"' ? "\\\"" : std::string(1, ch);
  }
  quoted += "\"";
  return quoted;
#else
  std::string quoted = "'";
  for (char ch : value) {
    quoted += ch == '\'' ? "'\\''" : std::string(1, ch);
  }
  quoted += "'";
  return quoted;
#endif
}

std::string quote_command(const fs::path& path) {
#ifdef _WIN32
  // run_command closes this cmd.exe wrapper after all arguments have been appended.
  return "\"" + quote(path);
#else
  return quote(path);
#endif
}

std::string shell_command(const std::string& command) {
#ifdef _WIN32
  if (command.rfind("\"\"", 0) == 0) {
    return command + "\"";
  }
#endif
  return command;
}

void write_file(const fs::path& path, const std::string& contents) {
  fs::create_directories(path.parent_path());
  std::ofstream output(path);
  REQUIRE(output.is_open());
  output << contents;
}

fs::path unique_temp_dir(const std::string& name) {
  const auto tick = std::chrono::steady_clock::now().time_since_epoch().count();
  fs::path root = fs::temp_directory_path() / (name + "-" + std::to_string(tick));
  fs::create_directories(root);
  return root;
}

void run_command(const std::string& command) {
  const std::string effective_command = shell_command(command);
  INFO("command: " << effective_command);
  REQUIRE(std::system(effective_command.c_str()) == 0);
}

}  // namespace

TEST_CASE("REQ_PLAT_02_public_headers_compile_as_cxx17",
          "[e2e][req][m5][REQ-PLAT-02][REQ-API-01][REQ-API-03]") {
  const fs::path root = unique_temp_dir("xmlparser-m5-public-header");
  const fs::path source_dir = root / "source";
  const fs::path build_dir = root / "build";

  write_file(source_dir / "CMakeLists.txt",
             "cmake_minimum_required(VERSION 3.20)\n"
             "project(XmlParserM5Header LANGUAGES CXX)\n"
             "add_executable(header_compile main.cpp)\n"
             "target_compile_features(header_compile PRIVATE cxx_std_17)\n"
             "target_include_directories(header_compile PRIVATE \"" +
                 path_for_cmake(fs::path(XMLPARSER_SOURCE_DIR) / "include") + "\")\n");

  write_file(source_dir / "main.cpp",
             "#include <xmlparser/xmlparser.h>\n"
             "#include <string>\n"
             "#include <string_view>\n"
             "int main() {\n"
             "  xmlparser::v1::ParserOptions options;\n"
             "  options.version = xmlparser::v1::XmlVersion::Xml11;\n"
             "  options.external_dtd_resolver = [](std::string_view) {\n"
             "    return std::string{\"<!ELEMENT root EMPTY>\"};\n"
             "  };\n"
             "  return options.max_external_subset_bytes > 0 ? 0 : 1;\n"
             "}\n");

  run_command(quote_command(fs::path(CMAKE_COMMAND)) + " -S " + quote(source_dir) + " -B " +
              quote(build_dir));
  run_command(quote_command(fs::path(CMAKE_COMMAND)) + " --build " + quote(build_dir));
}

TEST_CASE("REQ_PLAT_06_public_api_uses_fixed_width_or_standard_size_types_intentionally",
          "[e2e][req][m5][REQ-PLAT-06][REQ-API-02]") {
  REQUIRE(sizeof(xmlparser::v1::SourceLocation::line) == sizeof(std::size_t));
  REQUIRE(sizeof(xmlparser::v1::SourceLocation::column) == sizeof(std::size_t));
  REQUIRE(sizeof(xmlparser::v1::SourceLocation::byte_offset) == sizeof(std::size_t));
}
