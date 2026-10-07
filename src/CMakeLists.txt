cmake_minimum_required(VERSION 3.13)

include(darknetxx-compile-options)
add_compile_options(${EXTRA_COMPILE_OPTIONS})
add_link_options(${EXTRA_LINK_OPTIONS})
add_compile_definitions(${EXTRA_COMPILE_DEFINITIONS})

# GCC 14+ turned several legacy C diagnostics into errors by default. Keep them
# as warnings for the vendored original Darknet C sources (compiled directly by
# some component tests), without relaxing the C++ strictness. The list holds
# only the options the compiler supports (see the compile options module).
foreach(demoted_error_option IN LISTS C_DEMOTED_ERRORS_OPTIONS)
  add_compile_options($<$<COMPILE_LANGUAGE:C>:${demoted_error_option}>)
endforeach()

include(darknetxx-targets)

# The library exports it's facade alone (see the src/lib/CMakeLists.txt), while
# every object library of the project is relocatable for it.
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# the public library headers include each other by their file names
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/facade/public)

# the mocks every component test reuses
set(GENERAL_MOCKS_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/tests/mocks")

configure_file(
  project-global-decls.h.in
  ${PROJECT_BINARY_DIR}/project-global-decls.h
)

# The executables over the library: the main one and the network weights
# files converter only and the GUI ones beside it, which add themselves to the
# list. They are declared before the components below, since the components
# hand their sources to them.
set(DARKNETXX_APP_TARGETS "")

if(ENABLE_DARKNETXX_EXECUTABLES)
  darknetxx_add_app_target(${DARKNETXX_BINARY_NAME})
  list(APPEND DARKNETXX_APP_TARGETS ${DARKNETXX_BINARY_NAME})

  include(${CMAKE_CURRENT_SOURCE_DIR}/darknetxxConverter-target.cmake)
  include(${CMAKE_CURRENT_SOURCE_DIR}/darknetxx-gui-target.cmake)
endif()

# the digits YOLO4 folder the training component tests copy and train
if(ENABLE_COMPONENT_TESTS)
  include(${CMAKE_CURRENT_SOURCE_DIR}/tests/digits-yolo4-folder.cmake)
endif()

add_subdirectory(log)
add_subdirectory(lib)

if(ENABLE_DARKNETXX_EXECUTABLES)
  add_subdirectory(app)
  add_subdirectory(converters)
  add_subdirectory(scripts)
endif()

if(TARGET ${DARKNETXX_GUI_BINARY_NAME})
  add_subdirectory(gui)
endif()

add_subdirectory(tests)
