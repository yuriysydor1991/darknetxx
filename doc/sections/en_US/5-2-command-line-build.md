## Command line build

The project uses the CMake build system so a build command should be familiar to the most of the C++ developers around the world!

Of course, project needs to be `git clone`-ed and it's root directory must be opened in the terminal or in your IDE first. After that you may execute next commands:

```
# from the project root

cmake -B build -S . && cmake --build build
```

Which effectively will create a directory named `build` (it's already added to the `.gitignore` list), configure project using the CMake available in the system (see the [Requirements](/doc/sections/en_US/3-requirements.md) section of this `README.md` file) and finally builds all the targets available in the project.

Without an explicit `CMAKE_BUILD_TYPE` the project is configured as an optimized `Release` build, because the Darknet computations run an order of magnitude slower without the optimization. Pass `-DCMAKE_BUILD_TYPE=Debug` to the configure command for a debug build.

The optimized builds of the x86 GCC and Clang compilers also compile the AVX and FMA kernels of the Darknet C core (the `ENABLE_SSE_AND_AVX_FLAGS` CMake option, which is ON by default there), so the resulting binary needs an AVX2 capable CPU. Pass `-DENABLE_SSE_AND_AVX_FLAGS=OFF` to the configure command to build a binary (or a package) for the older CPUs. The snap and the flatpak recipes of the project pass it already, since those packages run on other machines.
