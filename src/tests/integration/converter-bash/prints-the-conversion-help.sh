#!/bin/bash
#
# The help message of the converter names the binary being run and lists the
# conversion flags together with every format they accept, the empty command
# line of the converter is the failed help.

. "$(dirname "$0")/common.sh"

itest_init "$@"

PRINTED="$("${CONVERTER_BINARY}" --help)"

itest_expect_output_contains "${PRINTED}" "$(basename "${CONVERTER_BINARY}") [OPTIONS]"

for CONVERSION_FLAG in --convert-from --convert-src --convert-to --convert-dst \
                       --convert-cfg --convert-names ; do
    itest_expect_output_contains "${PRINTED}" "${CONVERSION_FLAG}"
done

for CONVERSION_FORMAT in weights dxxwjz1 dxxwjz2 dxxwjz2p dxxwjz3 dxxwjz3p yolo4 ; do
    itest_expect_output_contains "${PRINTED}" "${CONVERSION_FORMAT}"
done

if grep -qF -- "--train-cfg" <<< "${PRINTED}" ; then
    itest_fail "The converter help lists the training"
fi

itest_run_failing "${CONVERTER_BINARY}"

itest_expect_output_contains "$(cat "${WORK_DIR}/run.log")" "--convert-from"

VERSION="$("${CONVERTER_BINARY}" --version)"

itest_expect_output_contains "${VERSION}" "$(basename "${CONVERTER_BINARY}") version "
