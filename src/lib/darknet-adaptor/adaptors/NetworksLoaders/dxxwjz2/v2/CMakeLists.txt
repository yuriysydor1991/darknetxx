cmake_minimum_required(VERSION 3.13)

# The dxxwjz2 (the darknetxx weights json zipped) network weights files of the
# second version: the dxxwjz1 ones of the network description and the
# training record, read and written by the dxxwjz1 machinery extended by the
# members of theirs (see the Dxxwjz2Format).
add_library(
  DarknetCXXDxxwjz2V2OBJ OBJECT
  Dxxwjz2FileFormat.cpp
  Dxxwjz2JsonEmitter.cpp
  Dxxwjz2MetadataReader.cpp
  Dxxwjz2Parser.cpp
  Dxxwjz2Reader.cpp
  Dxxwjz2SaxHandler.cpp
  Dxxwjz2SubtreeBuilder.cpp
  Dxxwjz2Validator.cpp
  Dxxwjz2Writer.cpp
)

target_include_directories(
  DarknetCXXDxxwjz2V2OBJ
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
)

target_link_libraries(
  DarknetCXXDxxwjz2V2OBJ
  PUBLIC
    nlohmann_json::nlohmann_json
    ZLIB::ZLIB
)

add_subdirectory(tests)
