cmake_minimum_required(VERSION 3.13)

# The network editor model of no toolkit, which the GTKmm 4 and the Qt 6
# network editor binaries share: the network of the cfg sections, the options
# the editor knows and the limits of their values, the project file it edits.
add_library(
  DarknetXXNetEditorOBJ OBJECT
  AOption.cpp
  AnchorsOption.cpp
  BoundedOption.cpp
  ChoiceOption.cpp
  DivisorOption.cpp
  HeadClassesOption.cpp
  HeadFiltersOption.cpp
  InputSizeOption.cpp
  LayerIndexesOption.cpp
  LayerReferences.cpp
  NetworkModel.cpp
  NetworkProject.cpp
  NetworkSection.cpp
  NetworkValidator.cpp
  NumberOption.cpp
  OptionContext.cpp
  OptionPresenter.cpp
  ScalesOption.cpp
  SectionSchema.cpp
  SectionSchemas.cpp
  StepsOption.cpp
  TextOption.cpp
)

target_include_directories(
  DarknetXXNetEditorOBJ
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
)

foreach(NET_EDITOR_BINARY_NAME IN LISTS DARKNETXX_NET_EDITOR_TARGETS)
  target_link_libraries(${NET_EDITOR_BINARY_NAME} DarknetXXNetEditorOBJ)
endforeach()

add_subdirectory(tests)
