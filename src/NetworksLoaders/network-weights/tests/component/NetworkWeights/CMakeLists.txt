cmake_minimum_required(VERSION 3.13)

# the cfg files of the networks the structures are compared of
set(CTEST_NETWORK_WEIGHTS_CFG_DIR "${PROJECT_SOURCE_DIR}/misc/examples/cfg")
set(
  CTEST_NETWORK_WEIGHTS_DATA_DIR
  "${PROJECT_SOURCE_DIR}/src/NetworksLoaders/OrigDefaultLoader/tests/component/OrigDefaultLoader/data"
)

configure_file(
  CTEST_NetworkWeights_provider.h.in
  CTEST_NetworkWeights_provider.h
)

add_executable(
  CTEST_NetworkWeights
  CTEST_NetworkWeights.cpp
)

target_include_directories(
  CTEST_NetworkWeights
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
  CTEST_NetworkWeights
  DarknetXXNetworksCore
  GTest::gtest_main GTest::gmock
)

include(GoogleTest)

# Every case runs all the cfg files of it's suite in one process, the biggest
# of the networks take a few seconds each to be built.
#
# The tests run in the misc directory, so the data/9k.tree of the yolo9000.cfg
# the darknet reads beside the network is the misc/data one.
gtest_add_tests(
  TARGET CTEST_NetworkWeights
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/misc
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 1200)
