cmake_minimum_required(VERSION 3.13)

# The option itself is declared in the darknetxx-misc-variables-declare
# module along with the rest of the project options.
if (NOT ENABLE_NLOHMANN_JSON)
  return()
endif()

set(DARKNETXX_NLOHMANN_GIT "https://github.com/nlohmann/json.git" CACHE STRING "The Nlohmann JSON library git source repository")
# a release tag rather than the moving master branch, so every build of the
# project gets one and the same library
set(DARKNETXX_NLOHMANN_GIT_TAG "v3.11.3" CACHE STRING "The Nlohmann JSON project git repository tag of interest")

# the fetched library is used as a subproject, its tests and install rules
# are of no use for the build
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(NLOHMANN_force_shared_crt ON CACHE BOOL "" FORCE)

darknetxx_default_3rdparty_enabler(
  NAME nlohmann_json
  GIT_REPOSITORY ${DARKNETXX_NLOHMANN_GIT}
  GIT_TAG ${DARKNETXX_NLOHMANN_GIT_TAG}
)

if (NOT TARGET nlohmann_json::nlohmann_json)
  message(FATAL_ERROR "The nlohmann json library made available provides no nlohmann_json::nlohmann_json target")
endif()

# Link the nlohmann_json::nlohmann_json target to your target(s) of interest,
# for example:
#   target_link_libraries(${DARKNETXX_BINARY_NAME} nlohmann_json::nlohmann_json)
#
# The darknetxx links it into the dxxwjz1 network weights files reader and
# writer (see the src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1
# directory).
