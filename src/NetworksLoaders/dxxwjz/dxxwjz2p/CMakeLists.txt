cmake_minimum_required(VERSION 3.13)

# The dxxwjz2p (the darknetxx weights json zipped project) files of the first
# version: the dxxwjz2 ones of the YOLO4 annotations of the images of the
# training project, read and written by the dxxwjz2 machinery extended by the
# member of theirs (see the Dxxwjz2pFormat).
add_library(
  DarknetCXXDxxwjz2pV1OBJ OBJECT
  Dxxwjz2pAnnotationsJson.cpp
  Dxxwjz2pAnnotationsReader.cpp
  Dxxwjz2pFileFormat.cpp
  Dxxwjz2pJsonEmitter.cpp
  Dxxwjz2pParser.cpp
  Dxxwjz2pReader.cpp
  Dxxwjz2pSaxHandler.cpp
  Dxxwjz2pValidator.cpp
  Dxxwjz2pWriter.cpp
)

target_include_directories(
  DarknetCXXDxxwjz2pV1OBJ
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
)

target_link_libraries(
  DarknetCXXDxxwjz2pV1OBJ
  PUBLIC
    nlohmann_json::nlohmann_json
    ZLIB::ZLIB
)
add_subdirectory(tests)
