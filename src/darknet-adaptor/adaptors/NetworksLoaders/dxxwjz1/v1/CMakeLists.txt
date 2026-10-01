cmake_minimum_required(VERSION 3.13)

# The dxxwjz1 (the darknetxx weights json zipped) network weights files of the
# first version: the reader, the writer and the validator of them with the
# JSON text of the floats, the streaming JSON writer and the SAX parser (see
# the Dxxwjz1Format).
add_library(
  DarknetCXXDxxwjz1V1OBJ OBJECT
  Dxxwjz1Base64.cpp
  Dxxwjz1FloatCodec.cpp
  Dxxwjz1JsonEmitter.cpp
  Dxxwjz1Parser.cpp
  Dxxwjz1Reader.cpp
  Dxxwjz1Validator.cpp
  Dxxwjz1Writer.cpp
)

target_include_directories(
  DarknetCXXDxxwjz1V1OBJ
  PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
)

target_link_libraries(
  DarknetCXXDxxwjz1V1OBJ
  PUBLIC
    nlohmann_json::nlohmann_json
    ZLIB::ZLIB
)

target_link_libraries(
  ${PROJECT_BINARY_NAME}
  DarknetCXXDxxwjz1V1OBJ
)

add_subdirectory(tests)
