cmake_minimum_required(VERSION 3.13)

# The integration test of the whole darknetxx executable. The
# ITEST_digits_training test trains a tiny network from scratch to detect the
# digit glyphs of the misc/data/labels images, then the ITEST_digits tests run
# the darknetxx detection on every glyph and expect its digit to be recognized,
# check the dxxwjz3 file of the training, resume the training and train the
# network of the dxxwjz3 file of no weights.

if(NOT TARGET ${DARKNETXX_BINARY_NAME})
  message(STATUS
    "The ITEST_digits tests are skipped, the binary they drive is not being built")
  return()
endif()

set(ITEST_DIGITS_GLYPH_SIZES 8)
set(ITEST_DIGITS_DATA_DIR "${CMAKE_CURRENT_BINARY_DIR}/data")
set(ITEST_DIGITS_NAMES "${CMAKE_CURRENT_BINARY_DIR}/digits.names")
set(ITEST_DIGITS_TRAIN_LIST "${CMAKE_CURRENT_BINARY_DIR}/train.txt")
set(ITEST_DIGITS_TRAINING_DIR "${CMAKE_CURRENT_BINARY_DIR}/training")
set(ITEST_DIGITS_BACKUP_DIR "${ITEST_DIGITS_TRAINING_DIR}/backup")
set(ITEST_DIGITS_CFG "${CMAKE_CURRENT_BINARY_DIR}/digits.cfg")
# the training names the final weights after the cfg file, the dxxwjz3 file
# (the default format) keeps the network and the training beside them
set(ITEST_DIGITS_WEIGHTS "${ITEST_DIGITS_BACKUP_DIR}/digits_final.dxxwjz3")
# the working directories of the trainings the tests resume and start
set(ITEST_DIGITS_RESUMED_DIR "${CMAKE_CURRENT_BINARY_DIR}/resumed")
set(ITEST_DIGITS_DETECTIONS_DIR "${CMAKE_CURRENT_BINARY_DIR}/detections")

# The training data set: every size of every digit glyph, labeled with a single
# box of the whole image, since the image holds the glyph alone.
set(names "")
set(train_list "")
math(EXPR last_glyph_size "${ITEST_DIGITS_GLYPH_SIZES} - 1")

foreach(digit RANGE 9)
  # the label images are named after the ASCII code of the glyph
  math(EXPR glyph_code "48 + ${digit}")
  string(APPEND names "${digit}\n")

  foreach(size RANGE ${last_glyph_size})
    set(image "${ITEST_DIGITS_DATA_DIR}/digit_${digit}_${size}")

    configure_file(
      ${PROJECT_SOURCE_DIR}/misc/data/labels/${glyph_code}_${size}.png
      ${image}.png
      COPYONLY
    )

    file(WRITE ${image}.txt "${digit} 0.5 0.5 1 1\n")
    string(APPEND train_list "${image}.png\n")
  endforeach()
endforeach()

file(WRITE ${ITEST_DIGITS_NAMES} "${names}")
file(WRITE ${ITEST_DIGITS_TRAIN_LIST} "${train_list}")

configure_file(digits.cfg.in ${ITEST_DIGITS_CFG} @ONLY)

add_test(
  NAME ITEST_digits_training
  COMMAND ${CMAKE_COMMAND}
    -D DARKNETXX=$<TARGET_FILE:${DARKNETXX_BINARY_NAME}>
    -D CFG=${ITEST_DIGITS_CFG}
    -D WORK_DIR=${ITEST_DIGITS_TRAINING_DIR}
    -D BACKUP_DIR=${ITEST_DIGITS_BACKUP_DIR}
    -D WEIGHTS=${ITEST_DIGITS_WEIGHTS}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/train-digits.cmake
)

# The training takes under a minute for the Debug build and a couple of
# minutes for the sanitized ones, so the timeout leaves room for slow machines.
# The ctest starts the training on its own before the tests which require the
# trained network, even for a filtered run.
set_tests_properties(
  ITEST_digits_training
  PROPERTIES
    FIXTURES_SETUP ITEST_digits_trained
    TIMEOUT 3600
)

configure_file(
  ITEST_digits_provider.h.in
  ITEST_digits_provider.h
  @ONLY
)

add_executable(
  ITEST_digits
  ITEST_digits.cpp
)

# the tests run the darknetxx executable, so it is built along with them
add_dependencies(ITEST_digits ${DARKNETXX_BINARY_NAME})

target_compile_definitions(
  ITEST_digits
  PRIVATE ITEST_DIGITS_DARKNETXX=\"$<TARGET_FILE:${DARKNETXX_BINARY_NAME}>\"
)

target_include_directories(
  ITEST_digits
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
)

# the dxxwjz3 files of the trainings are the gzip compressed JSON documents
target_link_libraries(
  ITEST_digits
  nlohmann_json::nlohmann_json
  ZLIB::ZLIB
  GTest::gtest_main
)

include(GoogleTest)

gtest_add_tests(
  TARGET ITEST_digits
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(
  ${noArgsTests}
  PROPERTIES
    FIXTURES_REQUIRED ITEST_digits_trained
    TIMEOUT 600
)

# The ThreadSanitizer fails the training with the known race of the yolo layer
# statistics counter, which the misc/.tsan-suppress file suppresses alone.
if (ENABLE_SANITIZERS_THREADS)
  set_tests_properties(
    ITEST_digits_training ${noArgsTests}
    PROPERTIES
      ENVIRONMENT "TSAN_OPTIONS=suppressions=${PROJECT_SOURCE_DIR}/misc/.tsan-suppress"
  )
endif()
