cmake_minimum_required(VERSION 3.13)

option(
  ENABLE_DARKNETXX_DATA_DRIVERS
  "Set to ON to enable the darknetxx-datadrivers network files formats library (system wide installed)"
  ON
)

# The option is honoured the same way every other third party enabler of the
# project honours its own, but unlike the optional ones this library is what
# reads and writes the network weights files and the training projects, so
# switching it off leaves the sources without the weights documents, the
# formats and the converter they are written against. Say so here instead of
# letting the configure succeed and the compilation fail on a missing
# <DxxDataDriversFacade.h>.
if (NOT ENABLE_DARKNETXX_DATA_DRIVERS)
  message(WARNING
    "ENABLE_DARKNETXX_DATA_DRIVERS is OFF - the project will not compile: "
    "the network weights files, the training projects and their conversion "
    "come from that library.")
  return()
endif()

# The library carries its major and minor version inside the installed package,
# target and binary names, matching its version carrying public namespace, so
# that two minor versions install completely side by side. The default below is
# what the library produces out of the box; its own LIB_INCLUDE_MINOR_IN_NAME
# (ON by default), LIB_INCLUDE_MICRO_IN_NAME and LIB_NAME_SUFFIX options may
# shorten or extend that name. The name the find_package() probe asks for is
# therefore a cache variable rather than a literal, so a differently named
# build of the very same library - a major-only "DarknetxxDataDrivers-0" or a
# "DarknetxxDataDrivers-0.12-dev" one - is found by a single -D on the command
# line. The headers of such a build are installed under the include directory
# of it's name as well, while the src/NetworksLoaders/DataDriversAliases.h
# includes the <DarknetxxDataDrivers-0.12/...> one, so the alias header follows
# the name of such a build too.
set(
  DARKNETXX_DXXDD_PACKAGE "DarknetxxDataDrivers-0.12"
  CACHE STRING "The darknetxx-datadrivers CMake package (and target) name to look for"
)

# Where the library comes from. Nothing in this build fetches it - the two
# variables are what the packager manifests, which have to build it inside
# their own sandbox, are configured with (the misc/packagers/flatpak.conf.json.in
# and the misc/packagers/snapcraft.yaml.in ones, the misc/Dockerfiles/*.in
# images and the misc/ci/install-darknetxx-datadrivers.sh script of the
# pipelines).
set(
  DARKNETXX_DXXDD_GIT "https://github.com/yuriysydor1991/darknetxx-datadrivers.git"
  CACHE STRING "The darknetxx-datadrivers library git source repository"
)
set(
  DARKNETXX_DXXDD_GIT_TAG "main"
  CACHE STRING "The darknetxx-datadrivers project git repository tag of interest"
)

# No Internet fallback: the library is consumed installed, the way its own
# "Using the library in your project" documentation section describes, and its
# headers are included through the versioned <DarknetxxDataDrivers-0.12/...>
# directory only an installation provides (see the
# src/NetworksLoaders/DataDriversAliases.h), which pins the library version
# every translation unit is talking to.
#
# The package carries its <package>ConfigVersion.cmake of the same minor
# version compatibility, while the package name itself carries the major and
# the minor version already, so no version is requested here.
find_package(${DARKNETXX_DXXDD_PACKAGE} QUIET CONFIG)

if (NOT ${DARKNETXX_DXXDD_PACKAGE}_FOUND)
  message(FATAL_ERROR
    "The ${DARKNETXX_DXXDD_PACKAGE} library was not found in the system. "
    "Build and install it from ${DARKNETXX_DXXDD_GIT} (the "
    "misc/ci/install-darknetxx-datadrivers.sh script does), point the project "
    "at it with -DCMAKE_PREFIX_PATH=<prefix>, or correct the package name with "
    "-DDARKNETXX_DXXDD_PACKAGE=<name>. The project does not build without it: "
    "the network weights files and the training projects are read and written "
    "exclusively by this library.")
endif()

set(DXXDD_LIBRARIES ${DARKNETXX_DXXDD_PACKAGE}::${DARKNETXX_DXXDD_PACKAGE})

message(STATUS "darknetxx-datadrivers target: ${DXXDD_LIBRARIES}")

# The library supplies the weights documents, the formats registry, the
# network sources, the cfg sections and the converter, which the object
# libraries all over the src subtree (the darknet adaptor, the loaders, the
# training, the recognition, the library facade, the GUIs and the tests)
# compile against. Propagate its usage requirements through link_libraries() to
# every target built below instead of only to the final library, so the object
# libraries and the test executables compile and link. This is why the enabler
# is included before add_subdirectory(src) (see the darknetxx-feature-enabler).
link_libraries(${DXXDD_LIBRARIES})
