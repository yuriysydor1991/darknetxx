#!/bin/bash -e
#
# Runs the darknetxx-gui of the docker image on the display of the host: the
# X11 one (allowed to the local containers while the run lasts) and the
# Wayland one of the session, when there is one.
#
#   gui-run.sh <image> <YOLO4 folder> [docker run arguments...]

IMAGE="$1"
YOLO4_DIR="$2"
shift 2

DISPLAY_ARGS=(-e DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix)

if [[ -n "${WAYLAND_DISPLAY:-}" ]] && [[ -n "${XDG_RUNTIME_DIR:-}" ]] ; then
    DISPLAY_ARGS+=(
        -e WAYLAND_DISPLAY
        -e XDG_RUNTIME_DIR=/tmp/xdg
        -v "${XDG_RUNTIME_DIR}/${WAYLAND_DISPLAY}:/tmp/xdg/${WAYLAND_DISPLAY}"
    )
fi

if [[ -n "${DISPLAY:-}" ]] ; then
    xhost +local:docker
    trap 'xhost -local:docker' EXIT
fi

# the software renderer, since the container has none of the host GPU drivers
docker run --rm --ipc=host -e GSK_RENDERER=cairo "${DISPLAY_ARGS[@]}" \
    -v "${YOLO4_DIR}:/usr/share/yolo4-data" "$@" "${IMAGE}" darknetxx-gui
