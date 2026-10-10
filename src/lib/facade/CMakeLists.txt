cmake_minimum_required(VERSION 3.13)

# the public classes the library internals pass around
add_library(
  DarknetXXLibTypesOBJ OBJECT
  LibraryContext.cpp
  Recognition.cpp
  RecognizedObject.cpp
  TrainingProgress.cpp
)

add_library(
  DarknetXXLibFacadeOBJ OBJECT
  LibraryFacade.cpp
)

foreach(FACADE_OBJ DarknetXXLibTypesOBJ DarknetXXLibFacadeOBJ)
  target_include_directories(
    ${FACADE_OBJ}
    PRIVATE
      ${PROJECT_SOURCE_DIR}
      ${PROJECT_BINARY_DIR}
  )
endforeach()

# the include directories and the definitions of the weights file formats
target_link_libraries(
  DarknetXXLibFacadeOBJ
  PRIVATE DarknetCXXModDarknetAdaptorOBJ
)

add_subdirectory(tests)
