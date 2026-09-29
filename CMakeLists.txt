cmake_minimum_required(VERSION 3.13)

# Change the PROJECT_NAME var value to change executable name
set(
  PROJECT_NAME "darknetxx"
  CACHE STRING "Project general name with an executable name"
)

project(
  ${PROJECT_NAME}
  VERSION 0.11.0
  DESCRIPTION "Ported version of Darknet NN"
  HOMEPAGE_URL "http://www.kytok.org.ua/"
  LANGUAGES CXX C
)

# The Darknet C core runs an order of magnitude slower without optimization,
# so a single-config build without an explicit build type defaults to Release
# (as the original Darknet CMake project did).
get_property(PROJECT_GENERATOR_IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if(NOT CMAKE_BUILD_TYPE AND NOT PROJECT_GENERATOR_IS_MULTI_CONFIG)
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

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Git REQUIRED)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(template-project-misc-variables-declare)
include(template-project-feature-enabler)

add_subdirectory(src)
add_subdirectory(doc)
add_subdirectory(misc)

include(template-project-feature-enabler-post)

