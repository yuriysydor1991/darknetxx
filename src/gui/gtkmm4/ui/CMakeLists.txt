cmake_minimum_required(VERSION 3.13)

# The window descriptions and the Darknet logo icon of the misc/desktop embedded
# into the GUI binaries as a GResource, which registers itself on the binary
# start.
set(GUI_RESOURCE_XML "${CMAKE_CURRENT_SOURCE_DIR}/darknetxx-gui.gresource.xml")
set(GUI_RESOURCE_C "${CMAKE_CURRENT_BINARY_DIR}/darknetxx-gui-resources.c")

add_custom_command(
  OUTPUT ${GUI_RESOURCE_C}
  COMMAND ${GLIB_COMPILE_RESOURCES_EXEC}
          --sourcedir=${CMAKE_CURRENT_SOURCE_DIR}
          --sourcedir=${PROJECT_SOURCE_DIR}/misc/desktop
          --target=${GUI_RESOURCE_C}
          --generate-source
          ${GUI_RESOURCE_XML}
  DEPENDS
    ${GUI_RESOURCE_XML}
    ${CMAKE_CURRENT_SOURCE_DIR}/NetEditorWindow.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/RecognitionWindow.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/TrainWindow.ui
    ${PROJECT_SOURCE_DIR}/misc/desktop/darknet-logo.png
  COMMENT "Generating the GResource source of the GUI binaries"
)

add_library(
  DarknetXXGuiResourcesOBJ OBJECT
  ${GUI_RESOURCE_C}
)

# the generated code, which the project warnings are not for
set_target_properties(DarknetXXGuiResourcesOBJ PROPERTIES COMPILE_OPTIONS "")

target_link_libraries(
  DarknetXXGuiResourcesOBJ
  PRIVATE PkgConfig::GTKMM
)

# The Font Awesome icons of the network editor controls (see the
# darknetxx-fontawesome-enabler): the symbolic icons of the icon theme path
# the GtkmmIniter adds, which the build of no Font Awesome leaves to the icon
# theme.
if(NOT DEFINED DARKNETXX_FONTAWESOME_SVGS_DIR OR
   NOT TARGET ${DARKNETXX_NET_EDITOR_GUI_BINARY_NAME})
  return()
endif()

set(FONTAWESOME_RESOURCE_XML "${CMAKE_CURRENT_BINARY_DIR}/darknetxx-fontawesome.gresource.xml")
set(FONTAWESOME_RESOURCE_C "${CMAKE_CURRENT_BINARY_DIR}/darknetxx-fontawesome-resources.c")

darknetxx_fontawesome_write_gresource(
  "${FONTAWESOME_RESOURCE_XML}"
  "/ua/org/kytok/darknetxx/gui/icons"
)

add_custom_command(
  OUTPUT ${FONTAWESOME_RESOURCE_C}
  COMMAND ${GLIB_COMPILE_RESOURCES_EXEC}
          --target=${FONTAWESOME_RESOURCE_C}
          --generate-source
          ${FONTAWESOME_RESOURCE_XML}
  DEPENDS ${FONTAWESOME_RESOURCE_XML}
  COMMENT "Generating the GResource source of the Font Awesome icons"
)

add_library(
  DarknetXXFontAwesomeGResourceOBJ OBJECT
  ${FONTAWESOME_RESOURCE_C}
)

set_target_properties(
  DarknetXXFontAwesomeGResourceOBJ
  PROPERTIES COMPILE_OPTIONS ""
)

target_link_libraries(
  DarknetXXFontAwesomeGResourceOBJ
  PRIVATE PkgConfig::GTKMM
)
