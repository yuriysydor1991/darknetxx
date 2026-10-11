cmake_minimum_required(VERSION 3.13)

# ---------------------------------------------------------------------------
# The GUI binaries: the works of the darknetxx library behind the GTKmm 4
# windows of the src/gui component, the training of the YOLO4 folder chosen
# in the window of the training one and the recognition of the image chosen
# in the window of the recognition one.
#
# Both are built out of the very same components as the main binary plus the
# src/gui one, and their app::TrainGuiApplication and the
# app::RecognitionGuiApplication classes show the windows instead of starting
# the work the command line names. The DARKNETXX_TRAIN_GUI and the
# DARKNETXX_RECOGNITION_GUI definitions below tell the app::ApplicationFactory
# which one to create and the help and the version printers which binary to
# name.
# ---------------------------------------------------------------------------
if(NOT ENABLE_DARKNETXX_GUI)
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

foreach(GUI Train Recognition)
  string(TOUPPER ${GUI} GUI_ID)
  set(GUI_BINARY_NAME ${DARKNETXX_${GUI_ID}_GUI_BINARY_NAME})

  darknetxx_add_app_target(${GUI_BINARY_NAME})

  target_compile_definitions(
    ${GUI_BINARY_NAME}
    PRIVATE DARKNETXX_${GUI_ID}_GUI
  )

  target_sources(
    ${GUI_BINARY_NAME}
    PRIVATE
      ${CMAKE_CURRENT_LIST_DIR}/app/applications/GuiApplication.cpp
      ${CMAKE_CURRENT_LIST_DIR}/app/applications/${GUI}GuiApplication.cpp
      ${DARKNETXX_GUI_RC}
  )

  list(APPEND DARKNETXX_APP_TARGETS ${GUI_BINARY_NAME})
endforeach()
