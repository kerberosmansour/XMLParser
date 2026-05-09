#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
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

fs::path built_executable(const fs::path& build_dir, const std::string& name) {
  const fs::path single_config = build_dir / (name + std::string(CMAKE_EXECUTABLE_SUFFIX));
  if (fs::exists(single_config)) {
    return single_config;
  }
  const fs::path debug_config =
      build_dir / "Debug" / (name + std::string(CMAKE_EXECUTABLE_SUFFIX));
  if (fs::exists(debug_config)) {
    return debug_config;
  }
  return single_config;
}

void write_consumer(const fs::path& source_dir) {
  write_file(source_dir / "CMakeLists.txt",
             "cmake_minimum_required(VERSION 3.20)\n"
             "project(XmlParserM5Consumer LANGUAGES CXX)\n"
             "find_package(XmlParser CONFIG REQUIRED)\n"
             "add_executable(consumer main.cpp)\n"
             "target_link_libraries(consumer PRIVATE xmlparser::xmlparser)\n");

  write_file(source_dir / "main.cpp",
             "#include <xmlparser/xmlparser.h>\n"
             "int main() {\n"
             "  xmlparser::v1::ParserOptions options;\n"
             "  options.validation = xmlparser::v1::ValidationMode::Dtd;\n"
             "  (void)xmlparser::v1::parse(\"<!DOCTYPE root [<!ELEMENT root EMPTY>]><root/>\", options);\n"
             "  return 0;\n"
             "}\n");
}

void build_and_install_xmlparser(const fs::path& root, bool shared) {
  const fs::path build_dir = root / (shared ? "xmlparser-shared" : "xmlparser-static");
  const fs::path install_prefix = root / (shared ? "install-shared" : "install-static");
  run_command(quote_command(fs::path(CMAKE_COMMAND)) + " -S " + quote(XMLPARSER_SOURCE_DIR) +
              " -B " + quote(build_dir) +
              " -DXMLPARSER_BUILD_TESTS=OFF -DBUILD_SHARED_LIBS=" +
              (shared ? "ON" : "OFF"));
  run_command(quote_command(fs::path(CMAKE_COMMAND)) + " --build " + quote(build_dir));
  run_command(quote_command(fs::path(CMAKE_COMMAND)) + " --install " + quote(build_dir) +
              " --prefix " + quote(install_prefix));
}

void build_consumer(const fs::path& root, bool shared, bool run_executable) {
  const fs::path install_prefix = root / (shared ? "install-shared" : "install-static");
  const fs::path consumer_source = root / (shared ? "consumer-shared" : "consumer-static");
  const fs::path consumer_build =
      root / (shared ? "consumer-shared-build" : "consumer-static-build");
  write_consumer(consumer_source);

  run_command(quote_command(fs::path(CMAKE_COMMAND)) + " -S " + quote(consumer_source) + " -B " +
              quote(consumer_build) + " -DCMAKE_PREFIX_PATH=" + quote(install_prefix));
  run_command(quote_command(fs::path(CMAKE_COMMAND)) + " --build " + quote(consumer_build));

  if (run_executable) {
    run_command(quote(built_executable(consumer_build, "consumer")));
  }
}

}  // namespace

TEST_CASE("REQ_PLAT_03_find_package_imported_target_builds_consumer",
          "[e2e][req][m5][REQ-PLAT-03]") {
  const fs::path root = unique_temp_dir("xmlparser-m5-install-consumer");
  build_and_install_xmlparser(root, false);
  build_consumer(root, false, true);
}

TEST_CASE("REQ_PLAT_05_builds_static_library",
          "[e2e][req][m5][REQ-PLAT-05]") {
  const fs::path root = unique_temp_dir("xmlparser-m5-static");
  build_and_install_xmlparser(root, false);
  build_consumer(root, false, true);
}

TEST_CASE("REQ_PLAT_05_builds_shared_library",
          "[e2e][req][m5][REQ-PLAT-05]") {
  const fs::path root = unique_temp_dir("xmlparser-m5-shared");
  build_and_install_xmlparser(root, true);
  build_consumer(root, true, false);
}
