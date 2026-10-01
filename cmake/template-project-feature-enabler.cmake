cmake_minimum_required(VERSION 3.13)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/tools")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/enablers")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/enablers/compression")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/enablers/packagers")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/profilers")

include(template-project-enabler-function)

# Included before the sources are added, since they extend the compile options.
if(ENABLE_GPROF)
  include(template-project-profiler-gprof)
endif()

if(ENABLE_CALLGRIND)
  include(template-project-profiler-valgrind-callgrind)
endif()

if(ENABLE_UNIT_TESTS OR ENABLE_COMPONENT_TESTS OR ENABLE_INTEGRATION_TESTS)
  enable_testing()
  include(template-project-GTest-enabler)
endif()

if(ENABLE_CLANGFORMAT)
  include(template-project-clang-format-target)
endif()

if(ENABLE_CPPCHECK)
  include(template-project-cppcheck-target)
endif()

if(ENABLE_VALGRIND)
  include(template-project-valgrind-target)
endif()

if(ENABLE_CLANG_TIDY)
  include(template-project-clang-tidy-target)
endif()

if (ENABLE_SANITIZERS OR ENABLE_SANITIZERS_THREADS)
  include(template-project-sanitizers)
endif()

# The dxxwjz1 network weights files (see the
# src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1 directory) are the gzip
# compressed JSON documents, which the zlib and the nlohmann json libraries
# read and write, so neither of them can be left out. The option turned OFF is
# turned back ON: the cache of a build directory configured before the files
# keeps the OFF default the nlohmann json option had then.
foreach(required_library_option ENABLE_ZLIB ENABLE_NLOHMANN_JSON)
  get_property(
    required_library_option_cached
    CACHE ${required_library_option}
    PROPERTY TYPE
    SET
  )

  if (required_library_option_cached AND NOT ${required_library_option})
    message(
      WARNING
      "The dxxwjz1 network weights files require the library of the "
      "${required_library_option} option, the option is turned ON"
    )
    set_property(CACHE ${required_library_option} PROPERTY VALUE ON)
  endif()
endforeach()

# Each of the enablers declares or checks its ENABLE_* option and returns early
# when disabled, so they are included unconditionally.
include(template-project-zlib-enabler)
include(template-project-nlohmann-json-enabler)

# The packagers come after the libraries above, since the packages build them
# as well (the flatpak manifest names the nlohmann json library repository and
# tag of its enabler, for instance).
if(ENABLE_DEB)
  include(template-project-deb-enabler)
endif()

if(ENABLE_FLATPAK)
  include(template-project-flatpak-target)
endif()

# Additional packagers. Each module self-declares its ENABLE_* option and
# returns early when disabled, so they are included unconditionally.
include(template-project-snap-enabler)
include(template-project-rpm-enabler)
include(template-project-freebsd-pkg-enabler)
include(template-project-wix-enabler)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

