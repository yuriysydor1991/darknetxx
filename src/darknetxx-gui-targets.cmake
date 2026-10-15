cmake_minimum_required(VERSION 3.13)

# ---------------------------------------------------------------------------
# The GUI binaries: the works of the darknetxx library behind the windows of
# the src/gui component, the training of the YOLO4 folder chosen in the
# window of the training one, the recognition of the image chosen in the
# window of the recognition one and the network of the dxxwjz3p project the
# network editor edits (all of them of the GTKmm 4).
#
# Each is built out of the very same components as the main binary plus the
# src/gui one, and their app::TrainGuiApplication,
# app::RecognitionGuiApplication and app::NetEditorGuiApplication classes show
# the windows instead of starting the work the command line names. The
# DARKNETXX_TRAIN_GUI, the DARKNETXX_RECOGNITION_GUI and the
# DARKNETXX_NET_EDITOR_GUI definitions below tell the app::ApplicationFactory
# which one to create and the help and the version
# printers which binary to name. The cmake/darknetxx-gui-binaries.cmake tells
# the binaries of the build (the DARKNETXX_GTKMM_GUIS and the
# DARKNETXX_QT_GUIS lists).
# ---------------------------------------------------------------------------
set(DARKNETXX_GUI_TARGETS "")
set(DARKNETXX_GTKMM_GUI_TARGETS "")
set(DARKNETXX_NET_EDITOR_TARGETS "")

if(NOT DARKNETXX_GTKMM_GUIS AND NOT DARKNETXX_QT_GUIS)
  message(STATUS "The GUI binaries are excluded (ENABLE_DARKNETXX_GUI=OFF)")
  return()
endif()

# the Darknet logo of the binaries and of their shortcuts on the MS Windows
if(WIN32)
  set(DARKNETXX_GUI_RC "${PROJECT_BINARY_DIR}/darknetxx-gui.rc")
  configure_file(
    "${PROJECT_SOURCE_DIR}/misc/desktop/darknetxx-gui.rc.in"
    "${DARKNETXX_GUI_RC}"
    @ONLY
  )
endif()

foreach(GUI IN LISTS DARKNETXX_GTKMM_GUIS DARKNETXX_QT_GUIS)
  set(GUI_BINARY_NAME ${DARKNETXX_GUI_${GUI}_BINARY})

  darknetxx_add_app_target(${GUI_BINARY_NAME})

  target_compile_definitions(
    ${GUI_BINARY_NAME}
    PRIVATE ${DARKNETXX_GUI_${GUI}_DEFINITION}
  )

  target_sources(
    ${GUI_BINARY_NAME}
    PRIVATE
      ${CMAKE_CURRENT_LIST_DIR}/app/applications/GuiApplication.cpp
      ${CMAKE_CURRENT_LIST_DIR}/app/applications/${GUI}GuiApplication.cpp
      ${DARKNETXX_GUI_RC}
  )

  list(APPEND DARKNETXX_APP_TARGETS ${GUI_BINARY_NAME})
  list(APPEND DARKNETXX_GUI_TARGETS ${GUI_BINARY_NAME})

  if(GUI IN_LIST DARKNETXX_GTKMM_GUIS)
    list(APPEND DARKNETXX_GTKMM_GUI_TARGETS ${GUI_BINARY_NAME})
  endif()

  if(GUI IN_LIST DARKNETXX_NET_EDITOR_GUIS)
    list(APPEND DARKNETXX_NET_EDITOR_TARGETS ${GUI_BINARY_NAME})
  endif()
endforeach()
