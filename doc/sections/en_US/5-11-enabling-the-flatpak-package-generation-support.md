## Enabling the flatpak package generation support

In order to enable the flatpak generation of the redistribution package on the template project configure it with enabled option `ENABLE_FLATPAK` (GNU/Linux and alike):

```
# inside the project root directory 

cmake -B build -S . -DENABLE_FLATPAK=ON
```

Which in order will enable the `flatpak` target.

And finally to generate the flatpak package run the `flatpak` target for the build:

```
# inside the project root directory 

cmake --build build --target flatpak
```

In case of the success target build, there will be created the flatpak file under the root build directory named, for example, the `darknetxx-0.9.0.flatpak`. In order to examine and/or change the flatpak generation target parameters visit the `cmake/darknetxx-flatpak-target.cmake` or `misc/packagers/flatpak.conf.json.in ` files.

The project inside the sandbox of the `flatpak-builder` is built against the [darknetxx-datadrivers](https://github.com/yuriysydor1991/darknetxx-datadrivers) library of the network weights files formats (see the [Enabling the darknetxx-datadrivers library](/doc/sections/en_US/5-39-enabling-the-darknetxx-datadrivers-library.md) section), which the GNOME runtime has none of, so the manifest builds and installs the library first: the `darknetxx-datadrivers` module of the [misc/packagers/flatpak.conf.json.in](/misc/packagers/flatpak.conf.json.in) clones the repository of the `DARKNETXX_DXXDD_GIT` cache variable at the branch of the `DARKNETXX_DXXDD_GIT_TAG` one (the `main` branch of the library repository by default) and builds it's `Release` build after the `nlohmann-json` module, the one the library is built with, and before the module of the project, which finds it in the `/app` prefix of the sandbox. The package carries the library and it's `darknetxxConverter` binary along with the binaries of the project.

```
# inside the project root directory

# the flatpak of another revision of the library
cmake -B build -S . -DENABLE_FLATPAK=ON -DDARKNETXX_DXXDD_GIT_TAG=my-branch
```

Refer to the flatpak documentation on how to install and run applications distributed from a flatpaks files. For the current template project's defaults the install and run command may look like this:

```
# to install (version may change)
# inside the project build directory
flatpak install --user darknetxx-0.9.0.flatpak

# to run the application
flatpak run ua.org.kytok.template.darknetxx
```
