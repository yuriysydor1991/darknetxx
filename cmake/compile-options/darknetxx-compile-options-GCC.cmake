cmake_minimum_required(VERSION 3.13)

include(darknetxx-compile-options-GCC-and-Clang)

# the warnings of the GCC alone, which the Clang reports unknown
set(EXTRA_COMPILE_OPTIONS
  ${EXTRA_COMPILE_OPTIONS}
  -Wduplicated-cond
  -Wduplicated-branches
  -Wlogical-op
  $<$<COMPILE_LANGUAGE:CXX>:-Wuseless-cast>
)

if (ENABLE_COMPILER_CODE_ANALYZER)
  set(EXTRA_COMPILE_OPTIONS ${EXTRA_COMPILE_OPTIONS} -fanalyzer)
  # the GCC analyzer is at it's best with the C code of the Darknet core
  set(EXTRA_C_COMPILE_OPTIONS ${EXTRA_C_COMPILE_OPTIONS} -fanalyzer)
endif()
