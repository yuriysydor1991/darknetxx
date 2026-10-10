cmake_minimum_required(VERSION 3.13)

# The desktop entries of the GUI binaries and their icon, the Darknet logo
# (the misc/desktop/darknet-logo.png, the darknet.ico of the Darknet sources on
# a white disc), which the desktops and the packages show them with. They are
# named after the application ids of the binaries, as the Wayland compositors
# and the flatpak require.
if(NOT ENABLE_DARKNETXX_GUI OR NOT ENABLE_DARKNETXX_EXECUTABLES)
  return()
endif()

include(GNUInstallDirs)

# @brief Installs the desktop entry and the icon of the GUI binary of the
# application id.
function(darknetxx_desktop_entry GUI_BINARY_NAME GUI_APPLICATION_ID
         GUI_GENERIC_NAME GUI_COMMENT)
  set(DESKTOP_ENTRY_DST "${PROJECT_BINARY_DIR}/${GUI_APPLICATION_ID}.desktop")

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
    FILES "${PROJECT_SOURCE_DIR}/misc/desktop/darknet-logo.png"
    DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/256x256/apps"
    RENAME "${GUI_APPLICATION_ID}.png"
  )
endfunction()

darknetxx_desktop_entry(
  ${DARKNETXX_TRAIN_GUI_BINARY_NAME} ${DARKNETXX_APPLICATION_ID}
  "Neural network training" "Trains the network of a YOLO4 folder"
)

darknetxx_desktop_entry(
  ${DARKNETXX_RECOGNITION_GUI_BINARY_NAME} ${DARKNETXX_RECOGNITION_APPLICATION_ID}
  "Neural network recognition"
  "Detects the objects in the images or classifies them by the network of a weights file"
)
