cmake_minimum_required(VERSION 3.13)

# The dxxwjz4 (the darknetxx weights json zipped) network weights files of the
# fourth version: the dxxwjz3 ones of the description text of any length,
# read and written by the dxxwjz3 machinery extended by the member of theirs
# (see the Dxxwjz4Format).
add_library(
  DarknetCXXDxxwjz4V4OBJ OBJECT
  Dxxwjz4Description.cpp
  Dxxwjz4FileFormat.cpp
  Dxxwjz4JsonEmitter.cpp
  Dxxwjz4Parser.cpp
  Dxxwjz4Reader.cpp
  Dxxwjz4SaxHandler.cpp
  Dxxwjz4Validator.cpp
  Dxxwjz4Writer.cpp
)

target_include_directories(
  DarknetCXXDxxwjz4V4OBJ
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
)

target_link_libraries(
  DarknetCXXDxxwjz4V4OBJ
  PUBLIC
    nlohmann_json::nlohmann_json
    ZLIB::ZLIB
)

add_subdirectory(tests)
