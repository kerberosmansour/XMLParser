cmake_minimum_required(VERSION 3.20)

project(
  XmlParser
  VERSION 0.1.0
  DESCRIPTION "C++17 XML parsing library"
  LANGUAGES CXX)

include(GNUInstallDirs)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  set(XMLPARSER_DEFAULT_BUILD_TESTS ON)
else()
  set(XMLPARSER_DEFAULT_BUILD_TESTS OFF)
endif()

option(XMLPARSER_BUILD_TESTS "Build XMLParser tests"
       ${XMLPARSER_DEFAULT_BUILD_TESTS})

add_library(xmlparser src/xmlparser.cpp src/parser_core.cpp src/dom.cpp)
add_library(xmlparser::xmlparser ALIAS xmlparser)

set_target_properties(
  xmlparser
  PROPERTIES EXPORT_NAME xmlparser
             VERSION ${PROJECT_VERSION}
             SOVERSION ${PROJECT_VERSION_MAJOR}
             WINDOWS_EXPORT_ALL_SYMBOLS ON)

target_compile_features(xmlparser PUBLIC cxx_std_17)
target_include_directories(
  xmlparser PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                   $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

install(
  TARGETS xmlparser
  EXPORT XmlParserTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

include(CMakePackageConfigHelpers)

set(XMLPARSER_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/XmlParser")

configure_package_config_file(
  cmake/XmlParserConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/XmlParserConfig.cmake"
  INSTALL_DESTINATION "${XMLPARSER_INSTALL_CMAKEDIR}")

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/XmlParserConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/XmlParserConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/XmlParserConfigVersion.cmake"
        DESTINATION "${XMLPARSER_INSTALL_CMAKEDIR}")

install(EXPORT XmlParserTargets
        NAMESPACE xmlparser::
        DESTINATION "${XMLPARSER_INSTALL_CMAKEDIR}")

add_custom_target(
  format
  COMMAND ${CMAKE_COMMAND} -E echo
          "No formatter is configured in M1 - formatting is manually reviewed.")

add_custom_target(
  lint
  COMMAND ${CMAKE_COMMAND} -E echo
          "No linter is configured in M1 - static analysis setup is deferred.")

if(XMLPARSER_BUILD_TESTS)
  include(CTest)
  include(FetchContent)

  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.4
    GIT_SHALLOW TRUE)
  FetchContent_MakeAvailable(Catch2)

  add_executable(
    xmlparser_tests
    tests/bdd_m1_public_api.cpp
    tests/e2e_m1_cmake_consumer.cpp
    tests/e2e/platform_cmake_consumer_tests.cpp
    tests/e2e/platform_public_header_tests.cpp
    tests/e2e/platform_release_evidence_tests.cpp
    tests/req/error_exception_safety_tests.cpp
    tests/req/error_dtd_validation_tests.cpp
    tests/req/error_location_exception_tests.cpp
    tests/req/error_malformed_input_tests.cpp
    tests/req/error_recoverable_callback_tests.cpp
    tests/req/error_validation_distinction_tests.cpp
    tests/req/dom_attribute_access_tests.cpp
    tests/req/dom_model_tests.cpp
    tests/req/dom_mutation_tests.cpp
    tests/req/dom_serialization_tests.cpp
    tests/req/dom_traversal_tests.cpp
    tests/req/sax_callback_registration_tests.cpp
    tests/req/sax_event_coverage_tests.cpp
    tests/req/sax_incremental_tests.cpp
    tests/req/sax_namespace_event_tests.cpp
    tests/req/sax_streaming_tests.cpp
    tests/req/std_encoding_tests.cpp
    tests/req/std_xml11_tests.cpp
    tests/req/std_markup_constructs_tests.cpp
    tests/req/std_namespaces_tests.cpp
    tests/req/std_xml10_conformance_tests.cpp)
  target_link_libraries(xmlparser_tests PRIVATE xmlparser::xmlparser
                                                Catch2::Catch2WithMain)
  target_compile_features(xmlparser_tests PRIVATE cxx_std_17)
  target_compile_definitions(
    xmlparser_tests
    PRIVATE XMLPARSER_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
            XMLPARSER_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}"
            CMAKE_COMMAND="${CMAKE_COMMAND}"
            CMAKE_EXECUTABLE_SUFFIX="${CMAKE_EXECUTABLE_SUFFIX}")

  add_test(NAME xmlparser_bdd COMMAND xmlparser_tests "[bdd]")
  set_tests_properties(xmlparser_bdd PROPERTIES LABELS "bdd")

  add_test(NAME xmlparser_req COMMAND xmlparser_tests "[req]")
  set_tests_properties(xmlparser_req PROPERTIES LABELS "req;bdd")

  add_test(NAME xmlparser_req_conformance COMMAND xmlparser_tests "[conformance]")
  set_tests_properties(xmlparser_req_conformance PROPERTIES LABELS "req;conformance")

  add_test(NAME xmlparser_e2e COMMAND xmlparser_tests "[e2e]")
  set_tests_properties(xmlparser_e2e PROPERTIES LABELS "e2e")
endif()
