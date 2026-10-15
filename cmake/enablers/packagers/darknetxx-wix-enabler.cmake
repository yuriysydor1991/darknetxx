cmake_minimum_required(VERSION 3.13)

option(
  ENABLE_WIX
  "Set to ON to enable the WIX MSI package creation from the project build artifacts (MS Windows)"
  OFF
)

if(NOT ENABLE_WIX)
    return()
endif()

include(InstallRequiredSystemLibraries)

set(
  DARKNETXX_WIX_UPGRADE_GUID
  "12345678-1234-1234-1234-123456789012"
  CACHE STRING
  "Stable GUID identifying the product across upgrades - MUST be replaced with a project-unique value for a real release"
)

set(
  DARKNETXX_WIX_PROGRAM_MENU_FOLDER
  "${PROJECT_NAME}"
  CACHE STRING
  "Windows Start Menu folder name where the installer creates shortcuts"
)

set(
  DARKNETXX_WIX_CULTURES
  "en-US"
  CACHE STRING
  "Semicolon-separated list of UI cultures used by the WIX installer (e.g. en-US;uk-UA)"
)

set(
  DARKNETXX_WIX_LICENSE_RTF
  ""
  CACHE STRING
  "Optional absolute path to a RTF or TXT license file shown by the MSI installer"
)

set(
  DARKNETXX_WIX_PRODUCT_ICON
  "${DARKNETXX_LOGO_ICO}"
  CACHE STRING
  "Absolute path to an .ico file used as the MSI product icon (the Darknet logo by default, empty for none)"
)

set(CPACK_GENERATOR "WIX")
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_CONTACT ${DARKNETXX_MAINTAINER_EMAIL})
set(CPACK_PACKAGE_VENDOR "${DARKNETXX_MAINTAINER}")
set(CPACK_PACKAGE_DESCRIPTION ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME})

set(CPACK_WIX_UPGRADE_GUID ${DARKNETXX_WIX_UPGRADE_GUID})
set(CPACK_WIX_PROGRAM_MENU_FOLDER ${DARKNETXX_WIX_PROGRAM_MENU_FOLDER})
set(CPACK_WIX_CULTURES ${DARKNETXX_WIX_CULTURES})
set(CPACK_WIX_ROOT_FOLDER_ID "ProgramFiles64Folder")

if(DARKNETXX_WIX_LICENSE_RTF)
  set(CPACK_WIX_LICENSE_RTF ${DARKNETXX_WIX_LICENSE_RTF})
endif()

if(DARKNETXX_WIX_PRODUCT_ICON)
  set(CPACK_WIX_PRODUCT_ICON ${DARKNETXX_WIX_PRODUCT_ICON})
endif()

# The GUI binaries are the desktop apps: the Start Menu and the desktop
# shortcuts of the installer start them.
foreach(GUI IN LISTS DARKNETXX_GTKMM_GUIS DARKNETXX_QT_GUIS)
  list(
    APPEND CPACK_PACKAGE_EXECUTABLES
    ${DARKNETXX_GUI_${GUI}_BINARY} ${DARKNETXX_GUI_${GUI}_BINARY}
  )
  list(APPEND CPACK_CREATE_DESKTOP_LINKS ${DARKNETXX_GUI_${GUI}_BINARY})
endforeach()

include(CPack)
