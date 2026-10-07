cmake_minimum_required(VERSION 3.13)

project(
  darknetxx
  VERSION 0.11.0
  DESCRIPTION "Ported version of Darknet NN"
  HOMEPAGE_URL "http://www.kytok.org.ua/"
  LANGUAGES CXX C
)

# The project is either the top level one or the dependency of another one
# (made available by the FetchContent or added by the add_subdirectory), which
# gets the library of this one and none of it's tests, tools and packages.
string(
  COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}"
  DARKNETXX_IS_TOP_LEVEL
)

if(DARKNETXX_IS_TOP_LEVEL)
  # The Darknet C core runs an order of magnitude slower without
  # optimization, so a single-config build without an explicit build type
  # defaults to Release (as the original Darknet CMake project did).
  get_property(
    DARKNETXX_GENERATOR_IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG
  )

  if(NOT CMAKE_BUILD_TYPE AND NOT DARKNETXX_GENERATOR_IS_MULTI_CONFIG)
    message(STATUS "No build type specified, defaulting to Release")
    set(
      CMAKE_BUILD_TYPE "Release"
      CACHE STRING "Choose the type of build: Debug, Release, RelWithDebInfo or MinSizeRel"
      FORCE
    )
    set_property(
      CACHE CMAKE_BUILD_TYPE
      PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel"
    )
  endif()
else()
  # The options of the project which uses this one name it's own tests, tools,
  # packages and warnings, so the same named options of this one are turned
  # off, and the libraries this one requires are turned on, all of them inside
  # the scope of this project alone.
  foreach(
    top_level_option
    ENABLE_UNIT_TESTS ENABLE_COMPONENT_TESTS ENABLE_INTEGRATION_TESTS
    ENABLE_CLANGFORMAT ENABLE_CPPCHECK ENABLE_VALGRIND ENABLE_CLANG_TIDY
    ENABLE_GPROF ENABLE_CALLGRIND ENABLE_DOCKER ENABLE_JENKINS_DOCKER_PIPELINE
    ENABLE_DEB ENABLE_FLATPAK ENABLE_SNAP ENABLE_RPM ENABLE_FREEBSD_PKG
    ENABLE_WIX COMPILE_WARNINGS_AS_ERRORS ENABLE_COMPILER_CODE_ANALYZER
  )
    set(${top_level_option} OFF)
  endforeach()

  set(ENABLE_ZLIB ON)
  set(ENABLE_NLOHMANN_JSON ON)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# the commit of the build, when built of a git checkout
find_package(Git QUIET)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(darknetxx-misc-variables-declare)
include(darknetxx-feature-enabler)

add_subdirectory(src)

if(DARKNETXX_IS_TOP_LEVEL)
  add_subdirectory(doc)
endif()

add_subdirectory(misc)

include(darknetxx-feature-enabler-post)

