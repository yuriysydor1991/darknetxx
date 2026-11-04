cmake_minimum_required(VERSION 3.13)

# The network weights files formats of the darknetxx-datadrivers library as the
# tests of the darknet networks name them (see the FormatsAccess.h): the
# readers and the writers of the library registry over the darknet networks,
# the gzip documents the tests read and write themselves through the zlib.
add_library(DarknetXXFormatsAccess INTERFACE)

target_include_directories(
  DarknetXXFormatsAccess
  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

# The library keeps it's own gzip streams and JSON parsers inside of it, the
# tests read the gzip JSON documents of the library on their own.
find_package(ZLIB REQUIRED)

target_link_libraries(
  DarknetXXFormatsAccess
  INTERFACE
    DarknetXXNetworksCore
    nlohmann_json::nlohmann_json
    ZLIB::ZLIB
)
