#!/bin/bash
#
# The GTK 4 toolkit is the GUI binary one alone.

set -euo pipefail

shift

for BINARY in "$@" ; do
    if ldd "${BINARY}" | grep -E 'libgtk-4|libgtkmm-4' ; then
        echo "FAIL: $(basename "${BINARY}") links the GTK 4 toolkit" >&2
        exit 1
    fi
done
