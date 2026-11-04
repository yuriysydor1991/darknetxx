## Enabling DEB package generation with cpack

In order to be able to generate the `deb` package file configure project to support the `cpack` command feature with a `ENABLE_DEB` variable enabled:

```
# inside the project root directory 

cmake -B build -S . -DENABLE_DEB=ON
```

Next, build all available targets required for the `deb` package

```
# inside the project root directory

cmake --build build --target all
```

Finally, execute the `cpack` command inside the template project build directory:

```
# inside the project build directory

cpack
```

The package file should be generated inside the project build root directory. For example, if project name was't change and it's version is 0.8.0 so the package name may look like `darknetxx-0.8.0-Linux.deb`.

In order to examine details of the `deb` package configuration visit the `cmake/darknetxx-deb-enabler.cmake` file.

### The darknetxx-datadrivers library in the package

The [darknetxx-datadrivers](https://github.com/yuriysydor1991/darknetxx-datadrivers) library the binaries are linked against (see the [Enabling the darknetxx-datadrivers library](/doc/sections/en_US/5-39-enabling-the-darknetxx-datadrivers-library.md) section) is no distribution package, so the `dpkg-shlibdeps` scan of the package finds no package of it to depend on, and the package alone would start on a system which has the library installed already. The `ENABLE_DEB_DARKNETXX_DATA_DRIVERS` option, `ON` by default, packs the library into the package instead: the very shared object the found CMake package of the library points at, with it's soname link, goes into the `lib` directory the project installs it's own library into, so the package is complete on it's own, the way the ImagesAnnotator project packs it's own data libraries. It is the install rule of the build directory, so the `cmake --install` of the build configured so copies the library as well. The `darknetxxConverter` binary of the library isn't packed: it comes with the packages of the library itself (see [The darknetxxConverter binary](/doc/sections/en_US/5-35-the-darknetxxConverter-binary.md) section).

```
# inside the project root directory

# the package of no darknetxx-datadrivers library, for the systems which have it installed
cmake -B build -S . -DENABLE_DEB=ON -DENABLE_DEB_DARKNETXX_DATA_DRIVERS=OFF
```

| CMake option | Default | Effect |
| --- | --- | --- |
| `ENABLE_DEB_DARKNETXX_DATA_DRIVERS` | `ON` | Packs the shared object of the darknetxx-datadrivers library the binaries are linked against into the `deb` package. |
