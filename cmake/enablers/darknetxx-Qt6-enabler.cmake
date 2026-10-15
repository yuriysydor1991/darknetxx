cmake_minimum_required(VERSION 3.13)

# ---------------------------------------------------------------------------
# The Qt 6 toolkit of the Qt network editor binary (see the src/gui/qt6
# directory): the widgets and the SVG rendering of the Font Awesome icons of
# it's controls.
#
# The ENABLE_DARKNETXX_NET_EDITOR_QT option is ON by default once the Qt 6 is
# found, so the build of no Qt 6 goes on without the binary, while the option
# turned ON requires the Qt 6.
# ---------------------------------------------------------------------------
if(NOT ENABLE_DARKNETXX_EXECUTABLES OR NOT ENABLE_DARKNETXX_NET_EDITOR)
  return()
endif()

set(DARKNETXX_QT6_COMPONENTS Widgets Svg)

if(NOT DEFINED ENABLE_DARKNETXX_NET_EDITOR_QT)
  find_package(Qt6 QUIET COMPONENTS ${DARKNETXX_QT6_COMPONENTS})
endif()

option(
  ENABLE_DARKNETXX_NET_EDITOR_QT
  "Build the Qt 6 network editor binary beside the GTKmm 4 one (ON by default once the Qt 6 Widgets and Svg are found)"
  ${Qt6_FOUND}
)

if(NOT ENABLE_DARKNETXX_NET_EDITOR_QT)
  return()
endif()

find_package(Qt6 REQUIRED COMPONENTS ${DARKNETXX_QT6_COMPONENTS})

message(STATUS "Qt 6 version: ${Qt6_VERSION}")
