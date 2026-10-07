cmake_minimum_required(VERSION 3.13)

# The desktop entry and the icon of the GUI binary, which the desktops and the
# packages show it with. Both are named after the GUI application id, as the
# Wayland compositors and the flatpak require.
if(NOT ENABLE_DARKNETXX_GUI OR NOT ENABLE_DARKNETXX_EXECUTABLES)
  return()
endif()

include(GNUInstallDirs)

set(DESKTOP_ENTRY_DST "${PROJECT_BINARY_DIR}/${DARKNETXX_APPLICATION_ID}.desktop")

configure_file(
  "${PROJECT_SOURCE_DIR}/misc/desktop/darknetxx-gui.desktop.in"
  "${DESKTOP_ENTRY_DST}"
  @ONLY
)

install(
  FILES "${DESKTOP_ENTRY_DST}"
  DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications"
)

install(
  FILES "${PROJECT_SOURCE_DIR}/misc/desktop/darknetxx-gui.svg"
  DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps"
  RENAME "${DARKNETXX_APPLICATION_ID}.svg"
)
