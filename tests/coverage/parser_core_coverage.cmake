if(NOT DEFINED XMLPARSER_SOURCE_DIR)
  message(FATAL_ERROR "XMLPARSER_SOURCE_DIR is required")
endif()

set(coverage_build "${XMLPARSER_SOURCE_DIR}/build/m5-coverage")
set(profile_raw "${coverage_build}/parser-core.profraw")
set(profile_data "${coverage_build}/parser-core.profdata")

execute_process(
  COMMAND "${CMAKE_COMMAND}" -S "${XMLPARSER_SOURCE_DIR}" -B "${coverage_build}"
          -DXMLPARSER_BUILD_TESTS=ON
          "-DCMAKE_CXX_FLAGS=-fprofile-instr-generate -fcoverage-mapping"
          "-DCMAKE_EXE_LINKER_FLAGS=-fprofile-instr-generate -fcoverage-mapping"
  RESULT_VARIABLE configure_result)
if(NOT configure_result EQUAL 0)
  message(FATAL_ERROR "coverage configure failed")
endif()

execute_process(
  COMMAND "${CMAKE_COMMAND}" --build "${coverage_build}" --target xmlparser_tests
  RESULT_VARIABLE build_result)
if(NOT build_result EQUAL 0)
  message(FATAL_ERROR "coverage build failed")
endif()

set(test_exe "${coverage_build}/xmlparser_tests")
if(NOT EXISTS "${test_exe}")
  set(test_exe "${coverage_build}/Debug/xmlparser_tests")
endif()
if(NOT EXISTS "${test_exe}")
  message(FATAL_ERROR "coverage test executable not found")
endif()

execute_process(
  COMMAND "${CMAKE_COMMAND}" -E env "LLVM_PROFILE_FILE=${profile_raw}"
          "${test_exe}" "[req]~[e2e]"
  RESULT_VARIABLE test_result)
if(NOT test_result EQUAL 0)
  message(FATAL_ERROR "coverage test run failed")
endif()

execute_process(
  COMMAND xcrun llvm-profdata merge -sparse "${profile_raw}" -o "${profile_data}"
  RESULT_VARIABLE profdata_result)
if(NOT profdata_result EQUAL 0)
  message(FATAL_ERROR "llvm-profdata merge failed")
endif()

execute_process(
  COMMAND xcrun llvm-cov export "${test_exe}"
          "-instr-profile=${profile_data}"
          "${XMLPARSER_SOURCE_DIR}/src/parser_core.cpp"
          -summary-only
  RESULT_VARIABLE export_result
  OUTPUT_VARIABLE coverage_json)
if(NOT export_result EQUAL 0)
  message(FATAL_ERROR "llvm-cov export failed")
endif()

string(REGEX MATCH "\"lines\"[ \n\r\t]*:[ \n\r\t]*\\{[^}]*\"percent\"[ \n\r\t]*:[ \n\r\t]*([0-9]+\\.?[0-9]*)" _match "${coverage_json}")
if(NOT CMAKE_MATCH_1)
  message(FATAL_ERROR "could not parse parser-core line coverage")
endif()

set(line_coverage "${CMAKE_MATCH_1}")
if(line_coverage LESS 90)
  message(FATAL_ERROR "parser-core line coverage ${line_coverage}% is below 90%")
endif()

message(STATUS "parser-core line coverage ${line_coverage}%")
