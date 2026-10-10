cmake_minimum_required(VERSION 3.13)

# The options GCC and Clang share, to which the darknetxx-compile-options-GCC
# and the darknetxx-compile-options-Clang modules add the own ones of each.

# -D_FORTIFY_SOURCE=2 (needs OPT);

set(EXTRA_COMPILE_DEFINITIONS "${EXTRA_COMPILE_DEFINITIONS}")

# The warnings of the C++ alone stay away from the C sources some tests compile
# with the directory compile options.
set(EXTRA_COMPILE_OPTIONS
  ${EXTRA_COMPILE_OPTIONS}
  -Wall
  -Wextra
  -Wpedantic
  -Wshadow
  $<$<COMPILE_LANGUAGE:CXX>:-Wnon-virtual-dtor>
  $<$<COMPILE_LANGUAGE:CXX>:-Wold-style-cast>
  -Wcast-align
  -Wunused
  $<$<COMPILE_LANGUAGE:CXX>:-Woverloaded-virtual>
  -Wconversion
  -Wsign-conversion
  -Wnull-dereference
  -Wdouble-promotion
  -Wformat=2
  -Wimplicit-fallthrough
  -Wmisleading-indentation
  $<$<COMPILE_LANGUAGE:CXX>:-Wzero-as-null-pointer-constant>
  -fstack-protector-all
  -fno-common
  -fstrict-overflow
)

# The full RELRO of the ELF binaries takes the linker options, which the
# compile options never reach.
if (UNIX AND NOT APPLE)
  set(EXTRA_LINK_OPTIONS ${EXTRA_LINK_OPTIONS} -Wl,-z,relro -Wl,-z,now)
endif()

if (COMPILE_WARNINGS_AS_ERRORS)
  set(EXTRA_COMPILE_OPTIONS ${EXTRA_COMPILE_OPTIONS} -Werror)
endif()

# The COMPILE_WARNINGS_AS_ERRORS stays with the C++ sources on purpose: the
# vendored original Darknet C sources compile with plenty of the warnings.

set(
  EXTRA_C_COMPILE_OPTIONS
  ${EXTRA_C_COMPILE_OPTIONS}
  -Wall
  -Wextra
  -Wpedantic
  -Wshadow
  -Werror=implicit-function-declaration
)

# GCC 14+ promoted several legacy C diagnostics to errors by default. The
# vendored original Darknet C sources still trip them, so keep them as warnings
# (the project-specific implicit-function-declaration stays an error above).
# A compiler rejects a -Wno-error=<name> of a diagnostic it doesn't know (for
# example the -Wreturn-mismatch appeared in the GCC 14 only), so each one is
# passed only when the compiler accepts it.
include(CheckCCompilerFlag)

set(C_DEMOTED_ERRORS_OPTIONS "")

foreach(
  demoted_error
  incompatible-pointer-types
  int-conversion
  implicit-int
  return-mismatch
)
  string(MAKE_C_IDENTIFIER "C_HAS_WNO_ERROR_${demoted_error}" demoted_error_var)

  check_c_compiler_flag("-Wno-error=${demoted_error}" ${demoted_error_var})

  if (${demoted_error_var})
    list(APPEND C_DEMOTED_ERRORS_OPTIONS "-Wno-error=${demoted_error}")
  endif()
endforeach()

set(
  EXTRA_C_COMPILE_OPTIONS
  ${EXTRA_C_COMPILE_OPTIONS}
  ${C_DEMOTED_ERRORS_OPTIONS}
)

# The Darknet GEMM, im2col and activation kernels have the AVX/FMA code paths,
# which are compiled with the matching instruction set flags only (the
# ENABLE_SSE_AND_AVX_FLAGS option of the original Darknet CMake project). The
# flags go to the optimized builds of the Darknet C core only.
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64|amd64|x86|i[3-6]86)$" AND
   CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  set(SSE_AND_AVX_FLAGS_DEFAULT ON)
else()
  set(SSE_AND_AVX_FLAGS_DEFAULT OFF)
endif()

option(
  ENABLE_SSE_AND_AVX_FLAGS
  "Set to ON to compile the AVX/FMA kernels of the Darknet C core in the non Debug builds (x86 only; the binary needs an AVX2 capable CPU then)"
  ${SSE_AND_AVX_FLAGS_DEFAULT}
)

if(ENABLE_SSE_AND_AVX_FLAGS)
  foreach(
    sse_and_avx_flag
    -ffp-contract=fast -mavx -mavx2 -mfma -msse3 -msse4.1 -msse4.2
  )
    list(
      APPEND EXTRA_C_COMPILE_OPTIONS
      $<$<NOT:$<CONFIG:Debug>>:${sse_and_avx_flag}>
    )
  endforeach()
endif()

set(EXTRA_C_LINK_OPTIONS "")

if (ENABLE_OPENMP)
  set(EXTRA_COMPILE_OPTIONS ${EXTRA_COMPILE_OPTIONS} -fopenmp)
  set(EXTRA_C_COMPILE_OPTIONS ${EXTRA_C_COMPILE_OPTIONS} -fopenmp)
  set(EXTRA_LINK_OPTIONS ${EXTRA_LINK_OPTIONS} -fopenmp)
  set(EXTRA_C_LINK_OPTIONS -fopenmp)
  set(
    EXTRA_COMPILE_DEFINITIONS 
    ${EXTRA_COMPILE_DEFINITIONS}
    OPENMP=1
    Darknet_data_path=${Darknet_data_path}
  )
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(
    EXTRA_COMPILE_DEFINITIONS 
    ${EXTRA_COMPILE_DEFINITIONS}
    DEBUG=1
  )
endif()

if(ENABLE_FP_TRAPS)
  set(
    EXTRA_COMPILE_DEFINITIONS
    ${EXTRA_COMPILE_DEFINITIONS}
    ENABLE_FP_TRAPS=1
  )
endif()

