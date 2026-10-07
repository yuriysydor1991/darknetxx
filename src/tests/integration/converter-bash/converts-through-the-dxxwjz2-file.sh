#!/bin/bash
#
# The reference weights files convert to the dxxwjz2 ones, which keep the
# network cfg and the class names beside the weights, and back into the same
# bytes without the network cfg, through the dxxwjz1 files as well. The cfg
# alone makes the network to train, the dxxwjz2 file of no weights.

. "$(dirname "$0")/common.sh"

itest_init "$@"

# the class names of the classes=2 network
printf 'cat\ndog\n' > "${WORK_DIR}/2.names"

for NETWORK in 1 2 ; do
    CFG="${REFERENCE_DIR}/${NETWORK}.cfg"
    WEIGHTS="${REFERENCE_DIR}/${NETWORK}.weights"
    DXXWJZ2="${WORK_DIR}/${NETWORK}.dxxwjz2"
    NAMES="${REFERENCE_DIR}/1.names"

    if [[ ${NETWORK} == 2 ]] ; then
        NAMES="${WORK_DIR}/2.names"
    fi

    itest_run "${CONVERTER_BINARY}" \
      --convert-from weights --convert-src "${WEIGHTS}" \
      --convert-to dxxwjz2 --convert-dst "${DXXWJZ2}" \
      --convert-cfg "${CFG}" --convert-names "${NAMES}"

    TEXT="$(gzip -dc "${DXXWJZ2}")"

    itest_expect_output_contains "${TEXT}" '"format": "dxxwjz2"'
    itest_expect_output_contains "${TEXT}" '"version": 2'
    itest_expect_output_contains "${TEXT}" '"network": {"cfg": "'
    itest_expect_output_contains "${TEXT}" '{"index": 0, "name": '
    itest_expect_output_contains "${TEXT}" '"training": {"seconds": 0,'

    itest_run "${CONVERTER_BINARY}" \
      --convert-from dxxwjz2 --convert-src "${DXXWJZ2}" \
      --convert-to weights --convert-dst "${WORK_DIR}/${NETWORK}.back.weights"

    itest_expect_same_files "${WEIGHTS}" "${WORK_DIR}/${NETWORK}.back.weights"

    itest_run "${CONVERTER_BINARY}" \
      --convert-from dxxwjz2 --convert-src "${DXXWJZ2}" \
      --convert-to dxxwjz1 --convert-dst "${WORK_DIR}/${NETWORK}.dxxwjz1"

    itest_run "${CONVERTER_BINARY}" \
      --convert-from dxxwjz1 --convert-src "${WORK_DIR}/${NETWORK}.dxxwjz1" \
      --convert-to dxxwjz2 --convert-dst "${WORK_DIR}/${NETWORK}.again.dxxwjz2" \
      --convert-cfg "${CFG}"

    itest_run "${CONVERTER_BINARY}" \
      --convert-from dxxwjz2 --convert-src "${WORK_DIR}/${NETWORK}.again.dxxwjz2" \
      --convert-to weights --convert-dst "${WORK_DIR}/${NETWORK}.around.weights"

    itest_expect_same_files "${WEIGHTS}" "${WORK_DIR}/${NETWORK}.around.weights"

    # the dxxwjz2 file keeps the network description, which the dxxwjz1 one
    # doesn't
    itest_run_failing "${CONVERTER_BINARY}" \
      --convert-from dxxwjz1 --convert-src "${WORK_DIR}/${NETWORK}.dxxwjz1" \
      --convert-to dxxwjz2 --convert-dst "${WORK_DIR}/never.dxxwjz2"

    if [[ -e "${WORK_DIR}/never.dxxwjz2" ]] ; then
        itest_fail "The failed conversion wrote ${WORK_DIR}/never.dxxwjz2"
    fi
done

TO_TRAIN="${WORK_DIR}/to_train.dxxwjz2"

itest_run "${CONVERTER_BINARY}" \
  --convert-to dxxwjz2 --convert-dst "${TO_TRAIN}" \
  --convert-cfg "${REFERENCE_DIR}/2.cfg" --convert-names "${WORK_DIR}/2.names"

TEXT="$(gzip -dc "${TO_TRAIN}")"

itest_expect_output_contains "${TEXT}" '{"index": 1, "name": "dog"}'

if [[ ${TEXT} == *'"layers"'* ]] ; then
    itest_fail "The network to train ${TO_TRAIN} keeps the layers"
fi

# the formats of the weights alone keep nothing of the network to train
itest_run_failing "${CONVERTER_BINARY}" \
  --convert-from dxxwjz2 --convert-src "${TO_TRAIN}" \
  --convert-to weights --convert-dst "${WORK_DIR}/never.weights"

itest_run_failing "${CONVERTER_BINARY}" \
  --convert-to dxxwjz2 --convert-dst "${WORK_DIR}/never.dxxwjz2"

for NEVER in never.weights never.dxxwjz2 ; do
    if [[ -e "${WORK_DIR}/${NEVER}" ]] ; then
        itest_fail "The failed conversion wrote ${WORK_DIR}/${NEVER}"
    fi
done
