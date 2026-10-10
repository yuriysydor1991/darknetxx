#!/bin/bash -e
#
# Runs the darknetxx-train-gui of the docker image on the display of the host:
# the X11 one (allowed to the local containers while the run lasts) and the
# Wayland one of the session, when there is one, as the darknetxx user of the
# image, the one of the host user ids. The container gets every CPU core and
# the whole memory of the docker host, the training every core unless the
# darknetxx-train-gui arguments tell the --threads.
#
#   gui-run.sh <image> <YOLO4 folder> [docker run arguments...]
#              [-- darknetxx-train-gui arguments...]

IMAGE="$1"
YOLO4_DIR="$2"
shift 2

DOCKER_ARGS=()

while [[ $# -gt 0 ]] && [[ "$1" != "--" ]] ; do
    DOCKER_ARGS+=("$1")
    shift
done

# the rest are the darknetxx-train-gui arguments
if [[ $# -gt 0 ]] ; then
    shift
fi

CPUS=$(docker info --format '{{.NCPU}}')
MEMORY=$(docker info --format '{{.MemTotal}}')

# every core trains, unless the darknetxx-train-gui arguments tell the threads
THREADS=(--threads "${CPUS}")

if [[ " $* " == *" --threads "* ]] ; then
    THREADS=()
fi

DISPLAY_ARGS=(-e DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix)

# into the XDG_RUNTIME_DIR of the darknetxx user of the image, the one of the ids
# of the host user
if [[ -n "${WAYLAND_DISPLAY:-}" ]] && [[ -n "${XDG_RUNTIME_DIR:-}" ]] ; then
    DISPLAY_ARGS+=(
        -e WAYLAND_DISPLAY
        -v "${XDG_RUNTIME_DIR}/${WAYLAND_DISPLAY}:/run/user/$(id -u)/${WAYLAND_DISPLAY}"
    )
fi

if [[ -n "${DISPLAY:-}" ]] ; then
    xhost +local:docker
    trap 'xhost -local:docker' EXIT
fi

# the software renderer, since the container has none of the host GPU drivers
docker run --rm --ipc=host --cpus "${CPUS}" --memory "${MEMORY}" \
    -e GSK_RENDERER=cairo "${DISPLAY_ARGS[@]}" \
    -v "${YOLO4_DIR}:/usr/share/yolo4-data" "${DOCKER_ARGS[@]}" "${IMAGE}" \
    darknetxx-train-gui "${THREADS[@]}" "$@"
