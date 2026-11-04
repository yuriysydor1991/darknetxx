## Enabling the nlohmann JSON library

The [nlohmann](https://github.com/nlohmann/json.git) JSON library serves the tests of this project alone. The dxxwjz network weights files (the gzip compressed JSON documents, see the [Network weights files](/doc/sections/en_US/4-7-4-network-weights-files.md) section) are read and written by the [darknetxx-datadrivers](https://github.com/yuriysydor1991/darknetxx-datadrivers) library, which is built with a nlohmann JSON library of it's own and installs no header of it (see the [Enabling the darknetxx-datadrivers library](/doc/sections/en_US/5-39-enabling-the-darknetxx-datadrivers-library.md) section), while the component and the integration tests of this project read the JSON documents the library writes on their own, by the `nlohmann_json::nlohmann_json` target of this enabler.

The `ENABLE_NLOHMANN_JSON` CMake variable is `ON` by default for the top level project and `OFF` for the project of another one, which builds none of the tests (see the root [CMakeLists.txt](/CMakeLists.txt)). The installed system library is used when found (the `nlohmann-json3-dev` package of Debian and Ubuntu), otherwise the configure fetches the `v3.11.3` release of it through the available connection (the `DARKNETXX_NLOHMANN_GIT` and the `DARKNETXX_NLOHMANN_GIT_TAG` cache variables name the repository and the tag, the ones of the `nlohmann-json` module of the flatpak manifest as well, which the `darknetxx-datadrivers` module of the manifest is built with). The variable turned `OFF` makes nothing available, so it goes with the tests turned off.

```
# Inside the source root directory

cmake -S . -B build -DENABLE_NLOHMANN_JSON=ON
```
