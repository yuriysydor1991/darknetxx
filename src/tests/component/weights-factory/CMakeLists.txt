cmake_minimum_required(VERSION 3.13)

# The generated darknet networks and weights files the component and the
# integration tests share: the network specifications of every layer kind and
# their cfg files, the independently modelled weights file layouts and
# contents, the network variants from the tiny ones and up to the 4K resolution
# ones, the darknet networks of them and the directories of the test files.
add_library(
  DarknetXXWeightsFactory STATIC
  DarknetNetwork.cpp
  NetworkChecks.cpp
  WeightsFile.cpp
  WeightsNetSpec.cpp
  WeightsVariants.cpp
  WeightsWorkspace.cpp
)

# the checks of the darknet networks against the generated specifications
# take the darknet structs of the networks core, the darknet networks of them
# are made by the core (the test executables link it)
target_include_directories(
  DarknetXXWeightsFactory
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    $<TARGET_PROPERTY:DarknetXXNetworksCore,INTERFACE_INCLUDE_DIRECTORIES>
)

target_compile_definitions(
  DarknetXXWeightsFactory
  PUBLIC
    $<TARGET_PROPERTY:DarknetXXNetworksCore,INTERFACE_COMPILE_DEFINITIONS>
)

target_link_libraries(
  DarknetXXWeightsFactory
  PUBLIC
    GTest::gtest
)
