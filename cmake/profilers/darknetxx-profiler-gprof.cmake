cmake_minimum_required(VERSION 3.13)

# the run of the darknetxx executable is what is checked here
if(NOT ENABLE_DARKNETXX_EXECUTABLES)
  message(STATUS "The gprof target is skipped, the ${DARKNETXX_BINARY_NAME} executable is not being built")
  return()
endif()

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
  message(FATAL_ERROR "Profiling is only for the Debug builds")
endif()

find_program(GPROF_EXEC gprof REQUIRED)

set(EXTRA_COMPILE_OPTIONS
  ${EXTRA_COMPILE_OPTIONS}
  -pg -fprofile-generate
)

set(EXTRA_LINK_OPTIONS
  ${EXTRA_LINK_OPTIONS}
  -pg -fprofile-generate
)

# the Darknet C core, where the time goes, takes it's own compile options
set(EXTRA_C_COMPILE_OPTIONS
  ${EXTRA_C_COMPILE_OPTIONS}
  -pg -fprofile-generate
)

set(
  GPROF_ANALYZE_TXT_DST
  ${PROJECT_BINARY_DIR}/gprof-analyze.txt
  CACHE STRING "The gprof analyze result txt file"
)

message(STATUS "gprof: ${GPROF_EXEC}")
message(STATUS "gprof analyze dst: ${GPROF_ANALYZE_TXT_DST}")

add_custom_target(
  gprof-analyze
  COMMAND $<TARGET_FILE:${DARKNETXX_BINARY_NAME}> ${PROFILED_RUN_ARGS}
    && ${GPROF_EXEC} -I${PROJECT_SOURCE_DIR} $<TARGET_FILE:${DARKNETXX_BINARY_NAME}>
       ${PROJECT_BINARY_DIR}/gmon.out > ${GPROF_ANALYZE_TXT_DST}
  COMMENT "Executing gprof command for the project main binary file. Outputs into the ${GPROF_ANALYZE_TXT_DST}."
  DEPENDS ${DARKNETXX_BINARY_NAME}
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
)