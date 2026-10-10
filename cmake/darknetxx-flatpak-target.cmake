cmake_minimum_required(VERSION 3.13)

find_program(FLATPAKB_EXEC flatpak-builder REQUIRED)
find_program(FLATPAK_EXEC flatpak REQUIRED)

set(FLATPAK_PROJECT_URL ${DARKNETXX_APPLICATION_ID})

# The GUI training binary is the one the package runs, when it is built. The
# desktop entries of both GUI binaries are the package ones, since their names
# start with the package id, so the desktops start the recognition one too.
set(FLATPAK_COMMAND ${DARKNETXX_BINARY_NAME})

if(ENABLE_DARKNETXX_GUI)
  set(FLATPAK_COMMAND ${DARKNETXX_TRAIN_GUI_BINARY_NAME})
endif()
set(FLATPAK_REPO ${DARKNETXX_BINARY_NAME}-repo)
set(FLATPAK_DST_NAME ${DARKNETXX_BINARY_NAME}-${PROJECT_VERSION}.flatpak)
set(flatpakConfDst ${PROJECT_BINARY_DIR}/flatpak.conf.json)
set(
  FLATPAK_CONF_SRC 
  ${PROJECT_SOURCE_DIR}/misc/packagers/flatpak.conf.json.in
  CACHE STRING "The flatpak json conf source file cmake configured"
)

configure_file(${FLATPAK_CONF_SRC} ${flatpakConfDst})

message(STATUS "flatpak-builder: ${FLATPAKB_EXEC}")
message(STATUS "flatpak: ${FLATPAK_EXEC}")
message(STATUS "flatpak json conf src: ${FLATPAK_CONF_SRC}")

add_custom_target(
  flatpak
  COMMAND 
    ${FLATPAKB_EXEC} --repo=${FLATPAK_REPO} "${PROJECT_BINARY_DIR}/flatpak-build" ${flatpakConfDst} &&
    ${FLATPAK_EXEC} build-bundle ${FLATPAK_REPO} ${FLATPAK_DST_NAME} ${FLATPAK_PROJECT_URL}
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  COMMENT "Executing the flatpak-builder command to generate the ${FLATPAK_DST_NAME} flatpak package."
)
