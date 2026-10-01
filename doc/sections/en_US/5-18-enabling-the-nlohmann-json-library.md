## Enabling the nlohmann JSON library

The [nlohmann](https://github.com/nlohmann/json.git) JSON library reads the dxxwjz1 network weights files (the gzip compressed JSON documents, see the [Network weights files](/doc/sections/en_US/4-7-4-network-weights-files.md) section), so the project requires it and the `ENABLE_NLOHMANN_JSON` CMake variable is `ON` by default. The installed system library is used when found (the `nlohmann-json3-dev` package of Debian and Ubuntu), otherwise the configure fetches the `v3.11.3` release of it through the available connection (the `TEMPLATE_APP_NLOHMANN_GIT` and the `TEMPLATE_APP_NLOHMANN_GIT_TAG` cache variables name the repository and the tag). The variable can't be turned `OFF`: the configure turns it back `ON` with a warning, the cached `OFF` default of the build directories configured by the earlier versions of the project included.

```
# Inside the source root directory

cmake -S . -B build -DENABLE_NLOHMANN_JSON=ON
```
