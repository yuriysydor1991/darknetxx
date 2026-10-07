cmake_minimum_required(VERSION 3.13)

# The journal of the training: the network description, the losses and the
# time of the training the weights files it saves keep.
add_library(
  DarknetCXXTrainingJournalOBJ OBJECT
  TrainingJournal.cpp
)

target_include_directories(
  DarknetCXXTrainingJournalOBJ
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
)

add_subdirectory(tests)
