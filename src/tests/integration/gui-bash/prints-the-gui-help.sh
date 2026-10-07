#!/bin/bash
#
# The help and the version messages of the GUI binary name it.

set -euo pipefail

GUI_BINARY="$1"
NAME="$(basename "${GUI_BINARY}")"

if ! grep -qF -- "${NAME} [OPTIONS]" <<< "$("${GUI_BINARY}" --help)" ; then
    echo "FAIL: the help does not name ${NAME}" >&2
    exit 1
fi

if ! grep -qF -- "${NAME} version " <<< "$("${GUI_BINARY}" --version)" ; then
    echo "FAIL: the version does not name ${NAME}" >&2
    exit 1
fi
