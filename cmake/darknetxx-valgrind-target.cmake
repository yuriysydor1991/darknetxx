cmake_minimum_required(VERSION 3.13)

# the run of the darknetxx executable is what is checked here
if(NOT ENABLE_DARKNETXX_EXECUTABLES)
  message(STATUS "The valgrind target is skipped, the ${DARKNETXX_BINARY_NAME} executable is not being built")
  return()
endif()

find_program(VALGRIND_EXECUTABLE NAMES valgrind)

if (NOT VALGRIND_EXECUTABLE)
    message(
      WARNING
      "No valgrind executable found. "
      "No memory check target will be declared. "
      "Install valgrind by using command (GNU/Linux based):\n"
      "sudo apt install -y valgrind\n"
    )

    return()
endif()

message(STATUS "valgrind: ${VALGRIND_EXECUTABLE}")

add_custom_target(
  valgrind
  COMMAND ${VALGRIND_EXECUTABLE} --leak-check=full
    --error-exitcode=1 
    --show-leak-kinds=all 
    # the OpenMP (libgomp) worker threads keep their thread local storage till
    # the process exit, which counts as the possibly lost memory, so only the
    # definitely lost blocks fail the check, while all of them are shown
    --errors-for-leak-kinds=definite
    $<TARGET_FILE:${DARKNETXX_BINARY_NAME}> ${PROFILED_RUN_ARGS}
  COMMENT "Executing valgrind command for the project main binary file. Output all findings into the stdout."
  DEPENDS ${DARKNETXX_BINARY_NAME}
)
