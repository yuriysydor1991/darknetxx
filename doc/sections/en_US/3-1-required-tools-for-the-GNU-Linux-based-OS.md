## Required tools for the GNU/Linux based OS

In order to build minimum template project install the GCC C++ compiler with CMake and Git, along with the OpenCV development files the Darknet code depends on and the zlib and the nlohmann JSON ones the dxxwjz1 network weights files depend on (the configure fetches the two latter libraries through the available connection when they are missing, see the [Enabling the zlib library](/doc/sections/en_US/5-23-enabling-the-zlib-library.md) and the [Enabling the nlohmann JSON library](/doc/sections/en_US/5-18-enabling-the-nlohmann-json-library.md) sections).

```
sudo apt install -y git g++ cmake libopencv-dev zlib1g-dev nlohmann-json3-dev
```

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

The scripts ask for the `sudo` password when started by a regular user.
