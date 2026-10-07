cmake_minimum_required(VERSION 3.13)

# The same integration test of the whole darknetxx executable as the
# ITEST_digits one, driven by the ITEST_digits_bash.sh script alone with no
# test framework. The ITEST_digits_bash_training test trains the network from
# scratch, then the ITEST_digits_bash tests run the darknetxx detection on every
# glyph of their digit and expect it to be recognized.

if(NOT TARGET ${DARKNETXX_BINARY_NAME})
  message(STATUS
    "The ITEST_digits_bash tests are skipped, the binary they drive is not being built")
  return()
endif()

find_program(BASH_EXEC bash REQUIRED)

set(ITEST_DIGITS_BASH_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/ITEST_digits_bash.sh")

add_test(
  NAME ITEST_digits_bash_training
  COMMAND ${BASH_EXEC} ${ITEST_DIGITS_BASH_SCRIPT} train
    $<TARGET_FILE:${DARKNETXX_BINARY_NAME}>
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/misc/data/labels
)

# The training takes under a minute for the Debug build and a couple of
# minutes for the sanitized ones, so the timeout leaves room for slow machines.
# The ctest starts the training on its own before the tests which require the
# trained network, even for a filtered run.
set_tests_properties(
  ITEST_digits_bash_training
  PROPERTIES
    FIXTURES_SETUP ITEST_digits_bash_trained
    TIMEOUT 3600
)

set(recognition_tests "")

foreach(digit RANGE 9)
  add_test(
    NAME ITEST_digits_bash.recognizes_digit_${digit}
    COMMAND ${BASH_EXEC} ${ITEST_DIGITS_BASH_SCRIPT} recognize
      $<TARGET_FILE:${DARKNETXX_BINARY_NAME}>
      ${CMAKE_CURRENT_BINARY_DIR}
      ${digit}
  )

  list(APPEND recognition_tests ITEST_digits_bash.recognizes_digit_${digit})
endforeach()

set_tests_properties(
  ${recognition_tests}
  PROPERTIES
    FIXTURES_REQUIRED ITEST_digits_bash_trained
    TIMEOUT 600
)

# The ThreadSanitizer fails the training with the known race of the yolo layer
# statistics counter, which the misc/.tsan-suppress file suppresses alone.
if (ENABLE_SANITIZERS_THREADS)
  set_tests_properties(
    ITEST_digits_bash_training ${recognition_tests}
    PROPERTIES
      ENVIRONMENT "TSAN_OPTIONS=suppressions=${PROJECT_SOURCE_DIR}/misc/.tsan-suppress"
  )
endif()
