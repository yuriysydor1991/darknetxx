cmake_minimum_required(VERSION 3.13)

set(
  DARKNETXX_BINARY_NAME ${PROJECT_NAME}
  CACHE STRING "Project main binary name and target"
)

string(TOLOWER ${DARKNETXX_BINARY_NAME} DARKNETXX_BINARY_NAME_lower)

set(
  DARKNETXX_BINARY_INSTALLATION_DIR "bin"
  CACHE STRING "The project binary installation sub directory (used by the snap packager)"
)

set(
  DARKNETXX_MAINTAINER "Your Name"
  CACHE STRING "Project maintainer name (used for the DEB package)"
)

set(
  DARKNETXX_MAINTAINER_EMAIL "yourname@your.email.org"
  CACHE STRING "Project maintainer email (used for the DEB package)"
)

option(
  ENABLE_UNIT_TESTS 
  "Set to ON value if unit tests build and run should be available"
  OFF
)

option(
  ENABLE_COMPONENT_TESTS
  "Set to ON value if component tests build and run should be available"
  OFF
)

option(
  ENABLE_INTEGRATION_TESTS
  "Set to ON value if integration tests (a whole application training and detection runs) build and run should be available"
  OFF
)

option(
  GTEST_TRY_SYSTEM_PROBE 
  "Set to ON value if current project CMake files should probe the system GTest"
  ON
)

# The Doxygen documentation variables and options are declared in the
# doc/CMakeLists.txt file (see the add_subdirectory(doc) call in the root
# CMakeLists.txt), the same way as in the cpp-app-template project.

option(
  ENABLE_CLANGFORMAT
  "Set to ON if to enable the clang-format command execution target."
  OFF
)

option(
  ENABLE_CPPCHECK
  "Set to ON if to enable the cppcheck execution target."
  OFF
)

option(
  ENABLE_VALGRIND
  "Set to ON if to enable the valgrind memory check execution target."
  OFF
)

option(
  COMPILE_WARNINGS_AS_ERRORS
  "Set to ON if to pass -Werror compile parameter to the GCC compiler."
  OFF
)

option(
  ENABLE_COMPILER_CODE_ANALYZER
  "Set to ON if to pass -fanalyzer compile parameter to the GCC compiler. May dramatically increase the compile time."
  OFF
)

option(
  ENABLE_CLANG_TIDY
  "Set to ON if enable the clang-tidy code check target."
  OFF
)

option(
  ENABLE_DEB
  "Set to ON to enable the deb package creation from the project build artifacts"
  OFF
)

option(
  ENABLE_FLATPAK
  "Set to ON to enable the flatpak package creation from the project build artifacts"
  OFF
)

option(
  ENABLE_DOCKER
  "Set to ON to enable the dockers run"
  OFF
)

option(
  ENABLE_SANITIZERS
  "Set to ON to enable the sanitizers compile options"
  OFF
)

option(
  ENABLE_SANITIZERS_THREADS
  "Set to ON to enable the sanitizers for the threads compile options"
  OFF
)

set(
  MAX_LOG_LEVEL "3"
  CACHE STRING 
  "Sets the maximum severity of the compiled log messages. Error=0. Trace=5. By default Info=3"
)

# The dxxwjz1 network weights files are the JSON documents, so the library is
# required (see the darknetxx-feature-enabler module).
option(
  ENABLE_NLOHMANN_JSON
  "Set to ON to enable the nlohmann json library (by using system wide available or through Internet)"
  ON
)

# ---------------------------------------------------------------------------
# The library of all the project functionality (see the src/lib directory)
# and the executables over it.
#
#   -DENABLE_DARKNETXX_EXECUTABLES=OFF     -> the library alone, the default of
#                                             the project of another one
#   -DDARKNETXX_LIB_INSTALL_DEV_FILES=OFF  -> the library runtime alone
# ---------------------------------------------------------------------------
set(
  DARKNETXX_LIBRARY_NAME
  "${PROJECT_NAME}-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
  CACHE STRING "The library name, target and CMake package name"
)

option(
  ENABLE_DARKNETXX_EXECUTABLES
  "Build and install the darknetxx, the darknetxxConverter and the GUI executables beside the library"
  ${DARKNETXX_IS_TOP_LEVEL}
)

option(
  DARKNETXX_LIB_INSTALL_DEV_FILES
  "Install the public headers, the CMake package files and the namelink of the library beside it"
  ON
)

# ---------------------------------------------------------------------------
# The converter only binary.
#
# The project builds two application binaries out of one and the same code.
# The main one trains and runs the networks; the second one, named after it,
# carries the network weights files format conversion alone (the original
# darknet weights, the dxxwjz1 and the dxxwjz2 ones, see the
# src/NetworksLoaders directory). It is the very same
# src/app component compiled with the DARKNETXX_CONVERTER_ONLY definition,
# which leaves the app::Application class (and with it the training and the
# detection the class starts) out. The conversion stays available through the
# command line of the main binary as well, so the second one adds a deployment
# option rather than a feature.
#
#   (nothing given)                        -> both binaries
#   -DENABLE_DARKNETXX_CONVERTER=OFF       -> the main binary alone
# ---------------------------------------------------------------------------
option(
  ENABLE_DARKNETXX_CONVERTER
  "Build the network weights files converter only binary beside the main application one"
  ON
)

set(
  DARKNETXX_CONVERTER_BINARY_NAME "${DARKNETXX_BINARY_NAME}Converter"
  CACHE STRING "The network weights files converter only binary name and target"
)

# ---------------------------------------------------------------------------
# The GUI binaries: the works of the main binary behind the GTKmm 4 windows
# (see the src/darknetxx-gui-targets.cmake), the training one, which trains the
# chosen YOLO4 folder, and the recognition one, which detects the objects in
# the chosen image or classifies it by the network of the chosen weights file.
#
#   -DENABLE_DARKNETXX_GUI=OFF             -> no GUI binaries, no GTKmm 4 needed
# ---------------------------------------------------------------------------
option(
  ENABLE_DARKNETXX_GUI
  "Build the GUI training and recognition binaries beside the main application one (requires the GTKmm 4)"
  ON
)

set(
  DARKNETXX_TRAIN_GUI_BINARY_NAME "${DARKNETXX_BINARY_NAME}-train-gui"
  CACHE STRING "The GUI training binary name and target"
)

set(
  DARKNETXX_RECOGNITION_GUI_BINARY_NAME "${DARKNETXX_BINARY_NAME}-recognition-gui"
  CACHE STRING "The GUI recognition binary name and target"
)

# The application ids of the GUI binaries: their D-Bus names and the names of
# their desktop entries and icons, the training one is the flatpak package id
# as well.
set(DARKNETXX_APPLICATION_ID "ua.org.kytok.template.${DARKNETXX_BINARY_NAME}")
set(DARKNETXX_RECOGNITION_APPLICATION_ID "${DARKNETXX_APPLICATION_ID}.recognition")

# The Darknet logo of the Darknet sources, the icon of the GUI binaries and of
# their shortcuts on the MS Windows and of the MSI package.
set(
  DARKNETXX_LOGO_ICO
  "${PROJECT_SOURCE_DIR}/src/detector-cmake-adaptor/darknet/src/darknet.ico"
)

option(
  ENABLE_DOCKER_DARKNET_PREDICTOR
  "Set to ON to enable the predictor docker container runs"
  OFF
)

option(
  ENABLE_DOCKER_DARKNET
  "Set to ON to enable the darkent dockers targets"
  OFF
)

set(
  DEFAULT_LOG_FILE_PATH ""
  CACHE STRING 
  "Sets the default log file path"
)

option(
  ENABLE_DOCKER_DARKNETXX_CPU_TRAIN
  "Set to ON to enable the darkentxx CPU train docker target"
  OFF
)

option(
  ENABLE_OPENMP
  "Set to ON to enable the darkentxx OpenMP code parallelisation "
  ON
)

option(
  ENABLE_DOCKER_DARKNETXX_PREDICTOR
  "Set to ON to enable the darkentxx CPU predictor docker target"
  OFF
)

option(
  ENABLE_GPROF
  "Set to ON to enable the gprof application profiler analysis"
  OFF
)

option(
  ENABLE_CALLGRIND
  "Set to ON to enable the valgrind (callgrind) application profiler analysis"
  OFF
)

option(
  ENABLE_FP_TRAPS
  "Set to ON to terminate the application on a floating point division by zero, invalid operation or overflow (a diagnostic: the Darknet computations rely on the IEEE semantics of them here and there)"
  OFF
)

set(
  PROFILED_RUN_ARGS
  "--detect-cfg;${PROJECT_SOURCE_DIR}/src/NetworksLoaders/OrigDefaultLoader/tests/component/OrigDefaultLoader/data/1.cfg;--weights;${PROJECT_SOURCE_DIR}/src/NetworksLoaders/OrigDefaultLoader/tests/component/OrigDefaultLoader/data/1.weights;--names;${PROJECT_SOURCE_DIR}/src/NetworksLoaders/OrigDefaultLoader/tests/component/OrigDefaultLoader/data/1.names;--image;${PROJECT_SOURCE_DIR}/misc/data/dog.jpg;--dont-show"
  CACHE STRING
  "The darknetxx command line arguments (a semicolon separated list) of the valgrind, callgrind and gprof-analyze target runs, a short detection on the bundled test data by default"
)

set(Darknet_data_sub_install "share/darknetxx/${PROJECT_VERSION}")
set(Darknet_data_path "${CMAKE_INSTALL_PREFIX}/${Darknet_data_sub_install}/")

set(
  DOCKER_STARTER_NUM_THREADS 5
  CACHE STRING 
  "Sets the docker trainer script number of threads"
)

set(
  DOCKER_STARTER_NET_NTH_RESIZE 10
  CACHE STRING 
  "Sets the docker trainer script number of net every Nth resize"
)

set(
  DOCKER_STARTER_NTH_ITER_DATA_RELOAD 0
  CACHE STRING 
  "Sets the docker trainer script nth train iteration data reload"
)

set(
  DOCKER_STARTER_AVG_LOSS_LIMIT 0.0
  CACHE STRING 
  "Sets the docker trainer the avg loss limit to stop training"
)

string(TIMESTAMP PROJECT_CONFIGURE_DATE "%Y-%m-%d %H:%M:%S")

if(GIT_FOUND)
  execute_process(
    COMMAND "${GIT_EXECUTABLE}" rev-parse HEAD
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    OUTPUT_VARIABLE PROJECT_GIT_COMMIT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
  )

  execute_process(
    COMMAND "${GIT_EXECUTABLE}" branch --show-current
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    OUTPUT_VARIABLE PROJECT_CURRENT_BRANCH_NAME
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
  )
endif()

file(
  GLOB_RECURSE 
  ALLSOURCES 
  LIST_DIRECTORIES false 
  CONFIGURE_DEPENDS
  "src/*.h" "src/*.cpp" "src/*.c" "src/*.cu"
)

message(STATUS "PROJECT_NAME: ${PROJECT_NAME}")
message(STATUS "DARKNETXX_BINARY_NAME: ${DARKNETXX_BINARY_NAME}")
message(STATUS "PROJECT_CONFIGURE_DATE: ${PROJECT_CONFIGURE_DATE}")

