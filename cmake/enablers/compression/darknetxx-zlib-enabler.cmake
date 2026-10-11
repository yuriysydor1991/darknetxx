cmake_minimum_required(VERSION 3.13)

option(
    ENABLE_ZLIB
    "Enables the zlib compression library (deflate, gzip, zip) for the project usage through system installed one or FetchContent by internet"
    ON
)

if (NOT ENABLE_ZLIB)
    return()
endif()

set(DARKNETXX_ZLIB_GIT "https://github.com/madler/zlib.git" CACHE STRING "The zlib library git source repository")
set(DARKNETXX_ZLIB_GIT_TAG "v1.3.1" CACHE STRING "The zlib project git repository tag of interest")

# the fetched zlib is linked statically, so its examples are of no use
set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "Build the zlib examples" FORCE)

darknetxx_default_3rdparty_enabler(
  NAME ZLIB
  GIT_REPOSITORY ${DARKNETXX_ZLIB_GIT}
  GIT_TAG ${DARKNETXX_ZLIB_GIT_TAG}
)

# The system zlib comes as the ZLIB::ZLIB target, while the fetched one
# declares the zlibstatic (static) and the zlib (shared) targets. The alias
# gives the project the single ZLIB::ZLIB target to link for both of them.
if (NOT TARGET ZLIB::ZLIB AND TARGET zlibstatic)
  set_target_properties(zlibstatic PROPERTIES POSITION_INDEPENDENT_CODE ON)
  add_library(ZLIB::ZLIB ALIAS zlibstatic)
endif()

if (NOT TARGET ZLIB::ZLIB)
  message(FATAL_ERROR "The zlib library made available provides no ZLIB::ZLIB target")
endif()

# Link the ZLIB::ZLIB target to your target(s) of interest (e.g. the
# ${DARKNETXX_BINARY_NAME} executable or any of your own libraries):
#   target_link_libraries(${DARKNETXX_BINARY_NAME} ZLIB::ZLIB)
#
# The darknetxx links it into the zlibi::ZlibController (see the src/zlib
# directory), the compressor of the dxxwjz1 network weights files.
#
# See doc/sections/en_US/5-23-enabling-the-zlib-library.md (and the uk_UA copy).
