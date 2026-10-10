## Enabling the Docker container build and run

To be able to build and run the Docker container with current template project' target binary file enable it's CMake target with setting the `ENABLE_DOCKER` variable into value `ON` by using the next command:

```
# inside the project root directory

cmake -S . -B build -DENABLE_DOCKER=ON
```

After that there should be available the Docker build and run CMake target `docker-single-run` which may be triggered with the command:

```
# inside the project root directory

cmake --build build --target docker-single-run 
```

Examine the `cmake/dockerers/darknetxx-docker-single-runner-target.cmake` file for the `docker-single-run` target details.

### Starter scripts

The [scripts/docker](/scripts/docker) directory holds the shortcuts which perform both commands above, so the whole run takes a single command:

| Script | Starts the CMake target |
| --- | --- |
| [single-run.sh](/scripts/docker/single-run.sh) | `docker-single-run` |
| [single-build.sh](/scripts/docker/single-build.sh) | `docker-single-build` |
| [cpu-train-gui.sh](/scripts/docker/cpu-train-gui.sh) | `docker-single-run-darknetxx-cpu-train-gui` of the YOLO4 folder of it's first argument (see the [The darknetxx-train-gui binary](/doc/sections/en_US/5-36-the-GUI-binaries.md#the-darknetxx-train-gui-binary) section) |

```
# inside the project root directory

scripts/docker/single-run.sh
```

Each of them configures the `build/release` directory with the `ENABLE_DOCKER` option set to `ON` and then builds the target of interest, so no `docker` command of its own is ever issued. The `docker-single-run` CMake target depends on the `docker-single-build` one, which means the [single-run.sh](/scripts/docker/single-run.sh) script builds the image as well and the [single-build.sh](/scripts/docker/single-build.sh) one is only needed to build the image without starting a container.

The scripts accept the very same parameters as the [Quick build scripts](/doc/sections/en_US/5-34-quick-build-scripts.md) ones, since they are their callers:

```
# inside the project root directory

# reuses the already configured build/release directory
scripts/docker/single-run.sh --no-reconfigure

# the local Docker daemon socket instead of the default tcp://127.0.0.1:2375 address
scripts/docker/single-run.sh -DDOCKER_HOST_STR=unix:///var/run/docker.sock
```

Keep in mind that the `ENABLE_DOCKER` option makes the configure step require the `docker` executable to be present in the system, so the scripts fail the configure step on a machine with no Docker installed (see the [Optional for the Docker container runs](/doc/sections/en_US/3-10-optional-for-docker-container-runs.md) requirements section).

### The container user

The training and the prediction images (the `ENABLE_DOCKER_DARKNETXX_CPU_TRAIN`, the `ENABLE_DOCKER_DARKNETXX_GPU_TRAIN`, the `ENABLE_DOCKER_DARKNETXX_PREDICTOR` and the original Darknet `ENABLE_DOCKER_DARKNET` and `ENABLE_DOCKER_DARKNET_PREDICTOR` ones) run their containers as the regular `darknetxx` user instead of the `root` one, the command line trainings and the [darknetxx-train-gui](/doc/sections/en_US/5-36-the-GUI-binaries.md#the-darknetxx-train-gui-binary) ones alike. The image gives that user the ids of the user who configures the project (the `id -u` and the `id -g` ones), so the backups, the charts, the predictions and the logs the containers write into the mounted YOLO4 folder belong to the host user, who changes or removes them with no `sudo`. The `DOCKER_USER_UID` and the `DOCKER_USER_GID` CMake variables name other ids:

```
# inside the project root directory

cmake -S . -B build -DENABLE_DOCKER=ON -DENABLE_DOCKER_DARKNETXX_CPU_TRAIN=ON \
  -DYOLO4_TRAINING_DATA_DIR=/path/to/the/yolo4/folder \
  -DDOCKER_USER_UID=1001 -DDOCKER_USER_GID=1001
```

The ids are the `USER_UID` and the `USER_GID` build arguments of the image, so it is built for the very user it serves. The `ubuntu` user of the Ubuntu 24.04 base images gives it's `1000` ids away, while the other accounts and groups of the very same ids share them with the `darknetxx` one. The mounted YOLO4 folder has to be the host user one: Docker creates an absent folder as the `root` one, which the `darknetxx` user can't write into. The original Darknet recipe of the [Dockerfile.cpu](/misc/Dockerfiles/Dockerfile.cpu), which no CMake target builds, takes the same build arguments for it's `yolo` user.
