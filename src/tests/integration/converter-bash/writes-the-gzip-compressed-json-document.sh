#!/bin/bash
#
# The dxxwjz1 file is the gzip compressed JSON document, which the stock gzip
# tools read, and the edited plain JSON of it converts as well.

. "$(dirname "$0")/common.sh"

itest_init "$@"

DXXWJZ1="${WORK_DIR}/1.dxxwjz1"

itest_run "${CONVERTER_BINARY}" \
  --convert-from weights --convert-src "${REFERENCE_DIR}/1.weights" \
  --convert-to dxxwjz1 --convert-dst "${DXXWJZ1}" \
  --convert-cfg "${REFERENCE_DIR}/1.cfg"

if ! gzip -t "${DXXWJZ1}" ; then
    itest_fail "The ${DXXWJZ1} isn't a valid gzip file"
fi

TEXT="$(gzip -dc "${DXXWJZ1}")"

itest_expect_output_contains "${TEXT}" '"format": "dxxwjz1"'
itest_expect_output_contains "${TEXT}" '"version": 1'
itest_expect_output_contains "${TEXT}" '"header": {"major": 0, "minor": 2, "revision": 5, "seen": '
itest_expect_output_contains "${TEXT}" '{"index": 0, "type": "convolutional", "arrays": {'

# the plain JSON text converts to the same original weights file
gzip -dc "${DXXWJZ1}" > "${WORK_DIR}/1.json"

itest_run "${CONVERTER_BINARY}" \
  --convert-from dxxwjz1 --convert-src "${WORK_DIR}/1.json" \
  --convert-to weights --convert-dst "${WORK_DIR}/1.weights"

itest_expect_same_files "${REFERENCE_DIR}/1.weights" "${WORK_DIR}/1.weights"
