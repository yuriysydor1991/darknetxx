cmake_minimum_required(VERSION 3.13)

# ---------------------------------------------------------------------------
# The GUI binary: the training of the darknetxx library behind the GTKmm 4
# window of the src/gui component, which trains the YOLO4 folder chosen in it.
#
# It is built out of the very same components as the main binary plus the
# src/gui one, and it's app::GuiApplication class shows the window instead of
# starting the work the command line names. The DARKNETXX_GUI definition below
# tells the app::ApplicationFactory to create it and the help and the version
# printers to name the GUI binary.
# ---------------------------------------------------------------------------
if(NOT ENABLE_DARKNETXX_GUI)
  message(STATUS
    "The ${DARKNETXX_GUI_BINARY_NAME} target is excluded "
    "(ENABLE_DARKNETXX_GUI=OFF)")
  return()
endif()

darknetxx_add_app_target(${DARKNETXX_GUI_BINARY_NAME})

target_compile_definitions(
  ${DARKNETXX_GUI_BINARY_NAME}
  PRIVATE DARKNETXX_GUI
)

target_sources(
  ${DARKNETXX_GUI_BINARY_NAME}
  PRIVATE ${CMAKE_CURRENT_LIST_DIR}/app/GuiApplication.cpp
)

list(APPEND DARKNETXX_APP_TARGETS ${DARKNETXX_GUI_BINARY_NAME})
