**Your neat C++ application template project**

# Goal of the template project

Project is designed to increase the speed of the application creation process at the project startup by providing templated application structure. So developer may just jump straight into implementing a particular application with no or minimum project start structure set up.

Helps you go beyond raw code — build a complete, ready-to-ship **software product** fast!

Enables rapid creation of a complete **software product** — not just raw code or a simple program.

**Just fork it and implement your application straight away!**

Examine available branches to find your most applicable variant of the template or combine multiple branches by merging them to assemble the best suited template structure for your needs:

- `main` branch at [project root page](https://github.com/yuriysydor1991/cpp-app-template) (**current**) just the clear `main` function and all available CMake integrations with no additional app infrastructure classes.
- `app` branch at [app](https://github.com/yuriysydor1991/cpp-app-template/tree/app) with just general application related classes to generate a single binary executable.
- `applib` branch at [applib](https://github.com/yuriysydor1991/cpp-app-template/tree/applib) (**current**) for the application binary with additional separate library binary and header files (available for the installation) in order to provide library's code reusability across multiple applications.
files (available for the installation) in order to provide library's code reusability across multiple applications.
- `lib` branch at [lib](https://github.com/yuriysydor1991/cpp-app-template/tree/lib) for the library with the headers include files (and documentation) without target binary.
- `appQt6` branch at [appQt6](https://github.com/yuriysydor1991/cpp-app-template/tree/appQt6) for the application general classes with additional defined structure for the Qt6 QML window application development.
- `appGtkmm3` branch at [appGtkmm3](https://github.com/yuriysydor1991/cpp-app-template/tree/appGtkmm3) for the application general classes with additional defined structure for the Gtkmm-3.0 with C++ window application development.
- `appGtkmm3Glade` branch at [appGtkmm3Glade](https://github.com/yuriysydor1991/cpp-app-template/tree/appGtkmm3Glade) for the application general classes with additional defined structure for the C++ application development with Gtkmm-3.0 and [Glade](https://en.wikipedia.org/wiki/Glade_Interface_Designer) application with XML UI creation.
- `appFreeGlut` branch at [appFreeGlut](https://github.com/yuriysydor1991/cpp-app-template/tree/appFreeGlut) with just general application related classes to generate a single binary executable with a FreeGlut library for the OpenGL 3D development.
- `appSDL2` branch at [appSDL2](https://github.com/yuriysydor1991/cpp-app-template/tree/appSDL2) with just general application related classes to generate a single binary executable with a [SDL2](https://en.wikipedia.org/wiki/Simple_DirectMedia_Layer) library for the OpenGL 3D development and more!
- `appMeson` branch at [appMeson](https://github.com/yuriysydor1991/cpp-app-template/tree/appMeson) with just general application related classes to generate a single binary executable with the [Meson](https://mesonbuild.com/) build system.
- `appWt4` branch at [appWt4](https://github.com/yuriysydor1991/cpp-app-template/tree/appWt4) the template infrastructure to fast start Web application developing based on the [Wt C++](https://www.webtoolkit.eu/wt) full stack framework.
- `appPgSQLxx` branch at [appPgSQLxx](https://github.com/yuriysydor1991/cpp-app-template/tree/appPgSQLxx) the template infrastructure to fast start application development with the [PostgreSQL DBMS](https://en.wikipedia.org/wiki/PostgreSQL) usage.
- `appBoostBeast` branch at [appBoostBeast](https://github.com/yuriysydor1991/cpp-app-template/tree/appBoostBeast) the template infrastructure for the Web application fast start development with the [Boost Beast](https://www.boost.org/libs/beast) HTTP server.
- `appMySQLCppConn` branch at [appMySQLCppConn](https://github.com/yuriysydor1991/cpp-app-template/tree/appMySQLCppConn) the template infrastructure to fast start application development with the [MySQL DBMS](https://en.wikipedia.org/wiki/MySQL) usage.
- `appSQLiteCpp3` branch at [appSQLiteCpp3](https://github.com/yuriysydor1991/cpp-app-template/tree/appSQLiteCpp3) the template infrastructure to fast start application development with the [SQLite DBMS](https://en.wikipedia.org/wiki/SQLite) and [SQLiteCpp C++ bind](https://github.com/SRombauts/SQLiteCpp) usage.
- `appMongoDBCpp4` branch at [appMongoDBCpp4](https://github.com/yuriysydor1991/cpp-app-template/tree/appMongoDBCpp4) the template infrastructure to fast start application development with the [MongoDB NoSQL DBMS](https://en.wikipedia.org/wiki/MongoDB) usage.
- `appLog4Cpp5` branch at [appLog4Cpp5](https://github.com/yuriysydor1991/cpp-app-template/tree/appLog4Cpp5) the template infrastructure to fast start the application development with the [log4cpp](https://log4cpp.sourceforge.net/) extended logging library.
- `appBoostLog` branch at [appBoostLog](https://github.com/yuriysydor1991/cpp-app-template/tree/appBoostLog) the template infrastructure to fast start the application development with the [Boost.Log](https://www.boost.org/doc/libs/latest/libs/log/doc/html/index.html) extended logging library.

Alter current `README.md` and a `CHANGELOG.md` files to match your implementation introduced into the template project. Examine the `doc` directory for possible translations of a current md document:
- `uk_UA` at [doc/README.uk_UA.md](doc/README.uk_UA.md)

See more at the [kytok.org.ua](http://www.kytok.org.ua/)

Donate at [http://kytok.org.ua/page/pozertvy](http://kytok.org.ua/page/pozertvy)

# Cloning the C++ template project

In order to fast-start implementing a new application clone this project into your local directory by executing next command in the terminal (GNU/Linux based):

```
git clone https://github.com/yuriysydor1991/cpp-app-template.git
```

or 

```
git clone https://gitlab.com/yuriysydor1991/cpp-app-template.git
```

After this command executes you should find a directory named `cpp-app-template` inside your current one. So enter it by calling `cd cpp-app-template`.

# Forking and replacing the origin

In order to copy all the template project into your new clean repository (**clean repository with no** `README.md`, `.gitignore`, `LICENCE` or any other files) clone the current cpp-app-template project into your local directory and perform origin replacement command from inside the project root directory:

```
# REPLACE next URL with yours
git remote set-url origin https://github.com/yuriysydor1991/cpp-app-template.git
```

Replace the `https://github.com/yuriysydor1991/cpp-app-template.git` URL with one of your newly created project repository URL.

To verify successful repository remote URL (where all the changes will be pushed) replacement execute next command:

```
git remote -v
```

Which should show your new project repository.

Finally push your repository to a new location by executing next command:

```
git push
```

Forking may be performed in git Web interface panel.

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

To proceed the application implementation right away look for the `LibMain` class' `LibMain.cpp` file which is designed to accept initial code of the application. Specifically, new code may be placed into the `bool LibMain::libcall(std::shared_ptr<LibraryContext> ctx)` method. 

Although `Application` class instance which calls for the `libcall` method of the `LibMain` class is fully implemented and doesn't require any changes, it may also receive some changes in order for application to work properly if necessary. See the `Application` class default implementation method `int Application::run(std::shared_ptr<ApplicationContext> ctx)` in order to review it's code and introduce some changes.

The `LibMain` class will be compiled into destination target library in order to make it's implementation available for other external applications to reuse it.

**But do not forget about the SOLID principles and code decomposing!**

It's preferable to create other directories which would contain implemented components of the application and include them into the `LibMain` class implementation, rather than put all the code inside the `LibMain` class itself (thats may be ok for a trivial application).

## Changing the project and executable name

Change the name of the project in the project's root `CMakeLists.txt` file by introducing a new value for the the `PROJECT_NAME` and/or `PROJECT_BINARY_NAME` variable which is located at `cmake/template-project-misc-variables-declare.cmake`. It is recommended to do so the executable will represent your new application name instead of templated default one - the `CppAppTemplate`.

To change the destination library name change the `PROJECT_LIBRARY_NAME` variable value. It's default value will take the `PROJECT_NAME` value and appends the `Lib` string into it. For example, if name of the project is not changed yet the library will have the `libCppAppTemplateLib` name.

## Introducing custom command line parameters

In order to introduce some additional command line parameters for the binary look for the `CommandLineParser` class implementation. It contains command line parsing routines that are passed by `ApplicationFactory` class after the `main` function was called.

Add some additional custom fields into the `ApplicationContext` class in order to pass some custom command line flags and/or data to the `IApplication` interface abstract class descendants that will be created by the `ApplicationFactory` during command line arguments parse.

Some or all `ApplicationContext` fields may be transferred into the `LibraryContext` instance by the `ApplicationContext2LibraryContext` converter class instance during the `LibMain` default implementation call. Examine the `Application` default application implementation.

## Implement your own IApplication descendants

You may implement another custom `IApplication` descendant classes in order to support high level variety of the application behavior to not to mess original `Application` class with irrelevant `if`-s statements and mixing up code (remember about the SOLID's single responsibility principle).

You may accomplish `IApplication` subclassing by directly creating an `IApplication` subclass in a new file or extend existing `IApplication` descendant like `Application`, `ApplicationHelpPrinter` or a `ApplicationVersionPrinter`.

Register newly created custom `IApplication` descendant in the `ApplicationFactory`'s `create_application` method which is responsible to create appropriate application instance with accordance of a provided data through the command line parameters.

That may be accomplished by implementing a custom `ApplicationFactory` descendant and overriding it's create methods like `create_application` and/or others (call appropriate static member in the `main` function of the `main.cpp` file).

## Implement your own ILib descendants

All above recommendations for the `IApplication` subclassing are applying to the `ILib` class which must contain the application main functionality and which will be shared as library amongst all parties of interest.

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

Which in turn will generate the `doc/CppAppTemplate-html` directory (already added to the `.gitignore` file) which will contain the HTML-type documentation. In order to open and examine generated documentation open the `doc/CppAppTemplate-html/index.html` file. The `CppAppTemplate-html` directory name will change if changed default executable name for the project by setting a new value for the `PROJECT_BINARY_NAME` variable in the root `CMakeLists.txt` or the `DOXYGEN_OUT_HTML_NAME` which in turn set the whole name for the directory.

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

The package file should be generated inside the project build root directory. For example, if project name was't change and it's version is 0.8.0 so the package name may look like `CppAppTemplate-0.8.0-Linux.deb`.

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

In case of the success target build, there will be created the flatpak file under the root build directory named, for example, the `CppAppTemplate-0.9.0.flatpak`. In order to examine and/or change the flatpak generation target parameters visit the `cmake/template-project-flatpak-target.cmake` or `misc/flatpak.conf.json.in` files.

Refer to the flatpak documentation on how to install and run applications distributed from a flatpaks files. For the current template project's defaults the install and run command may look like this:

```
# to install (version may change)
# inside the project build directory
flatpak install --user CppAppTemplate-0.9.0.flatpak

# to run the application
flatpak run ua.org.kytok.template.CppAppTemplate
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
find . -name 'CppAppTemplate'
```

The `CppAppTemplate` is a default executable name. Replace it with your custom one if you change the name of the project in the root `CMakeLists.txt` file (the `PROJECT_NAME` and/or `PROJECT_BINARY_NAME` variable).

### Starting the generated executable

If executable compiles and is present in the build directory start it in the terminal with path found from a previous subsection by a command:

```
# from the build dir
./src/CppAppTemplate
```

Once again, the `CppAppTemplate` is the **default** name of the project. Replace it with our own custom one if it was changed in the project's root `CMakeLists.txt` file (the `PROJECT_NAME` and/or `PROJECT_BINARY_NAME` variable).

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
# installs generated library under the /usr/local/lib/
# and installs header include files under the /usr/local/include/CppAppTemplate
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
