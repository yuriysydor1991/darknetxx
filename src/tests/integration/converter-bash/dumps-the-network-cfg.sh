#!/bin/bash
#
# The network cfg the dxxwjz2 and the dxxwjz3 files and their projects keep is
# printed into the stdout alone: the cfg text of the dxxwjz2 file as it is, the
# one of the JSON of the dxxwjz3 file of no comments, which makes the very same
# dxxwjz3 file again. The files of no network cfg print nothing and fail.

. "$(dirname "$0")/common.sh"

itest_init "$@"

# @brief Prints the network cfg of the file by the binary into the given file,
# fails unless the run succeeds with nothing in the stderr.
itest_dump_cfg()
{
    if ! "$1" --dump-cfg "$2" > "$3" 2> "${WORK_DIR}/run.log" \
       || [[ -s "${WORK_DIR}/run.log" ]] ; then
        itest_fail "The cfg dump of $2 failed:
$(tail -n 40 "${WORK_DIR}/run.log")"
    fi
}

CFG="${REFERENCE_DIR}/1.cfg"
NAMES="${REFERENCE_DIR}/1.names"
WEIGHTS="${REFERENCE_DIR}/1.weights"

for FORMAT in dxxwjz1 dxxwjz2 dxxwjz3 ; do
    itest_run "${CONVERTER_BINARY}" \
      --convert-from weights --convert-src "${WEIGHTS}" \
      --convert-to ${FORMAT} --convert-dst "${WORK_DIR}/1.${FORMAT}" \
      --convert-cfg "${CFG}" --convert-names "${NAMES}"
done

itest_dump_cfg "${CONVERTER_BINARY}" "${WORK_DIR}/1.dxxwjz2" "${WORK_DIR}/2.cfg"
itest_expect_same_files "${CFG}" "${WORK_DIR}/2.cfg"

itest_dump_cfg "${CONVERTER_BINARY}" "${WORK_DIR}/1.dxxwjz3" "${WORK_DIR}/3.cfg"

if grep -q '^#' "${WORK_DIR}/3.cfg" ; then
    itest_fail "The cfg of the dxxwjz3 file keeps the comments"
fi

itest_run "${CONVERTER_BINARY}" \
  --convert-from weights --convert-src "${WEIGHTS}" \
  --convert-to dxxwjz3 --convert-dst "${WORK_DIR}/again.dxxwjz3" \
  --convert-cfg "${WORK_DIR}/3.cfg" --convert-names "${NAMES}"

itest_expect_same_files "${WORK_DIR}/1.dxxwjz3" "${WORK_DIR}/again.dxxwjz3"

for PROJECT in dxxwjz2p dxxwjz3p ; do
    itest_run "${CONVERTER_BINARY}" \
      --convert-from dxxwjz3 --convert-src "${WORK_DIR}/1.dxxwjz3" \
      --convert-to ${PROJECT} --convert-dst "${WORK_DIR}/1.${PROJECT}"

    itest_dump_cfg "${CONVERTER_BINARY}" "${WORK_DIR}/1.${PROJECT}" \
      "${WORK_DIR}/${PROJECT}.cfg"
    itest_expect_same_files "${WORK_DIR}/3.cfg" "${WORK_DIR}/${PROJECT}.cfg"
done

itest_dump_cfg "${DARKNETXX_BINARY}" "${WORK_DIR}/1.dxxwjz3" "${WORK_DIR}/main.cfg"
itest_expect_same_files "${WORK_DIR}/3.cfg" "${WORK_DIR}/main.cfg"

for NO_CFG in "${WORK_DIR}/1.dxxwjz1" "${WEIGHTS}" "${CFG}" \
              "${WORK_DIR}/nothing-here.dxxwjz3" ; do
    if "${CONVERTER_BINARY}" --dump-cfg "${NO_CFG}" \
         > "${WORK_DIR}/none.cfg" 2> "${WORK_DIR}/run.log" ; then
        itest_fail "The cfg dump of ${NO_CFG} succeeded"
    fi

    if [[ -s "${WORK_DIR}/none.cfg" ]] ; then
        itest_fail "The failed cfg dump of ${NO_CFG} printed into the stdout"
    fi
done

# the dump goes alone
itest_run_failing "${CONVERTER_BINARY}" --dump-cfg "${WORK_DIR}/1.dxxwjz3" \
  --convert-from dxxwjz3 --convert-src "${WORK_DIR}/1.dxxwjz3" \
  --convert-to dxxwjz2 --convert-dst "${WORK_DIR}/never.dxxwjz2"

if [[ -e "${WORK_DIR}/never.dxxwjz2" ]] ; then
    itest_fail "The dump combined with the conversion converted"
fi
