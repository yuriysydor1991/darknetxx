cmake_minimum_required(VERSION 3.13)

# ---------------------------------------------------------------------------
# The converter only binary: the network weights files format conversion of
# the library (the original darknet weights files, the dxxwjz1 and the dxxwjz2
# ones, see the src/lib/darknet-adaptor/adaptors/NetworksLoaders directory)
# alone.
#
# It is built out of the very same src/app component and the very same
# main.cpp as the main binary, while the DARKNETXX_CONVERTER_ONLY definition
# below tells the app::ApplicationFactory to run the conversion the command
# line asks for alone and the help and the version printers to name the
# converter binary.
# ---------------------------------------------------------------------------
if(NOT ENABLE_DARKNETXX_CONVERTER)
  message(STATUS
    "The ${DARKNETXX_CONVERTER_BINARY_NAME} target is excluded "
    "(ENABLE_DARKNETXX_CONVERTER=OFF)")
  return()
endif()

darknetxx_add_app_target(${DARKNETXX_CONVERTER_BINARY_NAME})

target_compile_definitions(
  ${DARKNETXX_CONVERTER_BINARY_NAME}
  PRIVATE DARKNETXX_CONVERTER_ONLY
)

list(APPEND DARKNETXX_APP_TARGETS ${DARKNETXX_CONVERTER_BINARY_NAME})
