**The porting to C++ try for the Darknet neural network repository**

Original repository taken from [https://github.com/AlexeyAB/darknet](https://github.com/AlexeyAB/darknet)

Some parts of the repository taken from the C++ template project located at [https://github.com/yuriysydor1991/cpp-app-template.git](https://github.com/yuriysydor1991/cpp-app-template.git)

**Work in progress. Do not expect project to work in the current state**

# Requirements

This section contains list of required packages and/or tools that must be present in the system in order to build templated project.

## Required tools for the GNU/Linux based OS

In order to build minimum template project install the GCC C++ compiler with CMake and Git.

```
sudo apt install -y git g++ cmake
```

## Required tools for the MS Windows based OS

In order to build project binaries download the install tool for the developer tools from the MSVC download page [https://visualstudio.microsoft.com/downloads/](https://visualstudio.microsoft.com/downloads/) and search and enable for the Native C++ application development section.

Separately each project may be downloaded and installed from different sources (**may require additional tools download and install**): 
- The Git versioning system at [https://git-scm.com/downloads/win](https://git-scm.com/downloads/win)
- The CMake build system at [https://cmake.org/download/](https://cmake.org/download/)
- The Windows 10 SDK at [https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/](https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/)

## Optional for the tests

If necessary to build and execute available tests install GTest or ensure available Internet access in order to project make it available by itself (GNU/Linux based):

```
sudo apt install -y googletest
```

Examine the [Project tests](#project-tests) for more info.

## Optional for the documentation

If necessary to generate the Doxygen documentation from the sources comments install doc generation system also (GNU/Linux based):

```
sudo apt install -y doxygen graphviz
```

Examine the [Documentation build](#documentation-build) section on how to enable documentation enable/build/install.

## Optional for the code formatting

To auto format code using available `clang-format` target the `clang-format` code formatter must be installed (GNU/Linux based):

```
sudo apt install -y clang-format
```

Examine configuring details in the [Enabling and performing code formatting target](#enabling-and-performing-code-formatting-target) subsection.

## Optional for the code analyzer (cppcheck)

In order to perform a static code analysis with the `cppcheck` command install it with next command (GNU/linux based):

```
sudo apt install -y cppcheck
```

## Optional for the code analyzer with clang-tidy

In order to perform a static code analysis with the `clang-tidy` command install it with next command (GNU/linux based):

```
sudo apt install -y clang-tidy
```

Examine the [Enabling the static code analyzer with clang-tidy](#enabling-the-static-code-analyzer-with-clang-tidy) section on how to enable the `cppcheck` target.

## Optional for the memory check with Valgrind

In order to enable the dynamic application check with the `valgrind` command install it with the command:

```
sudo apt install -y valgrind
```

For more details on how to enable and run the memory check target examine the [Enabling the dynamic memory check target with valgrind](#enabling-the-dynamic-memory-check-target-with-valgrind) section.

## Optional for the flatpak package

In order to generate the project flatpak package install the `flatpak-builder` command:

```
sudo apt install -y flatpak flatpak-builder
```

You'll also need the one of it's target SDK which may be installed by a command that may look like this:

```
flatpak install runtime/org.freedesktop.Sdk/x86_64/20.08
```

Replace the `runtime/org.freedesktop.Sdk/x86_64/20.08` with your preferred SDK. Consult the flatpak documentation on how to list all available options.

Look for the details at the [Enabling the flatpak package generation support](#enabling-the-flatpak-package-generation-support).

## Optional for the Docker container runs

In order to be able to start the Docker containers with the built template application install it with the next command:

```
sudo apt install -y docker.io docker-buildx
```

Next, configure the docker service to start to listen the local machine loopback TCP interface with it's configuration file edit with command:

```
sudo systemctl edit docker
```

Insert the next lines into the end of the configure file for opened docker service:

```
[Service]
ExecStart=
ExecStart=/usr/bin/dockerd -H fd:// -H tcp://127.0.0.1:2375
ExecReload=/bin/kill -s HUP $MAINPID
```

Save and close the configuration file. Now reload the systemctl and the Docker by itself:

```
sudo systemctl daemon-reload
sudo systemctl restart docker.service
```

To check if Docker works execute next command to run the simple `hello-world` container locally: 

```
DOCKER_HOST="tcp://127.0.0.1:2375" docker run hello-world
```

You may add the `export DOCKER_HOST="tcp://127.0.0.1:2375"` to your `.bashrc` file to avoid the `DOCKER_HOST` environment variable inclusion into every docker command run by executing the command:

```
echo 'export DOCKER_HOST="tcp://127.0.0.1:2375"' >> ~/.bashrc
```

Details at the section [Enabling the Docker container build and run](#enabling-the-docker-container-build-and-run)

# Project structure

## Implement code straight away!

To proceed the application implementation right away look for the `Application` class' `Application.cpp` file which is designed to accept initial code of the application. Specifically, new code may be placed into the `int Application::run(std::shared_ptr<ApplicationContext> ctx)` method.

**But do not forget about the SOLID principles and code decomposing!**

It's preferable to create other directories which would contain implemented components of the application and include them into the `Application` class implementation, rather than put all the code inside the `Application` class itself.

## Changing the project and executable name

Change the name of the project in the project's root `CMakeLists.txt` file by introducing a new value for the the `PROJECT_NAME` and/or `PROJECT_BINARY_NAME` variable which is located at `cmake/template-project-misc-variables-declare.cmake`. It is recommended to do so the executable will represent your new application name instead of templated default one - the `darknetxx`.

## Introducing custom command line parameters

In order to introduce some additional command line parameters for the binary look for the `CommandLineParser` class implementation. It contains command line parsing routines that are passed by `ApplicationFactory` class after the `main` function was called.

Add some additional custom fields into the `ApplicationContext` class in order to pass some custom command line flags and/or data to the `IApplication` interface abstract class descendants that will be created by the `ApplicationFactory` during command line arguments parse.

After registering a new flag in the `CommandLineParser`, also register it in the `ApplicationHelpPrinter` class so that the `--help`/`-h` output keeps representing all the available command line parameters. The flag string constants themselves are kept in a single place - the `CMDParamNames` class - and are referenced by both the parser and the help printer.

See the [Available command line parameters](#available-command-line-parameters) section for the full list of the parameters that the default implementation already supports.

## Implement your own IApplication descendants

You may implement another custom `IApplication` descendant classes in order to support high level variety of the application behavior to not to mess original `Application` class with irrelevant `if`-s statements and mixing up code (remember about the SOLID's single responsibility principle).

You may accomplish `IApplication` subclassing by directly creating an `IApplication` subclass in a new file or extend existing `IApplication` descendant like `Application`, `ApplicationHelpPrinter` or a `ApplicationVersionPrinter`.

Register newly created custom `IApplication` descendant in the `ApplicationFactory`'s `create_application` method which is responsible to create appropriate application instance with accordance of a provided data through the command line parameters.

That may be accomplished by implementing a custom `ApplicationFactory` descendant and overriding it's create methods like `create_application` and/or others (call appropriate static member in the `main` function of the `main.cpp` file).

## Version tracking and other project parameters

The project's `CMakeLists.txt` files are tracking of a current project git repository commit, project name, configure date and projects version that are forwarded into the projects's configure header file located at `src/app/project-global-decls.h.in`. The `project-global-decls.h.in` will be compiled into the build directory as `project-global-decls.h`. The `ApplicationVersionPrinter` class includes this project's configure file and uses the information provided to print appropriate version info. Usually, this code is executed by calling generated executable with a `-v` or `--version` command line flag. After the version is printed, the default implementation cause the application's `ApplicationVersionPrinter` instance to return zero value and cause the executable to stop it's execution.

## Project tests

Project already contains some simple test samples. Based on them you may implement full test of the introduced code. 

See the [Project build](#project-build) subsection to enable unit testing with CMake.

### Google Test

Currently test samples are based on the GTest framework. GTest framework by itself becomes available by the `FetchContent_Declare`/`FetchContent_MakeAvailable` CMake-commands if no system GTest is available. 

Of course, the project's CMakeLists.txt files are probing the GTest only if tests are enabled by `ENABLE_UNIT_TESTS` CMake variable. System GTest probe may be turned OFF by setting appropriate value to the `GTEST_TRY_SYSTEM_PROBE` CMake variable.

Look for a `cmake/template-project-GTest-enabler.cmake` to see details or change GTest version etc.

## Extensions

There will be introduced some other standard project features in the future.

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

Which effectively will create a directory named `build` (it's already added to the `.gitignore` list), configure project using the CMake available in the system (see the [Requirements](#requirements) section of this `README.md` file) and finally builds all the targets available in the project.

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

# Run the executable

## IDE run

Of course, if your IDE supports CMake build system integration you may just press the `Build` and/or `Run` button somewhere in the IDE window with opened project and it's done! No need to perform a search and execute commands.

## Command line run

### Searching for the generated executable

To find executable in the project build directory execute the `find` command:

```
find . -name 'darknetxx'
```

The `darknetxx` is a default executable name. Replace it with your custom one if you change the name of the project in the root `CMakeLists.txt` file (the `PROJECT_NAME` and/or `PROJECT_BINARY_NAME` variable).

### Starting the generated executable

If executable compiles and is present in the build directory start it in the terminal with path found from a previous subsection by a command:

```
# from the build dir
./src/darknetxx
```

Once again, the `darknetxx` is the **default** name of the project. Replace it with our own custom one if it was changed in the project's root `CMakeLists.txt` file (the `PROJECT_NAME` and/or `PROJECT_BINARY_NAME` variable).

## Available command line parameters

The generated executable accepts a set of command line parameters that are parsed by the `CommandLineParser` class and described by the `ApplicationHelpPrinter` class. The always up to date list, as supported by the current build, may be printed at any moment with the `--help` (or `-h`) flag:

```
# from the build dir
./src/darknetxx --help
```

The options are grouped below by their purpose. The options with a value placeholder (for example `<N>` or `<FILE>`) expect that value to be provided right after the flag, for example `--threads 4`. The flags without a placeholder are toggles and take no value. Passing an unknown parameter prints this help and stops the execution.

### General options

| Option | Value | Description |
| --- | --- | --- |
| `--help`, `-h` | | print the help message and exit |
| `--version`, `-v` | | print application version, build git commit and configure date, then exit |
| `--log-file`, `-l` | `<FILE>` | write the application log into the given file |

### Network and model options

| Option | Value | Description |
| --- | --- | --- |
| `--train-cfg` | `<FILE>` | training network configuration (`*.cfg`) file |
| `--detect-cfg` | `<FILE>` | detection network configuration (`*.cfg`) file |
| `--weights` | `<FILE>` | network weights file to load |
| `--image` | `<FILE>` | image file to run the detection on |
| `--gpus` | `<LIST>` | comma separated list of GPU indexes to use |
| `--threads` | `<N>` | number of worker threads to use |
| `--width` | `<N>` | override the network input width |
| `--height` | `<N>` | override the network input height |
| `--num-of-clusters` | `<N>` | number of anchor clusters (default 5) |

### Training options

| Option | Value | Description |
| --- | --- | --- |
| `--clear` | | clear (reset) the training iteration counters |
| `--map` | | calculate mAP during the training |
| `--mAP-epochs` | `<N>` | calculate mAP every given number of epochs (default 4) |
| `--dont-resize-network` | | do not resize the network during the training |
| `--net-resize-nth` | `<N>` | resize the network every Nth training iteration (default 10) |
| `--data-reload-nth` | `<N>` | reload the training data every Nth iteration |
| `--dont-reload-data-every-iteration` | | do not reload the training data on every iteration |
| `--stop-less-avg-loss` | `<VALUE>` | stop the training once the average loss reaches the given value or less |
| `--chart-path` | `<FILE>` | path to save the training chart into |
| `--dont-save-charts-every-iter` | | do not save the training chart on every iteration |
| `--draw-precision` | | draw the precision curve on the training chart |

### Detection and output options

| Option | Value | Description |
| --- | --- | --- |
| `--thresh` | `<VALUE>` | detection confidence threshold (default 0.25) |
| `--iou-thresh` | `<VALUE>` | intersection over union threshold (default 0.5) |
| `--hier` | `<VALUE>` | hierarchical detection threshold (default 0.5) |
| `--points` | `<N>` | number of points to use for the mAP calculation |
| `--letter-box` | | resize the image keeping the aspect ratio (letter box) |
| `--ext-output` | | print extended output with the bounding box coordinates |
| `--save-labels` | | save the detected labels |
| `--dontdraw-bbox` | | do not draw the detected bounding boxes |
| `--prefix` | `<PREFIX>` | prefix for the saved output file names |
| `--out` | `<FILE>` | file to write the results into |
| `--out-filename` | `<FILE>` | output video/image file name |
| `--benchmark` | | benchmark the detection performance |

### Camera, video and streaming options

| Option | Value | Description |
| --- | --- | --- |
| `--show` | | show the GUI window (enabled by default) |
| `--dont-show` | | do not show the GUI window |
| `--show-imgs` | | show the augmented training images |
| `--cam-index` | `<N>` | camera (web cam) index to capture from |
| `--frame-skip` | `<N>` | number of frames to skip between the detections |
| `--avg-frames` | `<N>` | number of frames to average the detections over (default 3) |
| `--time-limit-sec` | `<SEC>` | stop the processing after the given number of seconds |
| `--mjpeg-port` | `<PORT>` | port to stream the MJPEG result on |
| `--json-port` | `<PORT>` | port to stream the JSON result on |
| `--http-post-host` | `<HOST>` | host to send the HTTP POST results to |
| `--json-file-output` | `<FILE>` | file to write the JSON results into |

To introduce a new command line parameter of your own examine the [Introducing custom command line parameters](#introducing-custom-command-line-parameters) section.

## Tests run

### Run tests by the ctest

If enabled by the developer through the `ENABLE_UNIT_TESTS` variable and successfully build, the one may run all available test by the `ctest` command from a project build directory (GNU/Linux based):

```
# run from the project build directory

ctest
```

Alternatively, run the `ctest` command from any location by specifying the test dir (GNU/Linux based):

```
ctest --tests-dir /path/to/the/project/build/directory
```

In order to run particular test execute the `ctest` command with test's name after the `-R` command line flag. For example, for the `UTEST_ApplicationFactory` test it'll look something like this:

```
# from the project build directory 

ctest -R UTEST_ApplicationFactory
```

### Manual tests run

Currently tests are separated in different files named with `UTEST_*` pattern for the unit tests.

You may search for all compiled available tests by a next command:

```
# from the project root or a build directory

find -type f -executable -name 'UTEST_*'
```

After that, choose particular test of interest and execute it manually if needed. For example, for the `ApplicationFactory` class pass it's UT relative file system path from a project's root directory into the command line and hit enter (GNU/Linux based):

```
# from the project root

./build/src/app/tests/unit/ApplicationFactory/UTEST_ApplicationFactory
```

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

## Documentation install

If project was configured to support the documentation install by the command which looks like next:

```
# inside the project build directory

cmake ../ -DENABLE_DOC_DOXYGEN=ON -DDOXYGEN_DO_INSTALL=ON
```

The installation command (for example, described in the [Default installation](#default-installation) section) will install the generated HTML documentation files into appropriate directories.
