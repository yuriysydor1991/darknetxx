#!/bin/bash
#
# The reference weights files convert to the dxxwjz1 ones and back into the
# same bytes: with the network cfg and without it (the dxxwjz1 file keeps its
# structure), the dxxwjz1 file converts into itself.

. "$(dirname "$0")/common.sh"

itest_init "$@"

for NETWORK in 1 2 ; do
    CFG="${REFERENCE_DIR}/${NETWORK}.cfg"
    WEIGHTS="${REFERENCE_DIR}/${NETWORK}.weights"
    DXXWJZ1="${WORK_DIR}/${NETWORK}.dxxwjz1"

    itest_run "${CONVERTER_BINARY}" \
      --convert-from weights --convert-src "${WEIGHTS}" \
      --convert-to dxxwjz1 --convert-dst "${DXXWJZ1}" \
      --convert-cfg "${CFG}"

    itest_run "${CONVERTER_BINARY}" \
      --convert-from dxxwjz1 --convert-src "${DXXWJZ1}" \
      --convert-to weights --convert-dst "${WORK_DIR}/${NETWORK}.back.weights"

    itest_expect_same_files "${WEIGHTS}" "${WORK_DIR}/${NETWORK}.back.weights"

    itest_run "${CONVERTER_BINARY}" \
      --convert-from dxxwjz1 --convert-src "${DXXWJZ1}" \
      --convert-to weights --convert-dst "${WORK_DIR}/${NETWORK}.cfg.weights" \
      --convert-cfg "${CFG}"

    itest_expect_same_files "${WEIGHTS}" "${WORK_DIR}/${NETWORK}.cfg.weights"

    itest_run "${CONVERTER_BINARY}" \
      --convert-from dxxwjz1 --convert-src "${DXXWJZ1}" \
      --convert-to dxxwjz1 --convert-dst "${WORK_DIR}/${NETWORK}.again.dxxwjz1"

    itest_expect_same_files "${DXXWJZ1}" "${WORK_DIR}/${NETWORK}.again.dxxwjz1"
done
