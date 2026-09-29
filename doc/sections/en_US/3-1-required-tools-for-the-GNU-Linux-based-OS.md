## Required tools for the GNU/Linux based OS

In order to build minimum template project install the GCC C++ compiler with CMake and Git, along with the OpenCV development files the Darknet code depends on.

```
sudo apt install -y git g++ cmake libopencv-dev
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
