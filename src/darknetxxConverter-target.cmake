cmake_minimum_required(VERSION 3.13)

# ---------------------------------------------------------------------------
# The converter only binary: the network weights files format conversion of
# the main application (the original darknet weights files and the dxxwjz1
# ones, see the src/darknet-adaptor/adaptors/NetworksLoaders directory), with
# no training nor detection behind it.
#
# It is built out of the very same src/app component and the very same
# main.cpp as the main binary - what it does not get is the app::Application
# class, which starts the training and the detection, and the
# DarknetController and the OpenMP controller only that class needs. The
# DARKNETXX_CONVERTER_ONLY definition below is what tells the sources both
# binaries share - the app::ApplicationFactory, which creates that class, and
# the help and the version printers, which name the running binary - that the
# class is not there.
#
# The network infrastructure the conversion reads and writes the files with
# (the networks of the cfg files, the weights files formats) is the
# DarknetXXNetworksCore library, the one the component tests link, which
# holds the application context the app component is built with as well.
# ---------------------------------------------------------------------------
if(NOT ENABLE_DARKNETXX_CONVERTER)
  message(STATUS
    "The ${PROJECT_CONVERTER_BINARY_NAME} target is excluded "
    "(ENABLE_DARKNETXX_CONVERTER=OFF)")
  return()
endif()

add_executable(
  ${PROJECT_CONVERTER_BINARY_NAME}
  ${CMAKE_CURRENT_LIST_DIR}/main.cpp
)

target_compile_definitions(
  ${PROJECT_CONVERTER_BINARY_NAME}
  PRIVATE DARKNETXX_CONVERTER_ONLY
)

target_include_directories(
  ${PROJECT_CONVERTER_BINARY_NAME}
  PRIVATE ${CMAKE_SOURCE_DIR}
  PRIVATE ${CMAKE_CURRENT_LIST_DIR}
  PRIVATE ${CMAKE_BINARY_DIR}
)

target_link_libraries(
  ${PROJECT_CONVERTER_BINARY_NAME}
  DarknetXXNetworksCore
)

install(
  TARGETS ${PROJECT_CONVERTER_BINARY_NAME}
  RUNTIME DESTINATION bin
)
