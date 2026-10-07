cmake_minimum_required(VERSION 3.13)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/sanitizers")

if (ENABLE_SANITIZERS)
  include(darknetxx-sanitizers-leak-and-address)
endif()

if (ENABLE_SANITIZERS_THREADS)
  include(darknetxx-sanitizers-threads)
endif()
