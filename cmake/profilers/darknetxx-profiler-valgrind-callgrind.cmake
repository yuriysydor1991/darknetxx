cmake_minimum_required(VERSION 3.13)

# the run of the darknetxx executable is what is checked here
if(NOT ENABLE_DARKNETXX_EXECUTABLES)
  message(STATUS "The callgrind target is skipped, the ${DARKNETXX_BINARY_NAME} executable is not being built")
  return()
endif()

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
  message(FATAL_ERROR "Profiling is only for the Debug builds")
endif()

find_program(VALGRIND_EXEC valgrind)

# the debug build scripts enable the target, so a host without the valgrind
# skips it instead of failing the whole configure
if (NOT VALGRIND_EXEC)
  message(
    WARNING
    "No valgrind executable found. "
    "No callgrind target will be declared. "
    "Install valgrind by using command (GNU/Linux based):\n"
    "sudo apt install -y valgrind\n"
  )

  return()
endif()

message(STATUS "valgrind (callgrind): ${VALGRIND_EXEC}")

add_custom_target(
  callgrind
  COMMAND ${VALGRIND_EXEC} --tool=callgrind $<TARGET_FILE:${DARKNETXX_BINARY_NAME}> ${PROFILED_RUN_ARGS}
  COMMENT "Executing valgrind --tool=callgrind command for the project main binary file. Outputs into the ${PROJECT_BINARY_DIR}/callgrind.out.* files"
  DEPENDS ${DARKNETXX_BINARY_NAME}
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
)

