#!/bin/bash
#
# The GTK 4 and the Qt 6 toolkits are the ones of the GUI binaries alone.

set -euo pipefail

for BINARY in "$@" ; do
    if ldd "${BINARY}" | grep -E 'libgtk-4|libgtkmm-4|libQt6' ; then
        echo "FAIL: $(basename "${BINARY}") links a GUI toolkit" >&2
        exit 1
    fi
done
