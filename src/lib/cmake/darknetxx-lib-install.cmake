cmake_minimum_required(VERSION 3.13)

# ---------------------------------------------------------------------------
# The install rules of the library: the runtime alone, or the development
# files as well (the public headers, the CMake package files and the namelink
# the -l<name> linker flag looks for), see the DARKNETXX_LIB_INSTALL_DEV_FILES.
# ---------------------------------------------------------------------------
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if(NOT DARKNETXX_LIB_INSTALL_DEV_FILES)
  install(
    TARGETS ${DARKNETXX_LIBRARY_NAME}
    LIBRARY
      DESTINATION ${CMAKE_INSTALL_LIBDIR}
      COMPONENT Runtime
      NAMELINK_SKIP
  )

  return()
endif()

set(DARKNETXX_LIB_PACKAGE_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${DARKNETXX_LIBRARY_NAME}")

install(
  TARGETS ${DARKNETXX_LIBRARY_NAME}
  EXPORT ${DARKNETXX_LIBRARY_NAME}Targets
  LIBRARY
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT Runtime
    NAMELINK_COMPONENT Development
)

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/facade/public/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${DARKNETXX_LIBRARY_NAME}
  COMPONENT Development
)

install(
  EXPORT ${DARKNETXX_LIBRARY_NAME}Targets
  NAMESPACE ${DARKNETXX_LIBRARY_NAME}::
  DESTINATION ${DARKNETXX_LIB_PACKAGE_DIR}
  COMPONENT Development
)

configure_package_config_file(
  ${CMAKE_CURRENT_LIST_DIR}/DarknetxxLibraryConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/${DARKNETXX_LIBRARY_NAME}Config.cmake
  INSTALL_DESTINATION ${DARKNETXX_LIB_PACKAGE_DIR}
)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/${DARKNETXX_LIBRARY_NAME}ConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMinorVersion
)

install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${DARKNETXX_LIBRARY_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${DARKNETXX_LIBRARY_NAME}ConfigVersion.cmake
  DESTINATION ${DARKNETXX_LIB_PACKAGE_DIR}
  COMPONENT Development
)
