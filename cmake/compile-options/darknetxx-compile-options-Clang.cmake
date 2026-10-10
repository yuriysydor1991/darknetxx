cmake_minimum_required(VERSION 3.13)

include(darknetxx-compile-options-GCC-and-Clang)

# the Clang has no -fanalyzer, the ENABLE_CLANG_TIDY target analyzes the code
if (ENABLE_COMPILER_CODE_ANALYZER)
  message(WARNING "The ENABLE_COMPILER_CODE_ANALYZER is of the GCC alone, the Clang build ignores it")
endif()
