cmake_minimum_required(VERSION 3.13)

# The YOLO4 training folder of the conversion: the dxxwjz2p project files of
# the folder, read and written the way the training reads the folder (see the
# Yolo4FolderLayout).
add_library(
  DarknetCXXYolo4FolderOBJ OBJECT
  Yolo4FolderFormat.cpp
  Yolo4FolderLayout.cpp
  Yolo4FolderReader.cpp
  Yolo4FolderWriter.cpp
)

target_include_directories(
  DarknetCXXYolo4FolderOBJ
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(
  DarknetCXXYolo4FolderOBJ
  PUBLIC
    nlohmann_json::nlohmann_json
    ZLIB::ZLIB
)
add_subdirectory(tests)
