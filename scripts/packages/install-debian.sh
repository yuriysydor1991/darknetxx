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
)

APT_SUDO=""

if [[ ${EUID} -ne 0 ]] ; then
    APT_SUDO="sudo"
fi

log "Installing ${#PACKAGES[@]} packages"

${APT_SUDO} apt-get update

${APT_SUDO} apt-get install -y "${PACKAGES[@]}"

log "The required packages are installed"
