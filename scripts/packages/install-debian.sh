#!/bin/bash -e
#
# Installs the minimal set of packages required to build and run the project
# locally, the tests included.

PROJECT_ROOT=$(realpath "$(dirname "$0")/../..")

. "${PROJECT_ROOT}/scripts/common.sh"

PACKAGES=(
    build-essential
    cmake
    g++
    git
    googletest
    # the GTK 4 toolkit of the GUI binaries (-DENABLE_DARKNETXX_GUI=OFF
    # builds without it)
    libgtkmm-4.0-dev
    libgmock-dev
    libgtest-dev
    # the OpenCV is the mandatory dependency of the Darknet code
    libopencv-dev
    libstdc++6
    # the zlib and the nlohmann json are the mandatory dependencies of the
    # darknetxx-datadrivers library of the network weights files (the gzip
    # compressed JSON documents), the nlohmann json the one of the tests
    nlohmann-json3-dev
    zlib1g-dev
)

APT_SUDO=""

if [[ ${EUID} -ne 0 ]] ; then
    APT_SUDO="sudo"
fi

log "Installing ${#PACKAGES[@]} packages"

${APT_SUDO} apt-get update

${APT_SUDO} apt-get install -y "${PACKAGES[@]}"

log "The required packages are installed"

# The network weights files formats come from the darknetxx-datadrivers
# library, which is no distribution package: it is built of it's sources and
# installed by the script of the misc/ci directory.
log "Build and install the darknetxx-datadrivers library the project is" \
    "built against with misc/ci/install-darknetxx-datadrivers.sh"
