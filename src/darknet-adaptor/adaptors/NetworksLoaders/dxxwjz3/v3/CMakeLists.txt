cmake_minimum_required(VERSION 3.13)

# The dxxwjz3 (the darknetxx weights json zipped) network weights files of the
# third version: the dxxwjz2 ones of nothing but the JSON data, the network cfg
# of the JSON structure of the layer arrays, read and written by the dxxwjz2
# machinery extended by the members of theirs (see the Dxxwjz3Format).
add_library(
  DarknetCXXDxxwjz3V3OBJ OBJECT
  Dxxwjz3Bytes.cpp
  Dxxwjz3CfgJson.cpp
  Dxxwjz3FileFormat.cpp
  Dxxwjz3JsonEmitter.cpp
  Dxxwjz3Losses.cpp
  Dxxwjz3MetadataReader.cpp
  Dxxwjz3NetworkBuilder.cpp
  Dxxwjz3Parser.cpp
  Dxxwjz3Reader.cpp
  Dxxwjz3SaxHandler.cpp
  Dxxwjz3Validator.cpp
  Dxxwjz3ValueCodec.cpp
  Dxxwjz3Writer.cpp
)

target_include_directories(
  DarknetCXXDxxwjz3V3OBJ
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
)

target_link_libraries(
  DarknetCXXDxxwjz3V3OBJ
  PUBLIC
    nlohmann_json::nlohmann_json
    ZLIB::ZLIB
)

add_subdirectory(tests)
