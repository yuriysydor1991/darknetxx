cmake_minimum_required(VERSION 3.13)

# The integration tests of the GUI binaries driven by the shell scripts alone,
# started by ctest as
#
#   bash prints-the-gui-help.sh <GUI binary>
#   bash no-gui-toolkit-in-the-other-binaries.sh <darknetxx> [<darknetxxConverter>]
#
# and reporting their verdict through the exit status. The windows themselves
# are tested by the CTEST_TrainWindow, the CTEST_RecognitionWindow and the
# CTEST_NetEditorWindow tests.
if(NOT DARKNETXX_GUI_TARGETS)
  message(STATUS
    "The ITEST_gui_bash tests are skipped, the binaries they drive are not "
    "being built")
  return()
endif()

find_program(BASH_EXEC bash REQUIRED)

foreach(GUI_BINARY_NAME IN LISTS DARKNETXX_GUI_TARGETS)
  string(REPLACE "-" "_" ITEST_BINARY_NAME "${GUI_BINARY_NAME}")

  add_test(
    NAME ITEST_gui_bash.prints_the_gui_help.${ITEST_BINARY_NAME}
    COMMAND
      ${BASH_EXEC}
      "${CMAKE_CURRENT_SOURCE_DIR}/prints-the-gui-help.sh"
      "$<TARGET_FILE:${GUI_BINARY_NAME}>"
  )

  set_tests_properties(
    ITEST_gui_bash.prints_the_gui_help.${ITEST_BINARY_NAME}
    PROPERTIES TIMEOUT 600
  )
endforeach()

# the main binaries of no network editor only build
if(NOT TARGET ${DARKNETXX_BINARY_NAME})
  return()
endif()

set(ITEST_OTHER_BINARIES "$<TARGET_FILE:${DARKNETXX_BINARY_NAME}>")

if(TARGET ${DARKNETXX_CONVERTER_BINARY_NAME})
  list(APPEND ITEST_OTHER_BINARIES "$<TARGET_FILE:${DARKNETXX_CONVERTER_BINARY_NAME}>")
endif()

add_test(
  NAME ITEST_gui_bash.no_gui_toolkit_in_the_other_binaries
  COMMAND
    ${BASH_EXEC}
    "${CMAKE_CURRENT_SOURCE_DIR}/no-gui-toolkit-in-the-other-binaries.sh"
    ${ITEST_OTHER_BINARIES}
)

set_tests_properties(
  ITEST_gui_bash.no_gui_toolkit_in_the_other_binaries
  PROPERTIES TIMEOUT 600
)
