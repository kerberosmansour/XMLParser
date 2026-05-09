#include <catch2/catch_test_macros.hpp>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace {

namespace fs = std::filesystem;

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

std::string read_file(const fs::path& path) {
  std::ifstream input(path);
  REQUIRE(input.is_open());
  std::ostringstream buffer;
  buffer << input.rdbuf();
  return buffer.str();
}

void run_command(const std::string& command) {
  const std::string effective_command = shell_command(command);
  INFO("command: " << effective_command);
  REQUIRE(std::system(effective_command.c_str()) == 0);
}

}  // namespace

TEST_CASE("REQ_PLAT_04_default_link_has_no_non_std_runtime_dependency",
          "[e2e][req][m5][REQ-PLAT-04]") {
  run_command(quote_command(fs::path(CMAKE_COMMAND)) +
              " -DXMLPARSER_SOURCE_DIR=" + quote(XMLPARSER_SOURCE_DIR) +
              " -P " +
              quote(fs::path(XMLPARSER_SOURCE_DIR) /
                    "tests/e2e/platform_dependency_audit.cmake"));
}

TEST_CASE("REQ_PLAT_01_ci_matrix_linux_gcc11",
          "[e2e][req][m5][REQ-PLAT-01]") {
  const std::string workflow =
      read_file(fs::path(XMLPARSER_SOURCE_DIR) / ".github/workflows/ci.yml");
  REQUIRE(workflow.find("ubuntu-latest") != std::string::npos);
  REQUIRE(workflow.find("gcc") != std::string::npos);
}

TEST_CASE("REQ_PLAT_01_ci_matrix_linux_clang14",
          "[e2e][req][m5][REQ-PLAT-01]") {
  const std::string workflow =
      read_file(fs::path(XMLPARSER_SOURCE_DIR) / ".github/workflows/ci.yml");
  REQUIRE(workflow.find("clang") != std::string::npos);
}

TEST_CASE("REQ_PLAT_01_ci_matrix_macos_apple_clang14",
          "[e2e][req][m5][REQ-PLAT-01]") {
  const std::string workflow =
      read_file(fs::path(XMLPARSER_SOURCE_DIR) / ".github/workflows/ci.yml");
  REQUIRE(workflow.find("macos-latest") != std::string::npos);
}

TEST_CASE("REQ_PLAT_01_ci_matrix_windows_msvc_v142",
          "[e2e][req][m5][REQ-PLAT-01]") {
  const std::string workflow =
      read_file(fs::path(XMLPARSER_SOURCE_DIR) / ".github/workflows/ci.yml");
  REQUIRE(workflow.find("windows-latest") != std::string::npos);
  REQUIRE(workflow.find("msvc") != std::string::npos);
}

TEST_CASE("REQ_API_04_parser_core_line_coverage_is_at_least_90_percent",
          "[coverage][m5][REQ-API-04]") {
  run_command(quote_command(fs::path(CMAKE_COMMAND)) +
              " -DXMLPARSER_SOURCE_DIR=" + quote(XMLPARSER_SOURCE_DIR) +
              " -DXMLPARSER_BINARY_DIR=" + quote(XMLPARSER_BINARY_DIR) +
              " -P " +
              quote(fs::path(XMLPARSER_SOURCE_DIR) /
                    "tests/coverage/parser_core_coverage.cmake"));
}
