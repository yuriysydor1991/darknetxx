cmake_minimum_required(VERSION 3.13)

# -D_FORTIFY_SOURCE=2 (needs OPT);

set(EXTRA_COMPILE_DEFINITIONS "${EXTRA_COMPILE_DEFINITIONS}")

set(EXTRA_COMPILE_OPTIONS
  ${EXTRA_COMPILE_OPTIONS}
  -Wall
  -Wextra
  -Wpedantic
  -Wshadow
  -Wnon-virtual-dtor
  -Wold-style-cast
  -Wcast-align
  -Wunused
  -Woverloaded-virtual
  -Wconversion
  -Wsign-conversion
  -Wnull-dereference
  -Wdouble-promotion
  -Wformat=2
  -Wimplicit-fallthrough
  -Wduplicated-cond
  -Wduplicated-branches
  -Wlogical-op
  -Wuseless-cast
  -Wmisleading-indentation
  -Wzero-as-null-pointer-constant
  -fstack-protector-all
  -fno-common
  -fstrict-overflow
  -pie
  -Wl,-z,relro
  -Wl,-z,now
  -MMD
)

if (COMPILE_WARNINGS_AS_ERRORS)
  set(EXTRA_COMPILE_OPTIONS ${EXTRA_COMPILE_OPTIONS} -Werror)
endif()

if (ENABLE_COMPILER_CODE_ANALYZER)
  set(EXTRA_COMPILE_OPTIONS ${EXTRA_COMPILE_OPTIONS} -fanalyzer)
endif()

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
set(
  EXTRA_C_COMPILE_OPTIONS
  ${EXTRA_C_COMPILE_OPTIONS}
  -Wno-error=incompatible-pointer-types
  -Wno-error=int-conversion
  -Wno-error=implicit-int
  -Wno-error=return-mismatch
)

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

