#!/bin/bash
#
# Shared helpers of the network weights files converter integration tests.
#
# Every case is a script of its own, started by ctest as
#
#   bash <case>.sh <converter> <darknetxx> <reference data dir> <work dir>
#
# and reporting its verdict through the exit status, which is what ctest reads.

set -euo pipefail

itest_fail()
{
    echo "FAIL: $*" >&2

    exit 1
}

# @brief Accepts the four arguments every case is started with and prepares an
# empty work directory of its own.
itest_init()
{
    CONVERTER_BINARY="${1:-}"
    DARKNETXX_BINARY="${2:-}"
    REFERENCE_DIR="${3:-}"
    WORK_DIR="${4:-}"

    if [[ -z "${CONVERTER_BINARY}" ]] || [[ -z "${DARKNETXX_BINARY}" ]] \
       || [[ -z "${REFERENCE_DIR}" ]] || [[ -z "${WORK_DIR}" ]] ; then
        itest_fail "usage: $0 <converter> <darknetxx> <reference data dir> <work dir>"
    fi

    for BINARY in "${CONVERTER_BINARY}" "${DARKNETXX_BINARY}" ; do
        if [[ ! -x "${BINARY}" ]] ; then
            itest_fail "No binary to run at ${BINARY}"
        fi
    done

    rm -rf "${WORK_DIR}"
    mkdir -p "${WORK_DIR}"
}

# @brief Runs the binary with the arguments, the output goes into the log of
# the work directory. Fails unless the run succeeds.
itest_run()
{
    local binary="$1"
    shift

    if ! "${binary}" "$@" > "${WORK_DIR}/run.log" 2>&1 ; then
        itest_fail "The run of $(basename "${binary}") $* failed:
$(tail -n 40 "${WORK_DIR}/run.log")"
    fi
}

# @brief Runs the binary with the arguments expecting it to fail.
itest_run_failing()
{
    local binary="$1"
    shift

    if "${binary}" "$@" > "${WORK_DIR}/run.log" 2>&1 ; then
        itest_fail "The run of $(basename "${binary}") $* succeeded"
    fi
}

# @brief Fails unless the two files hold the same bytes.
itest_expect_same_files()
{
    if ! cmp -s -- "$1" "$2" ; then
        itest_fail "The files $1 and $2 differ"
    fi
}

# @brief Fails unless the given text is part of the given one. The text is
# handed over with no pipe: the grep stops reading at the first match, which
# would fail the writer of a long text with the SIGPIPE under the pipefail.
itest_expect_output_contains()
{
    local produced="$1"
    local wanted="$2"

    if ! grep -qF -- "${wanted}" <<< "${produced}" ; then
        itest_fail "The output carries no '${wanted}':
${produced}"
    fi
}
