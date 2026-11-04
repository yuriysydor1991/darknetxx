## Enabling the darknetxx-datadrivers library

The network weights files and the training projects - the original darknet weights files, the dxxwjz1, the dxxwjz2, the dxxwjz2p, the dxxwjz3, the dxxwjz3p, the dxxwjz4 and the dxxwjz4p ones and the YOLO4 training folders of the conversion - are read, written, validated and converted by the standalone [darknetxx-datadrivers](https://github.com/yuriysydor1991/darknetxx-datadrivers) library. Their documents, their readers, writers and validators, the converter between them, the cfg file sections and the zlib compression controller used to live inside this repository (the `src/NetworksLoaders` and the `src/zlib` directories) and were extracted into that library, made of the `applib` branch of the [cpp-app-template](https://github.com/yuriysydor1991/cpp-app-template), so the darknetxx and any other tool share one and the same implementation of the formats. The `darknetxxConverter` binary moved there with them (see [The darknetxxConverter binary](/doc/sections/en_US/5-35-the-darknetxxConverter-binary.md) section), while the main `darknetxx` binary keeps it's conversion command line (see the [Converting the network weights files](/doc/sections/en_US/6-2-4-converting-the-network-weights-files.md) section).

The library is therefore **mandatory**: the `ENABLE_DARKNETXX_DATA_DRIVERS` CMake option defaults to `ON` and the project does not build with it switched off - the configure only warns of the option turned `OFF`, while the sources, which are written against the library, don't compile without it.

```
# Inside the source root directory

cmake -S . -B build -DENABLE_DARKNETXX_DATA_DRIVERS=ON
```

The [cmake/enablers/darknetxx-DarknetxxDataDrivers-enabler.cmake](/cmake/enablers/darknetxx-DarknetxxDataDrivers-enabler.cmake) module looks the library up with the `find_package()` of it's CMake package configuration and stops the configure with an error which tells how to provide it when the library isn't found. There is no Internet fallback: the library is consumed installed, the way the [ImagesAnnotator](https://github.com/yuriysydor1991/ImagesAnnotator) project consumes it's own data libraries, and it's headers are included through the versioned `<DarknetxxDataDrivers-0.12/...>` directory only an installation provides. The module links the library to every target of the project (the `link_libraries()` of it), since the object libraries all over the [`src`](/src) directory and the test executables compile against it, so it is included before the sources are added.

### Building and installing the library

The library is no distribution package, so it is built of it's sources and installed before the project is configured. The [misc/ci/install-darknetxx-datadrivers.sh](/misc/ci/install-darknetxx-datadrivers.sh) script clones, builds (the `Release` build) and installs it in one command, the install prefix being it's only argument, the `/usr/local` one by default:

```
# inside the project root directory

# into the /usr/local prefix, the sudo password is asked for the install step
misc/ci/install-darknetxx-datadrivers.sh

# into a prefix of your own
misc/ci/install-darknetxx-datadrivers.sh /opt/darknetxx
```

The `DXXDD_GIT` and the `DXXDD_GIT_TAG` environment variables of the script name another repository and another branch or tag of it (the `main` branch of the [darknetxx-datadrivers](https://github.com/yuriysydor1991/darknetxx-datadrivers) repository by default), which is what a pipeline validating an unreleased revision of the library does:

```
# inside the project root directory

DXXDD_GIT_TAG=my-branch misc/ci/install-darknetxx-datadrivers.sh /opt/darknetxx
```

The very same steps by hand:

```
git clone https://github.com/yuriysydor1991/darknetxx-datadrivers.git
cmake -S darknetxx-datadrivers -B build-dxxdd -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/darknetxx
cmake --build build-dxxdd -j$(nproc)
cmake --install build-dxxdd
```

A library installed outside of the default search prefixes is found through the usual CMake variable, while such a prefix is complete for the installed binaries of the project once the loader knows about it, the `LD_LIBRARY_PATH` of the prefix for example (the script runs the `ldconfig` for the `/usr` and the `/usr/local` prefixes and reminds of the `LD_LIBRARY_PATH` for the rest):

```
# inside the project root directory

cmake -S . -B build -DCMAKE_PREFIX_PATH=/opt/darknetxx
cmake --build build

# the installed binaries of the project at the run time
LD_LIBRARY_PATH=/opt/darknetxx/lib darknetxx --version
```

The library itself is built with the zlib and the nlohmann JSON libraries (the system ones, or the ones it's configure fetches through the available connection) and, optionally, the OpenCV one, which reads the sizes of the YOLO4 folder images the `stb_image` header of the library can't read (see the [Required tools for the GNU/Linux based OS](/doc/sections/en_US/3-1-required-tools-for-the-GNU-Linux-based-OS.md) section). It installs:

| Installed file | Holds |
| --- | --- |
| `<prefix>/lib/libDarknetxxDataDrivers-0.12.so.0.12.0` with it's `.so.0` soname and `.so` namelink links | the shared library |
| `<prefix>/include/DarknetxxDataDrivers-0.12/*.h` | the public headers of the `DarknetxxDataDrivers012` namespace |
| `<prefix>/lib/cmake/DarknetxxDataDrivers-0.12/*.cmake` | the CMake package of the `DarknetxxDataDrivers-0.12::DarknetxxDataDrivers-0.12` target |
| `<prefix>/bin/darknetxxConverter` | the network weights files converter binary |

The `lib` directory is the `CMAKE_INSTALL_LIBDIR` one, the `lib/x86_64-linux-gnu` of the `/usr` prefix of the Debian and the Ubuntu, for example. The library documents it's formats, it's API and it's own build options in it's [repository](https://github.com/yuriysydor1991/darknetxx-datadrivers).

### Overriding the source repository and the tag

Nothing in this build fetches the library, but the packager manifests and the Docker images - the flatpak, the snap, the docker and the Jenkins pipeline ones - build it inside their own sandbox and take the repository from these two cache variables (see the [Enabling the flatpak package generation support](/doc/sections/en_US/5-11-enabling-the-flatpak-package-generation-support.md), the [Enabling the snap packager](/doc/sections/en_US/5-19-enabling-the-snap-package-manager.md) and the [Enabling the Docker container build and run](/doc/sections/en_US/5-12-enabling-the-docker-container-build-and-run.md) sections):

```
cmake -S . -B build \
  -DDARKNETXX_DXXDD_GIT=https://github.com/yuriysydor1991/darknetxx-datadrivers.git \
  -DDARKNETXX_DXXDD_GIT_TAG=main
```

### Overriding the package name

The library carries it's major and minor version inside the installed package, target and library names - `DarknetxxDataDrivers-0.12`, matching it's version carrying `DarknetxxDataDrivers012` public namespace - so two minor versions install completely side by side. It's own `LIB_INCLUDE_MINOR_IN_NAME` (`ON` by default), `LIB_INCLUDE_MICRO_IN_NAME` and `LIB_NAME_SUFFIX` options shorten or extend that name. The name the configure asks the `find_package()` for is the `DARKNETXX_DXXDD_PACKAGE` cache variable:

```
cmake -S . -B build -DDARKNETXX_DXXDD_PACKAGE=DarknetxxDataDrivers-0.12
```

The sources include the headers through the `<DarknetxxDataDrivers-0.12/...>` directory alone (see below), so the package of another name has to provide that very directory of the headers as well.

| CMake option | Default | Effect |
| --- | --- | --- |
| `ENABLE_DARKNETXX_DATA_DRIVERS` | `ON` | Looks the library up. `OFF` only warns: the project doesn't compile without it. |
| `DARKNETXX_DXXDD_PACKAGE` | `DarknetxxDataDrivers-0.12` | The CMake package (and target) name of the library to look for. |
| `DARKNETXX_DXXDD_GIT` | `https://github.com/yuriysydor1991/darknetxx-datadrivers.git` | The repository the packager manifests and the Docker images build the library of. |
| `DARKNETXX_DXXDD_GIT_TAG` | `main` | The branch or the tag of that repository. |

### What the project takes from the library

- the weights documents: the `WeightsDocument` of the `WeightsHeader` and the layers of the arrays, the `NetworkDescription`, the `TrainingRecord`, the `Yolo4Annotations` of the `Yolo4Image` and the `Yolo4Box`;
- the formats: the `WeightsFileFormats` registry of the `IWeightsFileFormat` formats and their `IWeightsReader` readers and `IWeightsWriter` writers, of the names and the file name extensions, the `NetworkSource` of the files networks are made of, the `NetworkFiles` reads of the cfg and the class names files and the `yolo4::Yolo4FolderLayout` of the YOLO4 folders;
- the cfg file sections, the `CfgSection` the network loader parses the cfg by and the `CfgError` it throws;
- the `WeightsConverter` of the `WeightsConversion` and the `INetworkWeights` interface of the networks the files are loaded into and saved of, with the `LayerType`, the `WeightsSchema` and the `WeightsLayout` of their arrays;
- the `DxxDataDriversFacade::init_logger`, which the `darknetxx012::LibraryFacade::init_logger` of the darknetxx library hands the logger of the application over to, so the messages of the formats land in the log of the application.

The single place naming the library is the [src/NetworksLoaders/DataDriversAliases.h](/src/NetworksLoaders/DataDriversAliases.h) header, which includes the `<DarknetxxDataDrivers-0.12/DxxDataDriversFacade.h>` header (it includes every other public header of the library) and declares the `dxxdd` alias of the versioned `DarknetxxDataDrivers012` namespace: the prefix pins the library version every translation unit is talking to, and the generic names such as the `CfgSection.h` and the `WeightsDocument.h` sit on the unprefixed include root beside it. The darknet networks of the formats stay in this project, see the [Network weights files](/doc/sections/en_US/4-7-4-network-weights-files.md) section.
