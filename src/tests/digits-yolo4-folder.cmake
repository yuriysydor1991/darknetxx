cmake_minimum_required(VERSION 3.13)

# The YOLO4 folder of the digits glyphs (see the src/tests/integration/digits
# directory), with the image paths of the training list relative to it, which
# the GUI component tests copy and train.
set(DARKNETXX_TESTS_YOLO4_DIR "${CMAKE_CURRENT_BINARY_DIR}/yolo4")

set(names "")
set(train_list "")

foreach(digit RANGE 9)
  # the label images are named after the ASCII code of the glyph
  math(EXPR glyph_code "48 + ${digit}")
  string(APPEND names "${digit}\n")

  foreach(size RANGE 7)
    set(image "data/obj/digit_${digit}_${size}")
    configure_file(
      ${PROJECT_SOURCE_DIR}/misc/data/labels/${glyph_code}_${size}.png
      ${DARKNETXX_TESTS_YOLO4_DIR}/${image}.png
      COPYONLY
    )
    file(WRITE ${DARKNETXX_TESTS_YOLO4_DIR}/${image}.txt "${digit} 0.5 0.5 1 1\n")
    string(APPEND train_list "${image}.png\n")
  endforeach()
endforeach()

file(WRITE ${DARKNETXX_TESTS_YOLO4_DIR}/data/obj.names "${names}")
file(WRITE ${DARKNETXX_TESTS_YOLO4_DIR}/data/train.txt "${train_list}")

set(ITEST_DIGITS_NAMES "data/obj.names")
set(ITEST_DIGITS_TRAIN_LIST "data/train.txt")
set(ITEST_DIGITS_BACKUP_DIR "backup")

configure_file(
  ${PROJECT_SOURCE_DIR}/src/tests/integration/digits/digits.cfg.in
  ${DARKNETXX_TESTS_YOLO4_DIR}/cfg/digits.cfg
  @ONLY
)
