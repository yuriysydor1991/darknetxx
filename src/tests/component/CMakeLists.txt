cmake_minimum_required(VERSION 3.13)

# the generated networks of the weights factory are the ones of the component
# and of the integration tests
if (NOT ENABLE_COMPONENT_TESTS AND NOT ENABLE_INTEGRATION_TESTS)
  return()
endif()

add_subdirectory(weights-factory)
