cmake_minimum_required(VERSION 3.13)

include(GNUInstallDirs)

# ---------------------------------------------------------------------------
# The project builds the library of all it's functionality (see the
# src/lib/CMakeLists.txt) and the executables of the src/app component over
# it, one and the same code built into more than one binary (see the
# src/*-target.cmake files). So the src/app component hands it's sources to
# every executable of the DARKNETXX_APP_TARGETS list the src/CMakeLists.txt
# fills, instead of naming one of them.
# ---------------------------------------------------------------------------

# @brief Adds the given sources to every target of the given list variable.
function(darknetxx_targets_sources TARGETS_LIST)
  foreach(APP_TARGET IN LISTS ${TARGETS_LIST})
    target_sources(${APP_TARGET} PRIVATE ${ARGN})
  endforeach()
endfunction()

# @brief Declares an executable of the src/main.cpp over the library, with the
# include roots, the own logger copy and the install rule every one of them
# has.
function(darknetxx_add_app_target APP_TARGET)
  add_executable(${APP_TARGET} ${PROJECT_SOURCE_DIR}/src/main.cpp)

  target_include_directories(
    ${APP_TARGET}
    PRIVATE ${PROJECT_SOURCE_DIR}
    PRIVATE ${PROJECT_SOURCE_DIR}/src
    PRIVATE ${PROJECT_BINARY_DIR}
  )

  target_link_libraries(
    ${APP_TARGET}
    ${DARKNETXX_LIBRARY_NAME}
    DarknetXXDefaultLoggerObj
  )

  # the installed library is found beside the installed executable
  set_target_properties(
    ${APP_TARGET}
    PROPERTIES INSTALL_RPATH "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}"
  )

  install(
    TARGETS ${APP_TARGET}
    RUNTIME DESTINATION ${DARKNETXX_BINARY_INSTALLATION_DIR}
  )
endfunction()
