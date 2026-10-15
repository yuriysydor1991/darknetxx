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

# The snap commands of the GTKmm 4 GUI binaries, when the project builds them
# (see the cmake/darknetxx-gui-binaries.cmake): the GNOME extension brings the GTK 4
# runtime and the desktop, the wayland, the x11 and the opengl plugs they show
# their windows with. The Darknet logo they install is the icon of the snap,
# which the snapcraft puts into their desktop entries as well.
set(DARKNETXX_SNAP_ICON "")
set(DARKNETXX_SNAP_GUI_APP "")
set(DARKNETXX_SNAP_GUI_BUILD_PACKAGE "")
set(DARKNETXX_SNAP_GUI_STAGE_PACKAGE "")
if(DARKNETXX_GTKMM_GUIS)
  list(GET DARKNETXX_GTKMM_GUIS 0 GUI)
  set(DARKNETXX_SNAP_ICON
    "icon: usr/share/icons/hicolor/256x256/apps/${DARKNETXX_GUI_${GUI}_ID}.png")

  # the GTKmm 4 ones alone, the extension brings no Qt 6
  foreach(GUI IN LISTS DARKNETXX_GTKMM_GUIS)
    string(APPEND DARKNETXX_SNAP_GUI_APP
      "  ${DARKNETXX_GUI_${GUI}_BINARY}:\n"
      "    command: usr/${DARKNETXX_BINARY_INSTALLATION_DIR}/${DARKNETXX_GUI_${GUI}_BINARY}\n"
      "    desktop: usr/share/applications/${DARKNETXX_GUI_${GUI}_ID}.desktop\n"
      "    extensions: [gnome]\n"
      "    plugs:\n"
      "      - home\n"
    )
  endforeach()
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
