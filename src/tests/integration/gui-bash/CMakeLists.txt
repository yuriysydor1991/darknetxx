cmake_minimum_required(VERSION 3.13)

# The integration tests of the GUI binary driven by the shell scripts alone,
# started by ctest as
#
#   bash <case>.sh <darknetxx-gui> <darknetxx> [<darknetxxConverter>]
#
# and reporting their verdict through the exit status. The window itself is
# tested by the CTEST_GtkmmWindow tests.
if(NOT TARGET ${DARKNETXX_GUI_BINARY_NAME})
  message(STATUS
    "The ITEST_gui_bash tests are skipped, the binary they drive is not being "
    "built")
  return()
endif()

find_program(BASH_EXEC bash REQUIRED)

set(ITEST_BINARIES "$<TARGET_FILE:${DARKNETXX_GUI_BINARY_NAME}>" "$<TARGET_FILE:${DARKNETXX_BINARY_NAME}>")

if(TARGET ${DARKNETXX_CONVERTER_BINARY_NAME})
  list(APPEND ITEST_BINARIES "$<TARGET_FILE:${DARKNETXX_CONVERTER_BINARY_NAME}>")
endif()

foreach(ITEST_CASE prints-the-gui-help no-gui-toolkit-in-the-other-binaries)
  string(REPLACE "-" "_" ITEST_CASE_NAME "${ITEST_CASE}")

  add_test(
    NAME ITEST_gui_bash.${ITEST_CASE_NAME}
    COMMAND
      ${BASH_EXEC}
      "${CMAKE_CURRENT_SOURCE_DIR}/${ITEST_CASE}.sh"
      ${ITEST_BINARIES}
  )

  set_tests_properties(ITEST_gui_bash.${ITEST_CASE_NAME} PROPERTIES TIMEOUT 600)
endforeach()
