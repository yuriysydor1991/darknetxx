cmake_minimum_required(VERSION 3.13)

# The Qt 6 window of the network editor over the model of the
# src/gui/net-editor, of no Qt meta objects of it's own (no moc).
add_library(
  DarknetXXQt6NetEditorWindowOBJ OBJECT
  DragHandle.cpp
  LayerRowWidget.cpp
  NetEditorMainWindow.cpp
  NetEditorQtIniter.cpp
  OptionValidator.cpp
  QtIcons.cpp
  QtOptionRow.cpp
  QtOptionsGrid.cpp
)

target_include_directories(
  DarknetXXQt6NetEditorWindowOBJ
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
)

# the emit, signals and slots names of the rest of the project stay theirs
target_compile_definitions(
  DarknetXXQt6NetEditorWindowOBJ
  PRIVATE QT_NO_KEYWORDS
)

target_link_libraries(
  DarknetXXQt6NetEditorWindowOBJ
  PRIVATE Qt6::Widgets Qt6::Svg
)

# The Darknet logo of the window and the Font Awesome icons of the controls
# (see the darknetxx-fontawesome-enabler), the icon theme ones of none.
set(QT_RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/darknetxx-qt.qrc")

if(DEFINED DARKNETXX_FONTAWESOME_SVGS_DIR)
  set(FONTAWESOME_QRC "${CMAKE_CURRENT_BINARY_DIR}/darknetxx-fontawesome.qrc")

  darknetxx_fontawesome_write_qrc("${FONTAWESOME_QRC}" "/icons")
  list(APPEND QT_RESOURCES "${FONTAWESOME_QRC}")

  target_compile_definitions(
    DarknetXXQt6NetEditorWindowOBJ
    PRIVATE DARKNETXX_FONTAWESOME
  )
endif()

qt_add_resources(QT_RESOURCE_SOURCES ${QT_RESOURCES})

# the generated code, which the project warnings are not for
set_source_files_properties(
  ${QT_RESOURCE_SOURCES}
  PROPERTIES COMPILE_OPTIONS "-w"
)

# the object files of the resources register them in the binary
target_sources(
  DarknetXXQt6NetEditorWindowOBJ
  PRIVATE ${QT_RESOURCE_SOURCES}
)

target_link_libraries(
  ${DARKNETXX_NET_EDITOR_QT_GUI_BINARY_NAME}
  DarknetXXQt6NetEditorWindowOBJ
  Qt6::Widgets
  Qt6::Svg
)

add_subdirectory(tests)
