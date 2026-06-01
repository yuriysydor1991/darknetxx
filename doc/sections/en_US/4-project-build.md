# Project build

## IDE build

Of course if your IDE supports CMake build system integration you may just press the `Build` and/or `Run` button somewhere in the IDE window with opened project and it's done! No need to perform a search and execute commands.

## Command line build

The project uses the CMake build system so a build command should be familiar to the most of the C++ developers around the world!

Of course, project needs to be `git clone`-ed and it's root directory must be opened in the terminal or in your IDE first. After that you may execute next commands:

```
# from the project root

mkdir -vp build && cd build && cmake ../ && cmake --build . --target all
```

Which effectively will create a directory named `build` (it's already added to the `.gitignore` list), configure project using the CMake available in the system (see the [Requirements](/doc/sections/en_US/1-requirements.md) section of this `README.md` file) and finally builds all the targets available in the project.

## Enabling testing

### Enabling unit testing

To enable project unit test availability (for building and running) reconfigure it with enabled `ENABLE_UNIT_TESTS` variable as follows (GNU/Linux based):

```
# from the project root

mkdir -vp build && cd build && cmake ../ -DENABLE_UNIT_TESTS=ON && cmake --build . --target all
```

### Disabling system GTest probe 

To disable the system available GTest framework assets usage set appropriate value to the `GTEST_TRY_SYSTEM_PROBE` CMake variable by executing command like (GNU/Linux based):

```
mkdir -vp build && cd build && cmake ../ -DENABLE_UNIT_TESTS=ON -DGTEST_TRY_SYSTEM_PROBE=OFF && cmake --build . --target all
```

During command execution project build system will try to make GTest available through the Internet only for current project with specified version in the `cmake/template-project-GTest-enabler.cmake` file.

## Documentation build

Currently it's possible to auto-generate the project documentation by the Doxygen tool from the available sources comments.

To enable Doxygen documentation CMake-target during the project configure process call a command that sets the `ENABLE_DOC_DOXYGEN` CMake variable to the `ON` value (GNU/Linux based):

```
# inside the project root directory 

mkdir -vp build && cd build && cmake ../ -DENABLE_DOC_DOXYGEN=ON
```

Which effectively will create a directory named `build` inside the project root directory, enters it by a `cd` command and configures project to enable Doxygen documentation build.

Finally build the documentation by executing the command:

```
# inside the project build directory

cmake --build . --target Doxygen-doc
```

Which in turn will generate the `doc/darknetxx-html` directory (already added to the `.gitignore` file) which will contain the HTML-type documentation. In order to open and examine generated documentation open the `doc/darknetxx-html/index.html` file. The `darknetxx-html` directory name will change if changed default executable name for the project by setting a new value for the `PROJECT_BINARY_NAME` variable in the root `CMakeLists.txt` or the `DOXYGEN_OUT_HTML_NAME` which in turn set the whole name for the directory.

The `doc/Doxyfile.in` file contains all available Doxygen configuration parameters which may be changed in order to change the documentation output.

## Configuring the documentation install support

It's possible to enable support for the document installation by setting up the `ENABLE_DOC_DOXYGEN` and `DOXYGEN_DO_INSTALL` variables to `ON` value during the project configure stage.

```
# inside the project build directory

cmake ../ -DENABLE_DOC_DOXYGEN=ON -DDOXYGEN_DO_INSTALL=ON
```

The `DOXYGEN_OUT_HTML_NAME` CMake variable will configure the documentation html directory name (passed into the `Doxyfile`).

## Enabling and performing code formatting target

In order to make `clang-format` target available set the `ENABLE_CLANGFORMAT` variable value to `ON` during the project configuration stage (GNU/Linux based):

```
# inside the project root directory 

mkdir -vp build && cd build && cmake ../ -DENABLE_CLANGFORMAT=ON
```

To perform the whole project code format in accordance with available `misc/.clang-format` code formatter configuration execute next building command:

```
# inside the project build directory

cmake --build . --target clang-format
```

The `clang-format` target details may be examined and/or altered in the `cmake/template-project-clang-format-target.cmake` CMake submodule file.

## Enabling the static code analyzer target with cppcheck

In order to make the cppcheck target available for the execution, configure project with enabled `ENABLE_CPPCHECK` variable:

```
# inside the project root directory 

mkdir -vp build && cd build && cmake ../ -DENABLE_CPPCHECK=ON
```

And to perform the actual static code analysis by itself run the `cppcheck` target for the build:

```
# inside the project build directory

cmake --build . --target cppcheck
```

The `cppcheck` target details may be examined and/or altered in the `cmake/template-project-cppcheck-target.cmake` CMake submodule file.

## Enabling the static code analyzer with clang-tidy

In order to enable the static code check for the each translation unit or each `*.cpp` file in the project with a `clang-tidy` application enable it during the project configuration stage by setting the `ENABLE_CLANG_TIDY` variable value to `ON` in the configuration command:

```
# inside the project root directory

mkdir -vp build && cd build && cmake ../ -DENABLE_CLANG_TIDY=ON
```

And to perform the actual static code analysis by itself build any target of interest and the `clang-tidy` command will perform check on each of the compilation target:

```
# inside the project build directory

cmake --build . --target all
```

For more information about the `clang-tidy` examine the `cmake/template-project-clang-tidy-target.cmake`, `misc/.clang-tidy` files
and the `clang-tidy` application documentation of a used version.

**Warning!** The `clang-tidy` static code analysis may dramatically increase the code compilation time.

## Enabling the dynamic memory check target with valgrind

In order to make the `valgrind` target available for the execution, configure project with the enabled `ENABLE_VALGRIND` variable like:

```
# inside the project root directory 

mkdir -vp build && cd build && cmake ../ -DENABLE_VALGRIND=ON
```

To perform the dynamic memory check on the application build run next command:

```
# inside the project build directory

cmake --build . --target valgrind
```

The `valgrind` target details may be examined and/or altered in the `cmake/template-project-valgrind-target.cmake` CMake submodule file.

**Warning! It will start application.** The dynamic memory check requires application to be started and go through the full cycle. Ensure that application execution will have a finite time.

## Enabling DEB package generation with cpack

In order to be able to generate the `deb` package file configure project to support the `cpack` command feature with a `ENABLE_DEB` variable enabled:

```
# inside the project root directory 

mkdir -vp build && cd build && cmake ../ -DENABLE_DEB=ON
```

Next, build all available targets required for the `deb` package

```
# inside the project build directory

cmake --build . --target all
```

Finally, execute the `cpack` command inside the template project build directory:

```
# inside the project build directory

cpack
```

The package file should be generated inside the project build root directory. For example, if project name was't change and it's version is 0.8.0 so the package name may look like `darknetxx-0.8.0-Linux.deb`.

In order to examine details of the `deb` package configuration visit the `cmake/template-project-deb-enabler.cmake` file.

## Enabling the flatpak package generation support

In order to enable the flatpak generation of the redistribution package on the template project configure it with enabled option `ENABLE_FLATPAK` (GNU/Linux and alike):

```
# inside the project root directory 

mkdir -vp build && cd build && cmake ../ -DENABLE_FLATPAK=ON
```

Which in order will enable the `flatpak` target.

And finally to generate the flatpak package run the `flatpak` target for the build:

```
# inside the project build directory

cmake --build . --target flatpak
```

In case of the success target build, there will be created the flatpak file under the root build directory named, for example, the `darknetxx-0.9.0.flatpak`. In order to examine and/or change the flatpak generation target parameters visit the `cmake/template-project-flatpak-target.cmake` or `misc/flatpak.conf.json.in` files.

Refer to the flatpak documentation on how to install and run applications distributed from a flatpaks files. For the current template project's defaults the install and run command may look like this:

```
# to install (version may change)
# inside the project build directory
flatpak install --user darknetxx-0.9.0.flatpak

# to run the application
flatpak run ua.org.kytok.template.darknetxx
```

## Enabling the Docker container build and run

To be able to build and run the Docker container with current template project' target binary file enable it's CMake target with setting the `ENABLE_DOCKER` variable into value `ON` by using the next command:

```
# inside the project root directory

cmake -S . -B build -DENABLE_DOCKER=ON
```

After that there should be available the Docker build and run CMake target `docker-single-run` which may be triggered with the command:

```
# inside the project root directory

cmake --build build --target docker-single-run 
```

Examine the `cmake/dockerers/template-project-docker-single-runner-target.cmake` file for the `docker-single-run` target details.

## Enabling sanitizers

In order to enable the sanitizers to check application at the runtime, configure project with the command:

```
cmake -S . -B build -DENABLE_SANITIZERS=ON
```

For a general purpose sanitizers. See the `cmake/template-project-sanitizers.cmake` to examine available list of the sanitizers.

Alternatively, enable the threads sanitizers with the command:


```
cmake -S . -B build -DENABLE_SANITIZERS_THREADS=ON
```

to check the possible data rises of the newly introduced multithreaded application code.

Do not combine the `ENABLE_SANITIZERS` and `ENABLE_SANITIZERS_THREADS`.

Any findings will be printed into the application' stdout/stderr after the build and run.
