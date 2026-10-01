## Enabling the zlib library

The [zlib](https://www.zlib.net/) compression library (deflate, gzip, zip) compresses the dxxwjz1 network weights files (see the [Network weights files](/doc/sections/en_US/4-7-4-network-weights-files.md) section) through the `zlibi::ZlibController` (see the [Supporting components](/doc/sections/en_US/4-7-3-supporting-components.md) section), so the project requires it and the `ENABLE_ZLIB` CMake variable is `ON` by default. The installed system library is used when found (the `zlib1g-dev` package of Debian and Ubuntu), otherwise the configure fetches and builds the `v1.3.1` release of it through the available connection. The variable can't be turned `OFF`: the configure turns it back `ON` with a warning.

```
# Inside the source root directory

cmake -S . -B build -DENABLE_ZLIB=ON
```

See the `cmake/enablers/compression/template-project-zlib-enabler.cmake` module for the `ZLIB::ZLIB` target to link to your targets of interest: the system library one, or the alias of the static library of the FetchContent build.
