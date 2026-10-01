cmake_minimum_required(VERSION 3.13)

# Makes a third party library available to the project: the system installed
# one when the find_package() finds it, the one fetched from the git
# repository and built along with the project otherwise.
#
#   template_project_default_3rdparty_enabler(
#     NAME <find_package name>
#     GIT_REPOSITORY <url>
#     GIT_TAG <tag>
#     [COMPONENTS <components>...]
#     [DISABLE_SYSTEM_PROBE]
#   )
#
# The <NAME>_FOUND variable tells the caller which one it got: it's true for
# the system library and false for the fetched one.
function(template_project_default_3rdparty_enabler)
  set(FCN_KEYWORDS_FLAGS DISABLE_SYSTEM_PROBE)
  set(FCN_KEYWORDS_SINGLE NAME GIT_REPOSITORY GIT_TAG)
  set(FCN_KEYWORDS_MULTI COMPONENTS)

  cmake_parse_arguments(
    "ARG"
    "${FCN_KEYWORDS_FLAGS}"
    "${FCN_KEYWORDS_SINGLE}"
    "${FCN_KEYWORDS_MULTI}"
    ${ARGN})

  if(ARG_COMPONENTS)
    set(COMPONENTS_STR COMPONENTS ${ARG_COMPONENTS})
  endif()

  if (NOT ARG_DISABLE_SYSTEM_PROBE)
    message(STATUS "Trying to probe system installed ${ARG_NAME}")
    find_package(${ARG_NAME} QUIET ${COMPONENTS_STR})
  endif()

  if (${ARG_NAME}_FOUND)
    message(STATUS "System already contains the ${ARG_NAME} library")
  else()
    message(STATUS "The '${ARG_NAME}' is not available in the system (or probing is OFF)")
    message(STATUS "Trying to make ${ARG_NAME} library available through the Internet")

    message(STATUS "${ARG_NAME} URL: ${ARG_GIT_REPOSITORY}")
    message(STATUS "${ARG_NAME} Tag: ${ARG_GIT_TAG}")

    include(FetchContent)

    # the history of the libraries is of no use for the build
    FetchContent_Declare(
        ${ARG_NAME}
        GIT_REPOSITORY ${ARG_GIT_REPOSITORY}
        GIT_TAG        ${ARG_GIT_TAG}
        GIT_SHALLOW    TRUE
    )

    FetchContent_MakeAvailable(${ARG_NAME})
  endif()

  set(${ARG_NAME}_FOUND ${${ARG_NAME}_FOUND} PARENT_SCOPE)

  message(STATUS "The project ${ARG_NAME} is made available")
endfunction()
