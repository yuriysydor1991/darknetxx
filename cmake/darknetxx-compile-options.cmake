cmake_minimum_required(VERSION 3.13)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/compile-options")

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  include(darknetxx-compile-options-MSVC)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  include(darknetxx-compile-options-Clang)
else()
  include(darknetxx-compile-options-GCC)
endif()

set(
  EXTRA_COMPILE_OPTIONS 
  ${EXTRA_COMPILE_OPTIONS} 
  -DMAX_LOG_LEVEL=${MAX_LOG_LEVEL}
  -DDEFAULT_LOG_FILE_PATH="${DEFAULT_LOG_FILE_PATH}"
)

# The Darknet C core resets the directory compile options, while its CLOG*
# macros compile out the levels above the MAX_LOG_LEVEL the same way too.
set(
  EXTRA_C_COMPILE_OPTIONS
  ${EXTRA_C_COMPILE_OPTIONS}
  -DMAX_LOG_LEVEL=${MAX_LOG_LEVEL}
)

message(STATUS "COMPILER ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "COMPILE OPTIONS: ${EXTRA_COMPILE_OPTIONS}")
