cmake_minimum_required(VERSION 3.13)

# The GUIs of the GUI binaries (see the src/darknetxx-gui-targets.cmake): the
# GTKmm 4 windows over the training and the recognition of the library and the
# network editor windows of the GTKmm 4 and the Qt 6 over the model of no
# toolkit they share.
if(DARKNETXX_NET_EDITOR_TARGETS)
  add_subdirectory(net-editor)
endif()

if(DARKNETXX_GTKMM_GUI_TARGETS)
  add_subdirectory(gtkmm4)
endif()

if(DARKNETXX_QT_GUIS)
  add_subdirectory(qt6)
endif()
