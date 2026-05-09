#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
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
    if (ch == '"') {
      quoted += "\\\"";
    } else {
      quoted += ch;
    }
  }
  quoted += "\"";
  return quoted;
#else
  std::string quoted = "'";
  for (char ch : value) {
    if (ch == '\'') {
      quoted += "'\\''";
    } else {
      quoted += ch;
    }
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

  const fs::path release_config =
      build_dir / "Release" / (name + std::string(CMAKE_EXECUTABLE_SUFFIX));
  if (fs::exists(release_config)) {
    return release_config;
  }

  const fs::path debug_config =
      build_dir / "Debug" / (name + std::string(CMAKE_EXECUTABLE_SUFFIX));
  if (fs::exists(debug_config)) {
    return debug_config;
  }

  return single_config;
}

void stage_runtime_dlls(const fs::path& install_prefix, const fs::path& executable) {
#ifdef _WIN32
  const fs::path bin_dir = install_prefix / "bin";
  if (!fs::exists(bin_dir)) {
    return;
  }
  for (const auto& entry : fs::directory_iterator(bin_dir)) {
    if (entry.path().extension() == ".dll") {
      fs::copy_file(entry.path(), executable.parent_path() / entry.path().filename(),
                    fs::copy_options::overwrite_existing);
    }
  }
#else
  (void)install_prefix;
  (void)executable;
#endif
}

}  // namespace

SCENARIO("M1 public header compiles as CXX17", "[e2e][m1][REQ-API-01]") {
  GIVEN("a separate CMake project includes the public header") {
    const fs::path root = unique_temp_dir("xmlparser-header-compile");
    const fs::path source_dir = root / "source";
    const fs::path build_dir = root / "build";

    write_file(source_dir / "CMakeLists.txt",
               "cmake_minimum_required(VERSION 3.20)\n"
               "project(XmlParserHeaderCompile LANGUAGES CXX)\n"
               "add_executable(header_compile main.cpp)\n"
               "target_compile_features(header_compile PRIVATE cxx_std_17)\n"
               "target_include_directories(header_compile PRIVATE \"" +
                   path_for_cmake(fs::path(XMLPARSER_SOURCE_DIR) / "include") + "\")\n");

    write_file(source_dir / "main.cpp",
               "#include <xmlparser/xmlparser.h>\n"
               "int main() {\n"
               "  xmlparser::v1::ParserOptions options;\n"
               "  options.version = xmlparser::v1::XmlVersion::Xml10;\n"
               "  return options.max_depth > 0 ? 0 : 1;\n"
               "}\n");

    WHEN("the project configures and builds with C++17") {
      run_command(quote_command(fs::path(CMAKE_COMMAND)) + " -S " + quote(source_dir) + " -B " +
                  quote(build_dir));
      run_command(quote_command(fs::path(CMAKE_COMMAND)) + " --build " + quote(build_dir));

      THEN("the translation unit compiles") {
        REQUIRE(fs::exists(built_executable(build_dir, "header_compile")));
      }
    }
  }
}

SCENARIO("M1 install tree consumer can find package", "[e2e][m1][REQ-PLAT-03]") {
  GIVEN("XMLParser is installed to a temporary prefix") {
    const fs::path root = unique_temp_dir("xmlparser-install-consumer");
    const fs::path install_prefix = root / "install";
    const fs::path consumer_source = root / "consumer";
    const fs::path consumer_build = root / "consumer-build";

    WHEN("a separate CMake project calls find_package(XmlParser CONFIG REQUIRED)") {
      run_command(quote_command(fs::path(CMAKE_COMMAND)) + " --install " + quote(XMLPARSER_BINARY_DIR) +
                  " --prefix " + quote(install_prefix));

      write_file(consumer_source / "CMakeLists.txt",
                 "cmake_minimum_required(VERSION 3.20)\n"
                 "project(XmlParserConsumer LANGUAGES CXX)\n"
                 "find_package(XmlParser CONFIG REQUIRED)\n"
                 "add_executable(consumer main.cpp)\n"
                 "target_link_libraries(consumer PRIVATE xmlparser::xmlparser)\n");

      write_file(consumer_source / "main.cpp",
                 "#include <xmlparser/xmlparser.h>\n"
                 "#include <exception>\n"
                 "int main() {\n"
                 "  xmlparser::v1::ParserOptions options;\n"
                 "  if (options.max_document_bytes == 0) return 1;\n"
                 "  try {\n"
                 "    (void)xmlparser::v1::parse(\"<root/>\");\n"
                 "  } catch (const xmlparser::v1::XmlParseException& error) {\n"
                 "    return 2;\n"
                 "  }\n"
                 "  return 0;\n"
                 "}\n");

      run_command(quote_command(fs::path(CMAKE_COMMAND)) + " -S " + quote(consumer_source) + " -B " +
                  quote(consumer_build) + " -DCMAKE_PREFIX_PATH=" + quote(install_prefix));
      run_command(quote_command(fs::path(CMAKE_COMMAND)) + " --build " + quote(consumer_build));

      THEN("it links xmlparser::xmlparser and runs a minimal program") {
        const fs::path executable = built_executable(consumer_build, "consumer");
        REQUIRE(fs::exists(executable));
        stage_runtime_dlls(install_prefix, executable);
        run_command(quote(executable));
      }
    }
  }
}
