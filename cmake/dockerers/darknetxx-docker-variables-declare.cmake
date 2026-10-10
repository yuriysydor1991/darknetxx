cmake_minimum_required(VERSION 3.13)

string(TOLOWER ${DARKNETXX_BINARY_NAME} DARKNETXX_BINARY_NAME_lower)

set(
  DOCKER_HOST_ADDRESS
  "127.0.0.1"
  CACHE STRING "The docker service machine address"
)

set(
  DOCKER_HOST_PORT
  "2375"
  CACHE STRING "The docker service machine port"
)

set(
  DOCKER_HOST_STR
  "tcp://${DOCKER_HOST_ADDRESS}:${DOCKER_HOST_PORT}"
  CACHE STRING "The docker service machine full address"
)

set(
  DOCKER_SINGLE_RUN_NAME
  "${DARKNETXX_BINARY_NAME_lower}-docker-single-run"
)

set(
  DOCKERFILE_SRC 
  "${PROJECT_SOURCE_DIR}/misc/Dockerfiles/Dockerfile.in"
  CACHE STRING 
  "The Dockerfile full path"
)

set(
  DOCKERFILE_DST
  "Dockerfile"
  CACHE STRING 
  "The Dockerfile destination name"
)

set(
  DOCKER_GUI_TRAINING_ARGS
  ""
  CACHE STRING
  "The darknetxx-train-gui command line arguments (a semicolon separated list) of the docker GUI trainings, the --map one for example"
)

# The trainings and the predictions of the images run as their darknetxx user
# of these ids, the ones of the configuring user by default, so the host user
# owns the backups, the charts, the predictions and the logs they write into
# the mounted folders.
execute_process(COMMAND id -u OUTPUT_VARIABLE DOCKER_HOST_UID OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND id -g OUTPUT_VARIABLE DOCKER_HOST_GID OUTPUT_STRIP_TRAILING_WHITESPACE)

set(DOCKER_USER_UID "${DOCKER_HOST_UID}" CACHE STRING "The user id the docker containers run as")
set(DOCKER_USER_GID "${DOCKER_HOST_GID}" CACHE STRING "The group id the docker containers run as")

set(
  DOCKER_USER_BUILD_ARGS
  --build-arg USER_UID=${DOCKER_USER_UID}
  --build-arg USER_GID=${DOCKER_USER_GID}
)

message(STATUS "Docker executable: ${DOCKER_EXEC}")
message(STATUS "Dockerfile configure source full path: ${DOCKERFILE_SRC}")
message(STATUS "Dockerfile destination name: ${DOCKERFILE_DST}")
message(STATUS "Docker single run instance name: ${DOCKER_SINGLE_RUN_NAME}")
message(STATUS "Docker containers user and group ids: ${DOCKER_USER_UID}:${DOCKER_USER_GID}")
message(STATUS "Docker host address and port: ${DOCKER_HOST_STR}")
message(STATUS "Project current branch name: ${PROJECT_CURRENT_BRANCH_NAME}")
