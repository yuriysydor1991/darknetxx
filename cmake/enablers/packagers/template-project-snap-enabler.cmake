cmake_minimum_required(VERSION 3.13)

option(
  ENABLE_SNAP
  "Set to ON to enable the snap package creation from the project build artifacts"
  OFF
)

if(NOT ENABLE_SNAP)
    return()
endif()

set(TEMPLATE_PROJECT_SNAP_GRADE "devel" CACHE STRING "The snap package grade")
set(TEMPLATE_PROJECT_SNAP_CONFINEMENT "strict" CACHE STRING "The snap package confinement")

# The snap command of the network weights files converter binary, when the
# project builds one (see the src/darknetxxConverter-target.cmake).
set(TEMPLATE_PROJECT_SNAP_CONVERTER_APP "")
if(ENABLE_DARKNETXX_CONVERTER)
  string(CONCAT TEMPLATE_PROJECT_SNAP_CONVERTER_APP
    "  ${PROJECT_CONVERTER_BINARY_NAME}:\n"
    "    command: usr/${PROJECT_BINARY_INSTALLATION_DIR}/${PROJECT_CONVERTER_BINARY_NAME}\n"
    "    plugs:\n"
    "      - home\n"
  )
endif()

configure_file(
  ${CMAKE_SOURCE_DIR}/misc/packagers/snapcraft.yaml.in
  ${CMAKE_CURRENT_BINARY_DIR}/snap/snapcraft.yaml
)

find_program(SNAPCRAFT_EXEC snapcraft)

add_custom_target(
  snap
  COMMAND ${SNAPCRAFT_EXEC} pack
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Packing project with help of the snapcraft"
)
