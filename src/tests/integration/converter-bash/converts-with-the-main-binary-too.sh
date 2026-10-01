#!/bin/bash
#
# The main binary carries the very same conversion: the same flags make the
# same dxxwjz1 file, which converts back into the same bytes.

. "$(dirname "$0")/common.sh"

itest_init "$@"

for BINARY in "${CONVERTER_BINARY}" "${DARKNETXX_BINARY}" ; do
    NAME="$(basename "${BINARY}")"

    itest_run "${BINARY}" \
      --convert-from weights --convert-src "${REFERENCE_DIR}/2.weights" \
      --convert-to dxxwjz1 --convert-dst "${WORK_DIR}/${NAME}.dxxwjz1" \
      --convert-cfg "${REFERENCE_DIR}/2.cfg"

    itest_run "${BINARY}" \
      --convert-from dxxwjz1 --convert-src "${WORK_DIR}/${NAME}.dxxwjz1" \
      --convert-to weights --convert-dst "${WORK_DIR}/${NAME}.weights"

    itest_expect_same_files "${REFERENCE_DIR}/2.weights" "${WORK_DIR}/${NAME}.weights"
done

itest_expect_same_files \
  "${WORK_DIR}/$(basename "${CONVERTER_BINARY}").dxxwjz1" \
  "${WORK_DIR}/$(basename "${DARKNETXX_BINARY}").dxxwjz1"
