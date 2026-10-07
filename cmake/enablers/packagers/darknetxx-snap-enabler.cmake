cmake_minimum_required(VERSION 3.13)

option(
  ENABLE_SNAP
  "Set to ON to enable the snap package creation from the project build artifacts"
  OFF
)

if(NOT ENABLE_SNAP)
    return()
endif()

set(DARKNETXX_SNAP_GRADE "devel" CACHE STRING "The snap package grade")
set(DARKNETXX_SNAP_CONFINEMENT "strict" CACHE STRING "The snap package confinement")

# The snap command of the network weights files converter binary, when the
# project builds one (see the src/darknetxxConverter-target.cmake).
set(DARKNETXX_SNAP_CONVERTER_APP "")
if(ENABLE_DARKNETXX_CONVERTER)
  string(CONCAT DARKNETXX_SNAP_CONVERTER_APP
    "  ${DARKNETXX_CONVERTER_BINARY_NAME}:\n"
    "    command: usr/${DARKNETXX_BINARY_INSTALLATION_DIR}/${DARKNETXX_CONVERTER_BINARY_NAME}\n"
    "    plugs:\n"
    "      - home\n"
  )
endif()

# The snap command of the GUI binary, when the project builds one (see the
# src/darknetxx-gui-target.cmake): the GNOME extension brings the GTK 4 runtime
# and the desktop, the wayland, the x11 and the opengl plugs it shows the
# window with.
set(DARKNETXX_SNAP_GUI_APP "")
set(DARKNETXX_SNAP_GUI_BUILD_PACKAGE "")
set(DARKNETXX_SNAP_GUI_STAGE_PACKAGE "")
if(ENABLE_DARKNETXX_GUI)
  string(CONCAT DARKNETXX_SNAP_GUI_APP
    "  ${DARKNETXX_GUI_BINARY_NAME}:\n"
    "    command: usr/${DARKNETXX_BINARY_INSTALLATION_DIR}/${DARKNETXX_GUI_BINARY_NAME}\n"
    "    desktop: usr/share/applications/${DARKNETXX_APPLICATION_ID}.desktop\n"
    "    extensions: [gnome]\n"
    "    plugs:\n"
    "      - home\n"
  )
  set(DARKNETXX_SNAP_GUI_BUILD_PACKAGE "      - libgtkmm-4.0-dev")
  set(DARKNETXX_SNAP_GUI_STAGE_PACKAGE "      - libgtkmm-4.0-0")
endif()

configure_file(
  ${PROJECT_SOURCE_DIR}/misc/packagers/snapcraft.yaml.in
  ${CMAKE_CURRENT_BINARY_DIR}/snap/snapcraft.yaml
)

find_program(SNAPCRAFT_EXEC snapcraft)

add_custom_target(
  snap
  COMMAND ${SNAPCRAFT_EXEC} pack
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Packing project with help of the snapcraft"
)
