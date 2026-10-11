## Using the darknetxx library

The project builds the shared `darknetxx-0.11` library of all it's
functionality (see the [The darknetxx library](/doc/sections/en_US/4-7-5-the-darknetxx-library.md)
section) and the executables over it. Another CMake project uses the library
made available by the FetchContent or installed and found by the
`find_package`.

### The options

| CMake option | Default | Effect |
| --- | --- | --- |
| `ENABLE_DARKNETXX_EXECUTABLES` | `ON` for the top level project, `OFF` for the project of another one | Builds and installs the `darknetxx`, the `darknetxxConverter`, the `darknetxx-train-gui` and the `darknetxx-recognition-gui` executables. `OFF` builds and installs the library alone. |
| `DARKNETXX_LIB_INSTALL_DEV_FILES` | `ON` | Installs the public headers, the CMake package files and the `libdarknetxx-0.11.so` namelink beside the library. `OFF` installs the library runtime alone. |
| `DARKNETXX_LIBRARY_NAME` | `darknetxx-<major>.<minor>` | The name of the library, of it's target, of the headers directory and of the CMake package. |

The library alone, with the development files:

```
# inside the project root directory

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DENABLE_DARKNETXX_EXECUTABLES=OFF
cmake --build build
cmake --install build --prefix /usr/local
```

installs

| Installed file | `DARKNETXX_LIB_INSTALL_DEV_FILES=ON` | `OFF` |
| --- | --- | --- |
| `<prefix>/lib/libdarknetxx-0.11.so.0.11.0` and it's `.so.0.11` soname link | yes | yes |
| `<prefix>/lib/libdarknetxx-0.11.so` namelink | yes | no |
| `<prefix>/include/darknetxx-0.11/*.h` public headers | yes | no |
| `<prefix>/lib/cmake/darknetxx-0.11/*.cmake` package files | yes | no |
| `<prefix>/share/darknetxx/<version>` darknet data (the labels font, the names lists) | yes | yes |

The `lib` directory is the `CMAKE_INSTALL_LIBDIR` one, `lib/x86_64-linux-gnu`
for the `/usr` prefix of the Debian and the Ubuntu, for example.

### FetchContent

The project of another one builds the library alone and none of the tests,
the tools and the packages of this one, whatever it's same named options say
(see the root [CMakeLists.txt](/CMakeLists.txt)). The CMake modules, the cache
variables, the functions and the targets of the project are named after it,
so they do not mix with the ones of another cpp-app-template derived project.

```cmake
include(FetchContent)

FetchContent_Declare(
  darknetxx
  GIT_REPOSITORY https://github.com/yuriysydor1991/darknetxx.git
  GIT_TAG master
)
FetchContent_MakeAvailable(darknetxx)

add_executable(my-trainer main.cpp)
target_link_libraries(my-trainer darknetxx-0.11::darknetxx-0.11)
```

### find_package

```cmake
find_package(darknetxx-0.11 REQUIRED CONFIG)

add_executable(my-trainer main.cpp)
target_link_libraries(my-trainer darknetxx-0.11::darknetxx-0.11)
```

### The code

The program reaches the library through the
[`LibraryFacade`](/src/lib/facade/public/LibraryFacade.h) and includes the
headers by the library prefixed name, which another library of the same names
does not shadow:

```cpp
#include <darknetxx-0.11/LibraryFacade.h>

int main()
{
  auto ctx = darknetxx011::LibraryFacade::create_library_context();

  ctx->convert_from = "weights";
  ctx->convert_src = "yolov4.weights";
  ctx->convert_cfg = "yolov4.cfg";
  ctx->convert_to = "dxxwjz1";
  ctx->convert_dst = "yolov4.dxxwjz1";

  auto lib = darknetxx011::LibraryFacade::create_library(ctx);

  return lib != nullptr && lib->libcall(ctx) ? 0 : 1;
}
```

The `train_cfg` context of the very same `create_library` call trains the
network and the `detect_cfg` one detects, while the
`LibraryFacade::create_trainer` trains the training project the
`LibraryFacade::open_training_project` opens of a path (a YOLO4 folder, a
dxxwjz2p or a dxxwjz3p project file) in the background and reports the iterations to the
`training_listener` of the context, and the
`LibraryFacade::training_chart` gives the PNG image of the training chart of a
dxxwjz2 or dxxwjz3 weights file (the empty one for a file of no training record), the
`LibraryFacade::network_cfg` the darknet cfg text of the network a dxxwjz2,
dxxwjz2p, dxxwjz3 or dxxwjz3p file keeps (the empty one for the rest). A program
of an own logger hands it over by the `LibraryFacade::init_logger` before any
other call, the library logs into the `darknetxx-0.11.0.log` file of the
working directory and the console otherwise.
