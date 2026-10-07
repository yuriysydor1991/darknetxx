cmake_minimum_required(VERSION 3.13)

message(STATUS "Docker runner target enabled")

find_program(DOCKER_EXEC docker REQUIRED)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/dockerers")

include(darknetxx-docker-variables-declare)

configure_file(${DOCKERFILE_SRC} ${DOCKERFILE_DST})

include(darknetxx-docker-single-runner-target)
include(darknetxx-docker-darknet-cpu-train-data)
include(darknetxx-docker-darknet-cpu-train-data-predictor)
include(darknetxx-docker-darknetxx-cpu-train-data)
include(darknetxx-docker-darknetxx-cpu-train-data-predictor)
include(darknetxx-docker-darknetxx-gpu-train-data)

# Jenkins pipeline inside a Docker container. The module self-declares its
# ENABLE_JENKINS_DOCKER_PIPELINE option and returns early when disabled.
include(darknetxx-docker-Jenkins-pipeline-runner-target)
