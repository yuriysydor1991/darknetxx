cmake_minimum_required(VERSION 3.13)

# The rewrite of the edited network into the network files of the
# darknetxx-datadrivers library formats, validated by the darknet network of
# the edited cfg (the network editor binaries save their networks through it).
add_library(
  DarknetCXXNetworkRewriterOBJ OBJECT
  NetworkRewriter.cpp
)

target_include_directories(
  DarknetCXXNetworkRewriterOBJ
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
)

add_subdirectory(tests)
