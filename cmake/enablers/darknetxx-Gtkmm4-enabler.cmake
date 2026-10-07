cmake_minimum_required(VERSION 3.13)

# ---------------------------------------------------------------------------
# The GTKmm 4 toolkit of the GUI binary (see the src/gui directory), probed
# once at the project scope for the GUI sources and their tests.
#
# The imported PkgConfig::GTKMM target hands it's include directories to the
# targets linking it as the system ones, so the strict warnings of the project
# stay quiet about the toolkit headers.
# ---------------------------------------------------------------------------
if(NOT ENABLE_DARKNETXX_GUI OR NOT ENABLE_DARKNETXX_EXECUTABLES)
  return()
endif()

find_package(PkgConfig REQUIRED)

pkg_check_modules(GTKMM REQUIRED IMPORTED_TARGET gtkmm-4.0)

# embeds the window description and it's resources into the GUI binary
find_program(GLIB_COMPILE_RESOURCES_EXEC glib-compile-resources REQUIRED)

message(STATUS "GTKmm 4 version: ${GTKMM_VERSION}")
