cmake_minimum_required(VERSION 3.13)

# The integration test of the network weights files converter only binary: the
# ITEST_converter tests run the darknetxxConverter executable (and the
# conversion command line of the darknetxx one) the way a user does and check
# the files it produces. The generated networks of every variant of the
# weights factory convert from the original darknet weights file to the
# dxxwjz1 and the dxxwjz2 ones and back into the same bytes.
if(NOT TARGET ${DARKNETXX_CONVERTER_BINARY_NAME})
  message(STATUS
    "The ITEST_converter tests are skipped, the binary they drive is not "
    "being built")
  return()
endif()

set(ITEST_CONVERTER_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(
  ITEST_CONVERTER_REFERENCE_DIR
  "${PROJECT_SOURCE_DIR}/src/lib/darknet-adaptor/adaptors/NetworksLoaders/OrigDefaultLoader/tests/component/OrigDefaultLoader/data"
)

configure_file(
  ITEST_converter_provider.h.in
  ITEST_converter_provider.h
  @ONLY
)

add_executable(
  ITEST_converter
  ConverterRuns.cpp
  ITEST_converter.cpp
  ITEST_converter_variants.cpp
)

# the tests run the executables, so they are built along with them
add_dependencies(
  ITEST_converter
  ${DARKNETXX_CONVERTER_BINARY_NAME}
  ${DARKNETXX_BINARY_NAME}
)

target_compile_definitions(
  ITEST_converter
  PRIVATE
    ITEST_CONVERTER_BINARY=\"$<TARGET_FILE:${DARKNETXX_CONVERTER_BINARY_NAME}>\"
    ITEST_DARKNETXX_BINARY=\"$<TARGET_FILE:${DARKNETXX_BINARY_NAME}>\"
)

target_include_directories(
  ITEST_converter
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
  ITEST_converter
  DarknetXXNetworksCore
  DarknetXXWeightsFactory
  GTest::gtest_main GTest::gmock
)

include(GoogleTest)

# Every TEST_P test runs all the generated variants of its suite in one
# process (the */<suite>.<test>/* ctest names), a handful of the converter runs
# a variant, the biggest variants convert the hundreds of megabytes.
gtest_add_tests(
  TARGET ITEST_converter
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

# The binaries link the OpenBLAS (through the OpenCV of some systems), which
# starts a thread of every core at the start of every process. The conversion
# needs none of them, while the tests start the converter the hundreds of
# times.
set_tests_properties(
  ${noArgsTests}
  PROPERTIES
    TIMEOUT 3600
    ENVIRONMENT "OPENBLAS_NUM_THREADS=1"
)
