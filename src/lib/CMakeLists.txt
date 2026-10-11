cmake_minimum_required(VERSION 3.13)

# ---------------------------------------------------------------------------
# The library of all the project functionality, the darknet training, the
# detection and the network weights files conversion, behind the facade of
# the facade/public headers, the only ones it exports (see the
# facade/public/LibraryAPI.h) and installs.
# ---------------------------------------------------------------------------

# the facade export half of the DARKNETXX_LIB_API of the public headers
add_compile_definitions(DARKNETXX_LIB_BUILDING=1)

# The components the library is built of, the src directories beside this
# one, included from here so they build under the library definitions above.
foreach(LIB_COMPONENT darknet-adaptor helpers OMP zlib training recognition)
  add_subdirectory(
    ${PROJECT_SOURCE_DIR}/src/${LIB_COMPONENT}
    ${PROJECT_BINARY_DIR}/src/${LIB_COMPONENT}
  )
endforeach()

add_subdirectory(libmain)
add_subdirectory(facade)

# The objects of the whole library, which the component tests of it's
# internals link as well.
add_library(DarknetXXLibraryObjects INTERFACE)

target_sources(
  DarknetXXLibraryObjects
  INTERFACE
    $<TARGET_OBJECTS:DarknetCXXModDarknetAdaptorOBJ>
    $<TARGET_OBJECTS:DarknetCXXADetectorAdaptorOBJ>
    $<TARGET_OBJECTS:DarknetCXXADetectorTrainerAdaptorOBJ>
    $<TARGET_OBJECTS:DarknetCXXOpenMPControllerOBJ>
    $<TARGET_OBJECTS:DarknetXXTrainingOBJ>
    $<TARGET_OBJECTS:DarknetXXRecognitionOBJ>
    $<TARGET_OBJECTS:DarknetXXLibMainOBJ>
    $<TARGET_OBJECTS:DarknetXXLibFacadeOBJ>
)

target_link_libraries(
  DarknetXXLibraryObjects
  INTERFACE DarknetXXNetworksCore
)

add_library(${DARKNETXX_LIBRARY_NAME} SHARED)

# the name the installed package exports it by
add_library(
  ${DARKNETXX_LIBRARY_NAME}::${DARKNETXX_LIBRARY_NAME}
  ALIAS ${DARKNETXX_LIBRARY_NAME}
)

target_link_libraries(
  ${DARKNETXX_LIBRARY_NAME}
  PRIVATE DarknetXXLibraryObjects
)

# The build tree mirrors the installed include/<library name> directory, so
# the library of the FetchContent takes the very same includes the installed
# one does.
set(DARKNETXX_LIB_BUILD_INCLUDEDIR "${CMAKE_CURRENT_BINARY_DIR}/include")

file(
  GLOB DARKNETXX_LIB_PUBLIC_HEADERS
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/facade/public/*.h"
)

foreach(public_header IN LISTS DARKNETXX_LIB_PUBLIC_HEADERS)
  get_filename_component(public_header_name ${public_header} NAME)

  configure_file(
    ${public_header}
    ${DARKNETXX_LIB_BUILD_INCLUDEDIR}/${DARKNETXX_LIBRARY_NAME}/${public_header_name}
    COPYONLY
  )
endforeach()

# Both the include root and the library name one are exported, so a user
# writes the safe #include <darknetxx-0.11/LibraryFacade.h> or the short
# #include <LibraryFacade.h>.
target_include_directories(
  ${DARKNETXX_LIBRARY_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${DARKNETXX_LIB_BUILD_INCLUDEDIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/facade/public>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${DARKNETXX_LIBRARY_NAME}>
)

set_target_properties(
  ${DARKNETXX_LIBRARY_NAME}
  PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/darknetxx-lib-install.cmake)
