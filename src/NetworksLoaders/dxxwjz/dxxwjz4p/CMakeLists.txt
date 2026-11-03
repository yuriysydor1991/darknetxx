cmake_minimum_required(VERSION 3.13)

# The dxxwjz4p (the darknetxx weights json zipped project) files of the first
# version: the whole training project in one file, the ZIP archive of the
# dxxwjz3p JSON document of the description text and of the images of it's
# annotations in the data folder (see the Dxxwjz4pFormat).
add_library(
  DarknetCXXDxxwjz4pV1OBJ OBJECT
  Dxxwjz4pArchive.cpp
  Dxxwjz4pFileFormat.cpp
  Dxxwjz4pJsonEmitter.cpp
  Dxxwjz4pParser.cpp
  Dxxwjz4pReader.cpp
  Dxxwjz4pSaxHandler.cpp
  Dxxwjz4pValidator.cpp
  Dxxwjz4pWorkspace.cpp
  Dxxwjz4pWriter.cpp
  Dxxwjz4pZip.cpp
)

target_include_directories(
  DarknetCXXDxxwjz4pV1OBJ
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
)

target_link_libraries(
  DarknetCXXDxxwjz4pV1OBJ
  PUBLIC
    nlohmann_json::nlohmann_json
    ZLIB::ZLIB
)

add_subdirectory(tests)
