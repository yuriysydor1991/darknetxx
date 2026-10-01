#!/bin/bash
#
# A conversion which could not be performed ends with a non zero exit status
# and writes nothing into its destination, which is what makes the binary
# usable from a script.

. "$(dirname "$0")/common.sh"

itest_init "$@"

CFG="${REFERENCE_DIR}/1.cfg"
WEIGHTS="${REFERENCE_DIR}/1.weights"
DST="${WORK_DIR}/out/never.file"

# the unknown format
itest_run_failing "${CONVERTER_BINARY}" \
  --convert-from darknet --convert-src "${WEIGHTS}" \
  --convert-to dxxwjz1 --convert-dst "${DST}" --convert-cfg "${CFG}"

# the original weights file keeps no structure, the cfg is required
itest_run_failing "${CONVERTER_BINARY}" \
  --convert-from weights --convert-src "${WEIGHTS}" \
  --convert-to dxxwjz1 --convert-dst "${DST}"

# the source that does not exist
itest_run_failing "${CONVERTER_BINARY}" \
  --convert-from dxxwjz1 --convert-src "${WORK_DIR}/nothing-here.dxxwjz1" \
  --convert-to weights --convert-dst "${DST}"

# the original weights file read as the dxxwjz1 one
itest_run_failing "${CONVERTER_BINARY}" \
  --convert-from dxxwjz1 --convert-src "${WEIGHTS}" \
  --convert-to weights --convert-dst "${DST}"

# the part of the conversion flags
itest_run_failing "${CONVERTER_BINARY}" \
  --convert-from weights --convert-src "${WEIGHTS}"

# the dxxwjz1 file of another network and the truncated one
itest_run "${CONVERTER_BINARY}" \
  --convert-from weights --convert-src "${WEIGHTS}" \
  --convert-to dxxwjz1 --convert-dst "${WORK_DIR}/1.dxxwjz1" \
  --convert-cfg "${CFG}"

itest_run_failing "${CONVERTER_BINARY}" \
  --convert-from dxxwjz1 --convert-src "${WORK_DIR}/1.dxxwjz1" \
  --convert-to weights --convert-dst "${DST}" \
  --convert-cfg "${REFERENCE_DIR}/2.cfg"

head -c 1000 "${WORK_DIR}/1.dxxwjz1" > "${WORK_DIR}/cut.dxxwjz1"

itest_run_failing "${CONVERTER_BINARY}" \
  --convert-from dxxwjz1 --convert-src "${WORK_DIR}/cut.dxxwjz1" \
  --convert-to weights --convert-dst "${DST}"

if [[ -e "${DST}" ]] || [[ -e "${WORK_DIR}/out" ]] ; then
    itest_fail "A failed conversion wrote into its destination"
fi
