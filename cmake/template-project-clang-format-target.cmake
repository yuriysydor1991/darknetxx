cmake_minimum_required(VERSION 3.13)

# The misc/.clang-format style uses the options of the clang-format 16 and
# newer (the InsertNewlineAtEOF one, for example), which the older versions
# reject, so the first installed version of those is taken.
set(CLANG_FORMAT_MIN_VERSION 16)

foreach(
  clang_format_name
  clang-format
  clang-format-22 clang-format-21 clang-format-20 clang-format-19
  clang-format-18 clang-format-17 clang-format-16
)
  unset(CLANG_FORMAT_CANDIDATE CACHE)

  find_program(CLANG_FORMAT_CANDIDATE ${clang_format_name})

  if (NOT CLANG_FORMAT_CANDIDATE)
    continue()
  endif()

  execute_process(
    COMMAND ${CLANG_FORMAT_CANDIDATE} --version
    OUTPUT_VARIABLE clang_format_version_output
    ERROR_QUIET
  )

  string(REGEX MATCH "version ([0-9]+)" clang_format_version_match "${clang_format_version_output}")

  if (CMAKE_MATCH_1 AND NOT CMAKE_MATCH_1 LESS CLANG_FORMAT_MIN_VERSION)
    set(CLANG_FORMAT_EXEC ${CLANG_FORMAT_CANDIDATE})
    break()
  endif()

  message(STATUS "Skipping the clang-format older than ${CLANG_FORMAT_MIN_VERSION}: ${CLANG_FORMAT_CANDIDATE}")
endforeach()

unset(CLANG_FORMAT_CANDIDATE CACHE)

message(STATUS "clang-format command found: " ${CLANG_FORMAT_EXEC})

if (NOT CLANG_FORMAT_EXEC)
  message(
    WARNING
    "No clang-format ${CLANG_FORMAT_MIN_VERSION} or newer executable found. No code formatting target defined."
  )
  return()
endif()

add_custom_target (
  clang-format
  COMMAND ${CLANG_FORMAT_EXEC} -style=file:'./misc/.clang-format' --verbose -i ${ALLSOURCES}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
