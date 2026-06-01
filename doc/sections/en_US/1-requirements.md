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

Examine the [Project tests](/doc/sections/en_US/2-project-structure.md#project-tests) for more info.

## Optional for the documentation

If necessary to generate the Doxygen documentation from the sources comments install doc generation system also (GNU/Linux based):

```
sudo apt install -y doxygen graphviz
```

Examine the [Documentation build](/doc/sections/en_US/4-project-build.md#documentation-build) section on how to enable documentation enable/build/install.

## Optional for the code formatting

To auto format code using available `clang-format` target the `clang-format` code formatter must be installed (GNU/Linux based):

```
sudo apt install -y clang-format
```

Examine configuring details in the [Enabling and performing code formatting target](/doc/sections/en_US/4-project-build.md#enabling-and-performing-code-formatting-target) subsection.

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

Examine the [Enabling the static code analyzer with clang-tidy](/doc/sections/en_US/4-project-build.md#enabling-the-static-code-analyzer-with-clang-tidy) section on how to enable the `cppcheck` target.

## Optional for the memory check with Valgrind

In order to enable the dynamic application check with the `valgrind` command install it with the command:

```
sudo apt install -y valgrind
```

For more details on how to enable and run the memory check target examine the [Enabling the dynamic memory check target with valgrind](/doc/sections/en_US/4-project-build.md#enabling-the-dynamic-memory-check-target-with-valgrind) section.

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

Look for the details at the [Enabling the flatpak package generation support](/doc/sections/en_US/4-project-build.md#enabling-the-flatpak-package-generation-support).

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

Details at the section [Enabling the Docker container build and run](/doc/sections/en_US/4-project-build.md#enabling-the-docker-container-build-and-run)
