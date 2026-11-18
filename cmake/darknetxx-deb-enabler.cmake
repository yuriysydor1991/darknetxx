cmake_minimum_required(VERSION 3.13)

include(GNUInstallDirs)
include(InstallRequiredSystemLibraries)

set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_CONTACT ${DARKNETXX_MAINTAINER_EMAIL})
set(CPACK_DEB_MAINTAINER "${DARKNETXX_MAINTAINER} ${DARKNETXX_MAINTAINER_EMAIL}")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${DARKNETXX_MAINTAINER} ${DARKNETXX_MAINTAINER_EMAIL}")
set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_PACKAGE_DESCRIPTION ${PROJECT_DESCRIPTION})
set(CPACK_DEB_PACKAGE_ARCHITECTURE "amd64")  # Or arm64
set(CPACK_DEBIAN_PACKAGE_DEPENDS "")
# the runtime libraries of the binaries, the GUI toolkit one included
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

# The darknetxx-datadrivers library the binaries are linked against is no
# distribution package, so the dpkg-shlibdeps scan above finds no package of it
# and the package would start on a system which has the library installed
# already alone. The option packs the library into the package instead - the
# very shared object the found CMake package of the
# darknetxx-DarknetxxDataDrivers-enabler module points at, with it's SONAME
# link, into the lib directory the project installs it's own library into - so
# the package is complete on its own, the way the ImagesAnnotator project packs
# it's own data libraries. The darknetxxConverter binary of the library comes
# with the packages of the library itself. A fetched library is installed by
# it's own install rules, the darknetxxConverter included, so only a found one
# is packed here.
option(
  ENABLE_DEB_DARKNETXX_DATA_DRIVERS
  "Set to ON to pack the darknetxx-datadrivers library the binaries are linked against into the deb package"
  ON
)

if(ENABLE_DEB_DARKNETXX_DATA_DRIVERS AND ${DARKNETXX_DXXDD_PACKAGE}_FOUND)
  install(
    FILES
      $<TARGET_FILE:${DXXDD_LIBRARIES}>
      $<TARGET_SONAME_FILE:${DXXDD_LIBRARIES}>
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

  message(STATUS "DEB package carries the ${DXXDD_LIBRARIES} library")
endif()

include(CPack)
