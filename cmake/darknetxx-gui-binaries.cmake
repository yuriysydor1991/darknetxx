cmake_minimum_required(VERSION 3.13)

# ---------------------------------------------------------------------------
# The GUI binaries the build makes, the GTKmm 4 ones of the
# DARKNETXX_GTKMM_GUIS list and the Qt 6 ones of the DARKNETXX_QT_GUIS one,
# which the targets (see the src/darknetxx-gui-targets.cmake), the desktop
# entries and the packagers take. Every GUI of the lists has the variables
#
#   DARKNETXX_GUI_<GUI>_BINARY      the binary name and target
#   DARKNETXX_GUI_<GUI>_ID          the application id: the D-Bus name and the
#                                   name of the desktop entry and the icon
#   DARKNETXX_GUI_<GUI>_DEFINITION  the definition the app component of it is
#                                   compiled with (see the
#                                   app::ApplicationFactory)
#   DARKNETXX_GUI_<GUI>_GENERIC     the generic name of the desktop entry
#   DARKNETXX_GUI_<GUI>_COMMENT     the comment of the desktop entry
# ---------------------------------------------------------------------------
set(DARKNETXX_GTKMM_GUIS "")
set(DARKNETXX_QT_GUIS "")

if(ENABLE_DARKNETXX_EXECUTABLES)
  if(ENABLE_DARKNETXX_GUI AND NOT DARKNETXX_NET_EDITOR_ONLY)
    list(APPEND DARKNETXX_GTKMM_GUIS Train Recognition)
  endif()

  if(ENABLE_DARKNETXX_GUI AND ENABLE_DARKNETXX_NET_EDITOR)
    list(APPEND DARKNETXX_GTKMM_GUIS NetEditor)
  endif()

  if(ENABLE_DARKNETXX_NET_EDITOR AND ENABLE_DARKNETXX_NET_EDITOR_QT)
    list(APPEND DARKNETXX_QT_GUIS NetEditorQt)
  endif()
endif()

set(DARKNETXX_GUI_Train_BINARY ${DARKNETXX_TRAIN_GUI_BINARY_NAME})
set(DARKNETXX_GUI_Train_ID ${DARKNETXX_APPLICATION_ID})
set(DARKNETXX_GUI_Train_DEFINITION DARKNETXX_TRAIN_GUI)
set(DARKNETXX_GUI_Train_GENERIC "Neural network training")
set(DARKNETXX_GUI_Train_COMMENT "Trains the network of a YOLO4 folder")

set(DARKNETXX_GUI_Recognition_BINARY ${DARKNETXX_RECOGNITION_GUI_BINARY_NAME})
set(DARKNETXX_GUI_Recognition_ID ${DARKNETXX_RECOGNITION_APPLICATION_ID})
set(DARKNETXX_GUI_Recognition_DEFINITION DARKNETXX_RECOGNITION_GUI)
set(DARKNETXX_GUI_Recognition_GENERIC "Neural network recognition")
set(
  DARKNETXX_GUI_Recognition_COMMENT
  "Detects the objects in the images or classifies them by the network of a weights file"
)

set(DARKNETXX_GUI_NetEditor_BINARY ${DARKNETXX_NET_EDITOR_GUI_BINARY_NAME})
set(DARKNETXX_GUI_NetEditor_ID ${DARKNETXX_NET_EDITOR_APPLICATION_ID})
set(DARKNETXX_GUI_NetEditor_DEFINITION DARKNETXX_NET_EDITOR_GUI)
set(DARKNETXX_GUI_NetEditor_GENERIC "Neural network editor")
set(
  DARKNETXX_GUI_NetEditor_COMMENT
  "Creates the network of a dxxwjz3p project or edits the one of the project"
)

set(DARKNETXX_GUI_NetEditorQt_BINARY ${DARKNETXX_NET_EDITOR_QT_GUI_BINARY_NAME})
set(DARKNETXX_GUI_NetEditorQt_ID ${DARKNETXX_NET_EDITOR_QT_APPLICATION_ID})
set(DARKNETXX_GUI_NetEditorQt_DEFINITION DARKNETXX_NET_EDITOR_QT_GUI)
set(DARKNETXX_GUI_NetEditorQt_GENERIC "Neural network editor (Qt)")
set(DARKNETXX_GUI_NetEditorQt_COMMENT ${DARKNETXX_GUI_NetEditor_COMMENT})

# the network editors of the Font Awesome icons
set(DARKNETXX_NET_EDITOR_GUIS ${DARKNETXX_GTKMM_GUIS} ${DARKNETXX_QT_GUIS})
list(FILTER DARKNETXX_NET_EDITOR_GUIS INCLUDE REGEX "^NetEditor")
