#!/bin/bash -e
#
# Trains the YOLO4 folder by the darknetxx-train-gui of the CPU training docker
# image (the docker-single-run-darknetxx-cpu-train-gui target), which takes
# every CPU core and the whole memory of the docker host.
#
#   cpu-train-gui.sh <YOLO4 folder> [quick build scripts parameters...]
#
# The DOCKER_GUI_TRAINING_ARGS list holds the darknetxx-train-gui arguments,
# the mAP calculation for example:
#
#   cpu-train-gui.sh <YOLO4 folder> -DDOCKER_GUI_TRAINING_ARGS=--map

PROJECT_ROOT=$(realpath "$(dirname "$0")/../..")
BUILD_SCRIPTS_ROOT=$(realpath "${PROJECT_ROOT}/scripts/build")

if [[ ! -d "$1" ]] ; then
    echo "Usage: $0 <YOLO4 folder> [quick build scripts parameters...]"
    exit 1
fi

YOLO4_DIR=$(realpath "$1")
shift

"${BUILD_SCRIPTS_ROOT}/release-configure.sh" -DENABLE_DOCKER=ON \
    -DENABLE_DOCKER_DARKNETXX_CPU_TRAIN=ON \
    "-DYOLO4_TRAINING_DATA_DIR=${YOLO4_DIR}" "$@"

"${BUILD_SCRIPTS_ROOT}/release-build.sh" \
    --target docker-single-run-darknetxx-cpu-train-gui "$@"
