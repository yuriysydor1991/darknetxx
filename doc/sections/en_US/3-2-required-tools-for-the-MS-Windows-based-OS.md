## Required tools for the MS Windows based OS

In order to build project binaries download the install tool for the developer tools from the MSVC download page [https://visualstudio.microsoft.com/downloads/](https://visualstudio.microsoft.com/downloads/) and search and enable for the Native C++ application development section.

Separately each project may be downloaded and installed from different sources (**may require additional tools download and install**): 
- The Git versioning system at [https://git-scm.com/downloads/win](https://git-scm.com/downloads/win)
- The CMake build system at [https://cmake.org/download/](https://cmake.org/download/)
- The Windows 10 SDK at [https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/](https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/)

The project is built against the [darknetxx-datadrivers](https://github.com/yuriysydor1991/darknetxx-datadrivers) library on every OS: the configure of the project looks the installed library up and stops with an error when it isn't found (see the [Enabling the darknetxx-datadrivers library](/doc/sections/en_US/5-39-enabling-the-darknetxx-datadrivers-library.md) section).
