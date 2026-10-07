cmake_minimum_required(VERSION 3.13)

# The window description embedded into the GUI binary as a GResource, which
# registers itself on the binary start.
set(GUI_RESOURCE_XML "${CMAKE_CURRENT_SOURCE_DIR}/darknetxx-gui.gresource.xml")
set(GUI_RESOURCE_C "${CMAKE_CURRENT_BINARY_DIR}/darknetxx-gui-resources.c")

add_custom_command(
  OUTPUT ${GUI_RESOURCE_C}
  COMMAND ${GLIB_COMPILE_RESOURCES_EXEC}
          --sourcedir=${CMAKE_CURRENT_SOURCE_DIR}
          --target=${GUI_RESOURCE_C}
          --generate-source
          ${GUI_RESOURCE_XML}
  DEPENDS ${GUI_RESOURCE_XML} ${CMAKE_CURRENT_SOURCE_DIR}/GtkmmWindow.ui
  COMMENT "Generating the GResource source of the ${DARKNETXX_GUI_BINARY_NAME}"
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

target_link_libraries(
  ${DARKNETXX_GUI_BINARY_NAME}
  DarknetXXGuiResourcesOBJ
)
