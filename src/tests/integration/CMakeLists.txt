cmake_minimum_required(VERSION 3.13)

if (NOT ENABLE_INTEGRATION_TESTS)
  return()
endif()

add_subdirectory(digits)
add_subdirectory(digits-bash)
add_subdirectory(converter)
add_subdirectory(converter-bash)
add_subdirectory(gui-bash)
