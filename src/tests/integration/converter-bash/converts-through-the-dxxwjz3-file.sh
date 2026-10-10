#!/bin/bash
#
# The reference weights files convert to the dxxwjz3 ones, which keep the
# network cfg as the JSON structure beside the weights, and back into the same
# bytes. The dxxwjz3 file converted to the dxxwjz2 one keeps the cfg text of
# the JSON, which converts back into the same dxxwjz3 file. The cfg alone
# makes the network to train, the dxxwjz3 file of no weights.

. "$(dirname "$0")/common.sh"

itest_init "$@"

for NETWORK in 1 2 ; do
    CFG="${REFERENCE_DIR}/${NETWORK}.cfg"
    WEIGHTS="${REFERENCE_DIR}/${NETWORK}.weights"
    DXXWJZ3="${WORK_DIR}/${NETWORK}.dxxwjz3"

    itest_run "${CONVERTER_BINARY}" \
      --convert-from weights --convert-src "${WEIGHTS}" \
      --convert-to dxxwjz3 --convert-dst "${DXXWJZ3}" \
      --convert-cfg "${CFG}" --convert-names "${REFERENCE_DIR}/1.names"

    TEXT="$(gzip -dc "${DXXWJZ3}")"

    itest_expect_output_contains "${TEXT}" '"format": "dxxwjz3"'
    itest_expect_output_contains "${TEXT}" '"version": 3'
    itest_expect_output_contains "${TEXT}" '"network": {"net": {"batch": 64, '
    itest_expect_output_contains "${TEXT}" '"scales": [0.1,0.1]'
    itest_expect_output_contains "${TEXT}" \
      '{"type": "maxpool", "options": {"size": 2, "stride": 2}}'

    if [[ ${TEXT} == *'"cfg"'* ]] ; then
        itest_fail "The ${DXXWJZ3} file keeps the cfg text"
    fi

    itest_run "${CONVERTER_BINARY}" \
      --convert-from dxxwjz3 --convert-src "${DXXWJZ3}" \
      --convert-to weights --convert-dst "${WORK_DIR}/${NETWORK}.back.weights"

    itest_expect_same_files "${WEIGHTS}" "${WORK_DIR}/${NETWORK}.back.weights"

    itest_run "${CONVERTER_BINARY}" \
      --convert-from dxxwjz3 --convert-src "${DXXWJZ3}" \
      --convert-to dxxwjz2 --convert-dst "${WORK_DIR}/${NETWORK}.dxxwjz2"

    itest_expect_output_contains "$(gzip -dc "${WORK_DIR}/${NETWORK}.dxxwjz2")" \
      '"network": {"cfg": "[net]\nbatch=64\nsubdivisions='

    itest_run "${CONVERTER_BINARY}" \
      --convert-from dxxwjz2 --convert-src "${WORK_DIR}/${NETWORK}.dxxwjz2" \
      --convert-to dxxwjz3 --convert-dst "${WORK_DIR}/${NETWORK}.again.dxxwjz3"

    itest_expect_same_files "${DXXWJZ3}" "${WORK_DIR}/${NETWORK}.again.dxxwjz3"
done

TO_TRAIN="${WORK_DIR}/to_train.dxxwjz3"

itest_run "${CONVERTER_BINARY}" \
  --convert-to dxxwjz3 --convert-dst "${TO_TRAIN}" \
  --convert-cfg "${REFERENCE_DIR}/2.cfg"

TEXT="$(gzip -dc "${TO_TRAIN}")"

itest_expect_output_contains "${TEXT}" '"network": {"net": {'

if [[ ${TEXT} == *'"header"'* ]] ; then
    itest_fail "The network to train ${TO_TRAIN} keeps the weights"
fi
