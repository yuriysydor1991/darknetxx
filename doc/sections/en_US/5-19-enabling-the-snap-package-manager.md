## Enabling the snap packager

In order to enable the project's `snap` package target designed to generate the project's [SNAP](https://snapcraft.io/docs/) binary package distributive, configure project with enabled `ENABLE_SNAP` CMake variable:

```
cmake -S . -B build -DENABLE_SNAP=ON
```

Visit the [cmake/enablers/packagers/darknetxx-snap-enabler.cmake](/cmake/enablers/packagers/darknetxx-snap-enabler.cmake) file to examine the details.

The [misc/packagers/snapcraft.yaml.in](/misc/packagers/snapcraft.yaml.in) recipe builds the [darknetxx-datadrivers](https://github.com/yuriysydor1991/darknetxx-datadrivers) library of the network weights files formats first (see the [Enabling the darknetxx-datadrivers library](/doc/sections/en_US/5-39-enabling-the-darknetxx-datadrivers-library.md) section): the `darknetxx-datadrivers` part clones the repository of the `DARKNETXX_DXXDD_GIT` cache variable at the branch of the `DARKNETXX_DXXDD_GIT_TAG` one and builds it's `Release` build into the `/usr` prefix of the snap with the OpenCV, the zlib and the nlohmann JSON development packages, staging the zlib and the OpenCV runtime libraries it links with, while the part of the project is built `after` it. The snap carries the `darknetxxConverter` binary the library installs as the `darknetxxConverter` app beside the `darknetxx` one (see [The darknetxxConverter binary](/doc/sections/en_US/5-35-the-darknetxxConverter-binary.md) section).
