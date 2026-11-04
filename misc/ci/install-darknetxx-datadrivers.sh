#!/bin/sh
#
# Builds and installs the darknetxx-datadrivers library the project is built
# against: the network weights files and the training projects formats, their
# documents and their converter (the library installs the darknetxxConverter
# binary beside itself). It is no distribution package and it is not fetched
# by the project configure - the
# cmake/enablers/darknetxx-DarknetxxDataDrivers-enabler.cmake only looks it
# up with find_package() - so every environment that configures this project
# has to provide it first. That is what this script is for, on the machines of
# the developers and of the pipelines which build the project outside of the
# Docker images of the misc/Dockerfiles directory (the images, the Jenkins
# pipeline one included, build and install the library themselves).
#
# Usage:
#
#   misc/ci/install-darknetxx-datadrivers.sh [install-prefix]
#
# The install prefix defaults to /usr/local. Point the project at a different
# one with -DCMAKE_PREFIX_PATH=<prefix> and, at the run time, with
# LD_LIBRARY_PATH=<prefix>/lib.
#
# The repository and the revision may be overridden through the environment,
# which is what a pipeline validating an unreleased library revision does:
#
#   DXXDD_GIT, DXXDD_GIT_TAG
#
# The library itself needs the zlib and the nlohmann json development
# packages (and, optionally, the OpenCV one), which the
# scripts/packages/install-*.sh scripts of this project install as well.
set -eu

PREFIX="${1:-/usr/local}"

DXXDD_GIT="${DXXDD_GIT:-https://github.com/yuriysydor1991/darknetxx-datadrivers.git}"
DXXDD_GIT_TAG="${DXXDD_GIT_TAG:-main}"

# The install step is the only one that may need the elevated rights, and only
# when the prefix is not writable by the current user.
SUDO=""
if [ ! -w "$(dirname "$PREFIX")" ] && [ "$(id -u)" -ne 0 ]; then
  SUDO="sudo"
fi

WORKDIR="$(mktemp -d)"
trap 'rm -rf "$WORKDIR"' EXIT

echo "=== darknetxx-datadrivers ($DXXDD_GIT @ $DXXDD_GIT_TAG) -> $PREFIX"

git clone --depth 1 --branch "$DXXDD_GIT_TAG" "$DXXDD_GIT" \
  "$WORKDIR/darknetxx-datadrivers"

cmake -S "$WORKDIR/darknetxx-datadrivers" \
  -B "$WORKDIR/darknetxx-datadrivers-build" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="$PREFIX"

cmake --build "$WORKDIR/darknetxx-datadrivers-build" -j"$(nproc)"

$SUDO cmake --install "$WORKDIR/darknetxx-datadrivers-build"

# A prefix outside of the default linker search path is only complete once the
# loader knows about it.
if [ "$PREFIX" != "/usr" ] && [ "$PREFIX" != "/usr/local" ]; then
  echo "NOTE: run the project with LD_LIBRARY_PATH=$PREFIX/lib"
elif [ -x "$(command -v ldconfig)" ]; then
  $SUDO ldconfig
fi

echo "OK: the darknetxx-datadrivers library is installed into $PREFIX"
