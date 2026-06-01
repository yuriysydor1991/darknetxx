cmake_minimum_required(VERSION 3.13)

include(template-project-compile-options)
add_compile_options(${EXTRA_COMPILE_OPTIONS})
add_link_options(${EXTRA_LINK_OPTIONS})
add_compile_definitions(${EXTRA_COMPILE_DEFINITIONS})

# GCC 14+ turned several legacy C diagnostics into errors by default. Keep them
# as warnings for the vendored original Darknet C sources (compiled directly by
# some component tests), without relaxing the C++ strictness.
add_compile_options(
  $<$<COMPILE_LANGUAGE:C>:-Wno-error=incompatible-pointer-types>
  $<$<COMPILE_LANGUAGE:C>:-Wno-error=int-conversion>
  $<$<COMPILE_LANGUAGE:C>:-Wno-error=implicit-int>
  $<$<COMPILE_LANGUAGE:C>:-Wno-error=return-mismatch>
)

add_executable(
  ${PROJECT_BINARY_NAME}
  main.cpp
)

target_include_directories(
  ${PROJECT_BINARY_NAME}
  PRIVATE ${CMAKE_SOURCE_DIR}
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${CMAKE_BINARY_DIR}
)

install(
  TARGETS ${PROJECT_BINARY_NAME}
  RUNTIME DESTINATION bin
)

configure_file(
  project-global-decls.h.in
  ${CMAKE_BINARY_DIR}/project-global-decls.h
)

add_subdirectory(app)
add_subdirectory(log)
add_subdirectory(darknet-adaptor)
add_subdirectory(scripts)
add_subdirectory(helpers)
add_subdirectory(OMP)

