cmake_minimum_required(VERSION 3.13)

if (NOT ENABLE_DOCKER_DARKNET_PREDICTOR)
  return()
endif()

set(
  DARKNET_CPU_PREDICTOR_DOCKERFILE_SRC
  "${PROJECT_SOURCE_DIR}/misc/Dockerfiles/Dockerfile.Darknet-CPU-predictor.in"
  CACHE STRING "The darknet training docker targets"
)

set(
  DARKNET_CPU_PREDICTOR_DOCKERFILE_DST
  "${PROJECT_BINARY_DIR}/Dockerfile.Darknet-CPU-predictor"
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

set(
  HOST_RESOURCES_IMAGES
  ""
  CACHE STRING
  "Host images dir to predict on, mounted into the predictor container"
)

if(HOST_RESOURCES_IMAGES STREQUAL "")
  message(FATAL_ERROR "No predict host resources images dir provided")
endif()

set(
  DARKNET_ORIG_SOURCE_DIR
  ""
  CACHE PATH
  "The original AlexeyAB Darknet checkout to build the comparison image of"
)

if(NOT IS_DIRECTORY "${DARKNET_ORIG_SOURCE_DIR}")
  message(FATAL_ERROR "No original Darknet checkout dir provided (DARKNET_ORIG_SOURCE_DIR)")
endif()

if(YOLO4_TRAINING_DATA_DIR STREQUAL "")
  message(FATAL_ERROR "No YOLO4 training data dir provided")
endif()

configure_file(${DARKNET_CPU_PREDICTOR_DOCKERFILE_SRC} ${DARKNET_CPU_PREDICTOR_DOCKERFILE_DST})

set(
  DARKNET_CPU_PREDICTOR_DOCKER_SINGLE_RUN_NAME
  "${DARKNETXX_BINARY_NAME_lower}-darknet-cpu-predictor-docker"
)

# Insert next into command to expand the traces
#--progress=plain 
# Write this if apt can't install packages anymore
# --no-cache
set(
  DARKNET_CPU_PREDICTOR_DOCKER_SINGLE_BUILD_CMD
    DOCKER_HOST=${DOCKER_HOST_STR} DOCKER_BUILDKIT=1 ${DOCKER_EXEC} build
      -f "${DARKNET_CPU_PREDICTOR_DOCKERFILE_DST}"
      --build-context project=${PROJECT_SOURCE_DIR}
      --build-context darknetmod=${DARKNET_ORIG_SOURCE_DIR}
      --build-arg CACHEBUST="${PROJECT_CONFIGURE_DATE}" 
      ${DOCKER_USER_BUILD_ARGS}
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
    -v ${HOST_RESOURCES_IMAGES}:/var/share/images:ro
    -v ${YOLO4_137_PRESET_DATA_FILE}:/usr/share/yolov4.conv.137:ro
    -v ${YOLO4_TINY_PRESET_DATA_FILE}:/usr/share/yolov4-tiny.conv.29:ro
    -v ${YOLO4_TRAINING_DATA_DIR}:/usr/share/yolo4-data
    ${DARKNET_CPU_PREDICTOR_DOCKER_SINGLE_RUN_NAME} &&
    xhost -local:docker
)

message(STATUS "docker darkent CPU single build command: ${DARKNET_CPU_PREDICTOR_DOCKER_SINGLE_BUILD_CMD}")
message(STATUS "docker darkent CPU single run command: ${DARKNET_CPU_PREDICTOR_DOCKER_SINGLE_RUN_CMD}")

add_custom_target(
  docker-single-build-darknet-cpu-predict
  COMMAND ${DARKNET_CPU_PREDICTOR_DOCKER_SINGLE_BUILD_CMD}
  COMMENT "Building the docker single run image ${DARKNET_CPU_PREDICTOR_DOCKER_SINGLE_RUN_NAME}"
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_custom_target(
  docker-single-run-darknet-cpu-predict
  COMMAND ${DARKNET_CPU_PREDICTOR_DOCKER_SINGLE_RUN_CMD}
  DEPENDS docker-single-build-darknet-cpu-predict
  COMMENT "Running the docker single run image ${DARKNET_CPU_PREDICTOR_DOCKER_SINGLE_RUN_NAME}"
)
