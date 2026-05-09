if(NOT DEFINED XMLPARSER_SOURCE_DIR)
  message(FATAL_ERROR "XMLPARSER_SOURCE_DIR is required")
endif()

file(READ "${XMLPARSER_SOURCE_DIR}/CMakeLists.txt" cmake_lists)

if(cmake_lists MATCHES "target_link_libraries[ \t\r\n]*\\([ \t\r\n]*xmlparser[ \t\r\n]")
  message(FATAL_ERROR "xmlparser production target links a non-standard runtime dependency")
endif()

message(STATUS "xmlparser production target has no mandatory runtime dependency")
