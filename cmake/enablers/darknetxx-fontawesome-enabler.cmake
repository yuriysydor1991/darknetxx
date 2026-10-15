cmake_minimum_required(VERSION 3.13)

# ---------------------------------------------------------------------------
# The Font Awesome Free icons of the controls of the network editors (the
# appFontAwesome branch of the cpp-app-template): the shallow single tag
# checkout of the whole icon set kept intact, the icons of interest of which
# the editors embed (see the darknetxx_fontawesome_write_gresource() below). The editors show the icons of the
# icon theme for the option turned OFF.
#
#   -DDARKNETXX_FONTAWESOME_DIR=/opt/Font-Awesome  -> the checkout to reuse,
#                                                     no download
# ---------------------------------------------------------------------------
option(
  ENABLE_FONTAWESOME
  "Set to ON to embed the Font Awesome Free icons of the network editor controls (by reusing an already present checkout or by fetching one through the Internet)"
  ON
)

set(
  DARKNETXX_FONTAWESOME_GIT "https://github.com/FortAwesome/Font-Awesome.git"
  CACHE STRING "The Font Awesome Free icon set git source repository"
)

set(
  DARKNETXX_FONTAWESOME_GIT_TAG "7.3.1"
  CACHE STRING "The Font Awesome project git repository tag of interest"
)

set(
  DARKNETXX_FONTAWESOME_DIR ""
  CACHE PATH "An already available Font Awesome checkout to reuse instead of downloading one (leave empty to download)"
)

# The solid icons of the editor controls (see the
# src/gui/net-editor/EditorIcons.h).
set(
  DARKNETXX_FONTAWESOME_ICONS
  file-circle-plus folder-open floppy-disk file-export plus minus grip-vertical
)

if(NOT ENABLE_FONTAWESOME OR NOT DARKNETXX_NET_EDITOR_GUIS)
  return()
endif()

# Font Awesome is an asset repository and not a library: it ships no
# CMakeLists.txt and nothing to link against, hence the checkout alone.
if(DARKNETXX_FONTAWESOME_DIR)
  message(STATUS "Reusing the Font Awesome checkout: ${DARKNETXX_FONTAWESOME_DIR}")

  set(DARKNETXX_FONTAWESOME_ROOT_DIR "${DARKNETXX_FONTAWESOME_DIR}")
else()
  message(STATUS "Trying to make the Font Awesome icon set available through the Internet")
  message(STATUS "Font Awesome URL: ${DARKNETXX_FONTAWESOME_GIT}")
  message(STATUS "Font Awesome Tag: ${DARKNETXX_FONTAWESOME_GIT_TAG}")

  include(FetchContent)

  FetchContent_Declare(
    fontawesome
    GIT_REPOSITORY ${DARKNETXX_FONTAWESOME_GIT}
    GIT_TAG        ${DARKNETXX_FONTAWESOME_GIT_TAG}
    GIT_SHALLOW    TRUE
  )

  FetchContent_MakeAvailable(fontawesome)

  set(DARKNETXX_FONTAWESOME_ROOT_DIR "${fontawesome_SOURCE_DIR}")
endif()

set(DARKNETXX_FONTAWESOME_SVGS_DIR "${DARKNETXX_FONTAWESOME_ROOT_DIR}/svgs/solid")
set(DARKNETXX_FONTAWESOME_LICENSE_FILE "${DARKNETXX_FONTAWESOME_ROOT_DIR}/LICENSE.txt")

foreach(icon IN LISTS DARKNETXX_FONTAWESOME_ICONS)
  if(NOT EXISTS "${DARKNETXX_FONTAWESOME_SVGS_DIR}/${icon}.svg")
    message(
      FATAL_ERROR
      "No ${icon} icon inside the ${DARKNETXX_FONTAWESOME_SVGS_DIR} directory. "
      "Point the DARKNETXX_FONTAWESOME_DIR variable to a complete Font Awesome "
      "Free checkout or leave it empty to download one."
    )
  endif()
endforeach()

message(STATUS "Font Awesome Free icons: ${DARKNETXX_FONTAWESOME_SVGS_DIR}")

include(GNUInstallDirs)

# the icons carry their license, the CC BY 4.0
install(
  FILES "${DARKNETXX_FONTAWESOME_LICENSE_FILE}"
  DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/doc/${DARKNETXX_BINARY_NAME_lower}"
  RENAME "fontawesome-LICENSE.txt"
)

# Writes the manifest only when it's content differs from the already written
# one, so a repeated configure run rebuilds no resources.
function(darknetxx_fontawesome_write_manifest outputFile content)
  file(WRITE "${outputFile}.tmp" "${content}")
  configure_file("${outputFile}.tmp" "${outputFile}" COPYONLY)
endfunction()

# @brief Writes the GResource manifest (the .gresource.xml) of the editor
# icons: the darknetxx-<name>-symbolic.svg of the scalable actions of the
# icon theme path of the prefix, which the GTK recolors as the symbolic icons.
function(darknetxx_fontawesome_write_gresource outputFile prefix)
  set(entries "")

  foreach(icon IN LISTS DARKNETXX_FONTAWESOME_ICONS)
    string(APPEND entries
      "    <file alias=\"darknetxx-${icon}-symbolic.svg\">"
      "${DARKNETXX_FONTAWESOME_SVGS_DIR}/${icon}.svg</file>\n")
  endforeach()

  darknetxx_fontawesome_write_manifest(
    "${outputFile}"
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<gresources>\n  <gresource prefix=\"${prefix}/scalable/actions\">\n${entries}  </gresource>\n</gresources>\n")
endfunction()
