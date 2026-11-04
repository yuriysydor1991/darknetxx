## Required tools for the GNU/Linux based OS

In order to build minimum template project install the GCC C++ compiler with CMake and Git, along with the OpenCV development files the Darknet code depends on and the zlib and the nlohmann JSON ones the [darknetxx-datadrivers](https://github.com/yuriysydor1991/darknetxx-datadrivers) library of the network weights files is built with (it's configure fetches the two latter libraries through the available connection when they are missing), the nlohmann JSON and the zlib ones being the libraries the tests of this project read the JSON documents of the library with as well (see the [Enabling the nlohmann JSON library](/doc/sections/en_US/5-18-enabling-the-nlohmann-json-library.md) and [The zlib library](/doc/sections/en_US/5-23-enabling-the-zlib-library.md) sections).

```
sudo apt install -y git g++ cmake libopencv-dev zlib1g-dev nlohmann-json3-dev
```

The project is built against the darknetxx-datadrivers library, the network weights files and the training projects formats of it, and the library is no distribution package nor is it fetched by the configure of the project, which only looks it up with the `find_package()`: build and install it before the project is configured. The shipped script clones, builds and installs it in one command with the tools above:

```
# inside the project root directory

misc/ci/install-darknetxx-datadrivers.sh
```

It installs into the `/usr/local` prefix; pass another prefix as it's only argument and point the configure at it with the `-DCMAKE_PREFIX_PATH=<prefix>` (see the [Enabling the darknetxx-datadrivers library](/doc/sections/en_US/5-39-enabling-the-darknetxx-datadrivers-library.md) section).

The [GUI binaries](/doc/sections/en_US/5-36-the-GUI-binaries.md) require the GTKmm 4 development files as well (Debian, Ubuntu 24.04 or newer), unless built without it by the `-DENABLE_DARKNETXX_GUI=OFF` or without any executable by the `-DENABLE_DARKNETXX_EXECUTABLES=OFF` (see the [Using the darknetxx library](/doc/sections/en_US/5-37-using-the-darknetxx-library.md) section):

```
sudo apt install -y libgtkmm-4.0-dev
```

The Qt 6 [network editor binary](/doc/sections/en_US/5-38-the-network-editor-binaries.md) is built as soon as the Qt 6 Widgets and Svg development files are found:

```
sudo apt install -y qt6-base-dev qt6-svg-dev
```

The [scripts/packages](/scripts/packages) directory holds the scripts which install the minimal set of packages required to build and run the project locally, the GTest framework of the tests included:

| Script | Installs the packages of |
| --- | --- |
| [install-debian.sh](/scripts/packages/install-debian.sh) | Debian, by the `apt-get install` command |
| [install-ubuntu.sh](/scripts/packages/install-ubuntu.sh) | Ubuntu, which shares the package set with Debian |

```
# inside the project root directory

scripts/packages/install-ubuntu.sh
```

The scripts ask for the `sudo` password when started by a regular user. They install the distribution packages alone and remind of the `misc/ci/install-darknetxx-datadrivers.sh` script above at the end, the one of the darknetxx-datadrivers library.
