cmake_minimum_required(VERSION 3.13)

if(NOT ENABLE_COMPONENT_TESTS)
  return()
endif()

# the model of the network editors over the library they save the networks
# by
foreach(CTEST_NAME NetEditorModel NetworkProject)
  set(CTEST_TARGET CTEST_${CTEST_NAME})

  add_executable(
    ${CTEST_TARGET}
    ${CTEST_NAME}/${CTEST_TARGET}.cpp
  )

  target_compile_definitions(
    ${CTEST_TARGET}
    PRIVATE ${CTEST_TARGET}_DATA_DIR=\"${CMAKE_CURRENT_BINARY_DIR}/${CTEST_NAME}\"
  )

  target_include_directories(
    ${CTEST_TARGET}
    PRIVATE
      ${PROJECT_SOURCE_DIR}
      ${PROJECT_BINARY_DIR}
  )

  target_link_libraries(
    ${CTEST_TARGET}
    DarknetXXNetEditorOBJ
    DarknetXXDefaultLoggerObj
    ${DARKNETXX_LIBRARY_NAME}
    GTest::gtest_main
  )

  include(GoogleTest)

  gtest_add_tests(
    TARGET ${CTEST_TARGET}
    TEST_SUFFIX .noArgs
    TEST_LIST noArgsTests
  )

  set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
endforeach()
