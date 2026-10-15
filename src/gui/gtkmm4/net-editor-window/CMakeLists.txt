cmake_minimum_required(VERSION 3.13)

# the window of the network editor over the model of the src/gui/net-editor
add_library(
  DarknetXXGtkmm4NetEditorWindowOBJ OBJECT
  GtkmmIcons.cpp
  LayerRow.cpp
  NetEditorGtkmmIniter.cpp
  NetEditorWindowContext.cpp
  NetEditorWindowLoader.cpp
  NetworkEventsHandler.cpp
  OptionRow.cpp
  OptionsGrid.cpp
  ProjectEventsHandler.cpp
)

target_include_directories(
  DarknetXXGtkmm4NetEditorWindowOBJ
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
)

target_link_libraries(
  DarknetXXGtkmm4NetEditorWindowOBJ
  PRIVATE PkgConfig::GTKMM
)

# the Font Awesome icons of the controls, the icon theme ones of none
if(TARGET DarknetXXFontAwesomeGResourceOBJ)
  target_compile_definitions(
    DarknetXXGtkmm4NetEditorWindowOBJ
    PRIVATE DARKNETXX_FONTAWESOME
  )

  target_link_libraries(
    ${DARKNETXX_NET_EDITOR_GUI_BINARY_NAME}
    DarknetXXFontAwesomeGResourceOBJ
  )
endif()

target_link_libraries(
  ${DARKNETXX_NET_EDITOR_GUI_BINARY_NAME}
  DarknetXXGtkmm4NetEditorWindowOBJ
)
