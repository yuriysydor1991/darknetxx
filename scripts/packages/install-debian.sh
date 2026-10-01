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
    libgmock-dev
    libgtest-dev
    # the OpenCV is the mandatory dependency of the Darknet code
    libopencv-dev
    libstdc++6
    # the zlib and the nlohmann json are the mandatory dependencies of the
    # dxxwjz1 network weights files (the gzip compressed JSON documents)
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
