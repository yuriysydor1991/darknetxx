cmake_minimum_required(VERSION 3.13)

# The integration tests of the network weights files converter only binary
# driven by the shell scripts alone, with no test framework: every case is a
# script of its own, started by ctest as
#
#   bash <case>.sh <converter> <darknetxx> <reference data dir> <work dir>
#
# and reporting its verdict through the exit status. What is driven here is
# the produced binaries themselves - no test framework, no mock and no library
# of this project is involved - so a case fails whenever the shipped converter
# would fail in the hands of a user.
if(NOT TARGET ${DARKNETXX_CONVERTER_BINARY_NAME})
  message(STATUS
    "The ITEST_converter_bash tests are skipped, the binary they drive is not "
    "being built")
  return()
endif()

find_program(BASH_EXEC bash REQUIRED)

set(ITESTS_WORK_DIR "${CMAKE_CURRENT_BINARY_DIR}/itests-data")
set(
  ITESTS_REFERENCE_DIR
  "${PROJECT_SOURCE_DIR}/src/lib/darknet-adaptor/adaptors/NetworksLoaders/OrigDefaultLoader/tests/component/OrigDefaultLoader/data"
)

set(
  ITEST_CASES
  converts-the-reference-weights-back-and-forth
  converts-through-the-dxxwjz2-file
  converts-through-the-dxxwjz3-file
  converts-the-yolo4-folder-through-the-project
  writes-the-gzip-compressed-json-document
  converts-with-the-main-binary-too
  dumps-the-network-cfg
  reports-a-failing-conversion
  prints-the-conversion-help
)

set(ITEST_NAMES "")

foreach(ITEST_CASE IN LISTS ITEST_CASES)
  string(REPLACE "-" "_" ITEST_CASE_NAME "${ITEST_CASE}")

  set(ITEST_NAME "ITEST_converter_bash.${ITEST_CASE_NAME}")

  add_test(
    NAME ${ITEST_NAME}
    COMMAND
      ${BASH_EXEC}
      "${CMAKE_CURRENT_SOURCE_DIR}/${ITEST_CASE}.sh"
      "$<TARGET_FILE:${DARKNETXX_CONVERTER_BINARY_NAME}>"
      "$<TARGET_FILE:${DARKNETXX_BINARY_NAME}>"
      "${ITESTS_REFERENCE_DIR}"
      "${ITESTS_WORK_DIR}/${ITEST_CASE}"
  )

  list(APPEND ITEST_NAMES ${ITEST_NAME})
endforeach()

# the conversion needs none of the OpenBLAS threads (see the ITEST_converter)
set_tests_properties(
  ${ITEST_NAMES}
  PROPERTIES
    TIMEOUT 600
    ENVIRONMENT "OPENBLAS_NUM_THREADS=1"
)
