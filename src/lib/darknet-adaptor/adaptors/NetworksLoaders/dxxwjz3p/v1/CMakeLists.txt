cmake_minimum_required(VERSION 3.13)

# The dxxwjz3p (the darknetxx weights json zipped project) files of the first
# version: the dxxwjz3 ones of the YOLO4 annotations of the images of the
# training project, read and written by the dxxwjz2p machinery of the dxxwjz3
# members (see the Dxxwjz3pFormat).
add_library(
  DarknetCXXDxxwjz3pV1OBJ OBJECT
  Dxxwjz3pFileFormat.cpp
  Dxxwjz3pJsonEmitter.cpp
  Dxxwjz3pParser.cpp
  Dxxwjz3pReader.cpp
  Dxxwjz3pSaxHandler.cpp
  Dxxwjz3pValidator.cpp
  Dxxwjz3pWriter.cpp
)

target_include_directories(
  DarknetCXXDxxwjz3pV1OBJ
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
)

target_link_libraries(
  DarknetCXXDxxwjz3pV1OBJ
  PUBLIC
    nlohmann_json::nlohmann_json
    ZLIB::ZLIB
)

add_subdirectory(tests)
