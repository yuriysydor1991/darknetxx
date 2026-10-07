cmake_minimum_required(VERSION 3.13)

# The format neutral model of the network weights files: the arrays every
# layer kind keeps and their order (the WeightsSchema), the arrays of the
# network in the order of the files (the WeightsLayout), the content of the
# file every format reads and writes (the WeightsDocument with the network
# description and the training record) and its validation.
add_library(
  DarknetCXXWeightsModelOBJ OBJECT
  NetworkDescription.cpp
  NetworkFiles.cpp
  TrainingRecord.cpp
  WeightsDocument.cpp
  WeightsDocumentValidator.cpp
  WeightsLayout.cpp
  WeightsReport.cpp
  WeightsSchema.cpp
)

target_include_directories(
  DarknetCXXWeightsModelOBJ
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
)

add_subdirectory(tests)
