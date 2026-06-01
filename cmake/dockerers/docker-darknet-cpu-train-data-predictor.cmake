cmake_minimum_required(VERSION 3.13)

if (NOT ENABLE_DOCKER_DARKNET_PREDICTOR)
  return()
endif()

set(
  DARKNET_CPU_PREDICTOR_DOCKERFILE_SRC
  "${CMAKE_SOURCE_DIR}/misc/Dockerfiles/Dockerfile.Darknet-CPU-predictor.in"
  CACHE STRING "The darknet training docker targets"
)

set(
  DARKNET_CPU_PREDICTOR_DOCKERFILE_DST
  "${CMAKE_BINARY_DIR}/Dockerfile.Darknet-CPU-predictor"
)

set(
  YOLO4_TRAINING_DATA_DIR
  ""
  CACHE STRING
  "Host training resources in YOLO4 format data dir"
)

set(
  YOLO4_137_PRESET_DATA_FILE
  "/data/lib/pkg/NeuralNets/yolov4.conv.137"
  CACHE STRING "The yolov4.conv.137 file location"
)

set(
  YOLO4_TINY_PRESET_DATA_FILE
  "/data/lib/pkg/NeuralNets/yolov4-tiny.conv.29"
  CACHE STRING "The yolov4-tiny.conv.29 file location"
)

if(YOLO4_TRAINING_DATA_DIR STREQUAL "")
  message(FATAL_ERROR "No YOLO4 training data dir provided")
endif()

configure_file(${DARKNET_CPU_PREDICTOR_DOCKERFILE_SRC} ${DARKNET_CPU_PREDICTOR_DOCKERFILE_DST})

set(
  DARKNET_CPU_PREDICTOR_DOCKER_SINGLE_RUN_NAME
  "${PROJECT_BINARY_NAME_lower}-darknet-cpu-train-docker"
)

# Insert next into command to expand the traces
#--progress=plain 
# Write this if apt can't install packages anymore
# --no-cache
set(
  DARKNET_CPU_PREDICTOR_DOCKER_SINGLE_BUILD_CMD
    DOCKER_HOST=${DOCKER_HOST_STR} DOCKER_BUILDKIT=1 ${DOCKER_EXEC} build
      -f "${DARKNET_CPU_PREDICTOR_DOCKERFILE_DST}"
      --build-context project=${CMAKE_SOURCE_DIR}
      --build-context darknetmod=${CMAKE_SOURCE_DIR}/src/NNs/darknet-yolo
      --build-arg CACHEBUST="${PROJECT_CONFIGURE_DATE}" 
      -t ${DARKNET_CPU_PREDICTOR_DOCKER_SINGLE_RUN_NAME} .
)

# --device /dev/dri
# --group-add video
# to check nvidia drivers: nvidia-smi
set(
  DARKNET_CPU_PREDICTOR_DOCKER_SINGLE_RUN_CMD
    xhost +local:docker &&
    DOCKER_HOST=${DOCKER_HOST_STR} 
    ${DOCKER_EXEC} run --rm -t
    --security-opt apparmor=unconfined
    --ipc=host
    -e DISPLAY=$$DISPLAY
    -v /tmp/.X11-unix:/tmp/.X11-unix
    -e DBUS_SESSION_BUS_ADDRESS=$(DBUS_SESSION_BUS_ADDRESS)
    -v $(XDG_RUNTIME_DIR)/bus:$(XDG_RUNTIME_DIR)/bus
    -v $(XDG_RUNTIME_DIR)/at-spi/bus_0:$(XDG_RUNTIME_DIR)/at-spi/bus_0
    -e XDG_RUNTIME_DIR=$(XDG_RUNTIME_DIR)
    --device /dev/dri
    --group-add video
    --gpus all
    --runtime=nvidia
    -v ${HOST_RESOURCES_IMAGES}:/var/share/images:ro
    -v ${YOLO4_137_PRESET_DATA_FILE}:/usr/share/yolov4.conv.137:ro
    -v ${YOLO4_TINY_PRESET_DATA_FILE}:/usr/share/yolov4-tiny.conv.29:ro
    -v ${YOLO4_TRAINING_DATA_DIR}:/usr/share/yolo4-data
    ${DARKNET_CPU_PREDICTOR_DOCKER_SINGLE_RUN_NAME} &&
    xhost -local:docker
)

message(STATUS "docker darkent CPU single build command: ${DOCKER_CPU_SINGLE_BUILD_CMD}")
message(STATUS "docker darkent CPU single run command: ${DOCKER_CPU_SINGLE_RUN_CMD}")

add_custom_target(
  docker-single-build-darknet-cpu-predict
  COMMAND ${DARKNET_CPU_PREDICTOR_DOCKER_SINGLE_BUILD_CMD}
  COMMENT "Building the docker single run image ${DARKNET_CPU_TRAIN_DOCKER_SINGLE_RUN_NAME}"
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_custom_target(
  docker-single-run-darknet-cpu-predict
  COMMAND ${DARKNET_CPU_PREDICTOR_DOCKER_SINGLE_RUN_CMD}
  DEPENDS docker-single-build-darknet-cpu-predict
  COMMENT "Running the docker single run image ${DARKNET_CPU_TRAIN_DOCKER_SINGLE_RUN_NAME}"
)
