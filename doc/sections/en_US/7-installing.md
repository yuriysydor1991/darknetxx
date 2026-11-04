
# Installing

Execute available install commands from the project's build directory.

## Default installation

In order to install generated executable (as shown previous) file into your's system binary default folder execute next command in the command line (GNU/Linux based):

```
# installs generated binary under the /usr/local/bin/ for example
# run from the project's build directory

sudo cmake --install .
```

Usually it's the `/usr/local/bin/` directory (on the Unix-like OS) which may be inaccessible from the `PATH` environment variable (e.g. can not be started as a regular command).

## Custom installation path

To install binary into the system globally available directory add the `--prefix` parameter to the command above as next:

```
# replace the /usr path with our own if needed
# run from the project's build directory

sudo cmake --install . --prefix "/usr"
```

Examine the `PATH` environment variable to chose directory best suited for your current OS (execute `echo $PATH` in the terminal). Alternatively, any path may be specified.

## The darknetxx-datadrivers library

The installed binaries and the installed library of the project load the shared library of the [darknetxx-datadrivers](https://github.com/yuriysydor1991/darknetxx-datadrivers) project at the run time (the `libDarknetxxDataDrivers-0.12.so.0`), which the install step of the project installs for the build of the `deb` package alone (the `ENABLE_DEB` and the `ENABLE_DEB_DARKNETXX_DATA_DRIVERS` options, see the [Enabling DEB package generation with cpack](/doc/sections/en_US/5-10-enabling-DEB-package-generation-with-cpack.md) section). Keep the library installed where the loader finds it: the `/usr` or the `/usr/local` prefix of the [misc/ci/install-darknetxx-datadrivers.sh](/misc/ci/install-darknetxx-datadrivers.sh) script, or the `LD_LIBRARY_PATH` of another one (see the [Enabling the darknetxx-datadrivers library](/doc/sections/en_US/5-39-enabling-the-darknetxx-datadrivers-library.md) section).

## Documentation install

If project was configured to support the documentation install by the command which looks like next:

```
# inside the project build directory

cmake ../ -DENABLE_DOC_DOXYGEN=ON -DDOXYGEN_DO_INSTALL=ON
```

The installation command (for example, described in the [Default installation](#default-installation) section) will install the generated HTML documentation files into appropriate directories.
