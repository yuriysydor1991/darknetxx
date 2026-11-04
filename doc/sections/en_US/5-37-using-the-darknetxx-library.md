## Using the darknetxx library

The project builds the shared `darknetxx-0.12` library of all it's
functionality (see the [The darknetxx library](/doc/sections/en_US/4-7-5-the-darknetxx-library.md)
section) and the executables over it. Another CMake project uses the library
made available by the FetchContent or installed and found by the
`find_package`.

Either way the library is built against the installed
[darknetxx-datadrivers](https://github.com/yuriysydor1991/darknetxx-datadrivers)
library of the network weights files formats, which nothing fetches: install
it first and point the configure at it's prefix by the `CMAKE_PREFIX_PATH`
(see the
[Enabling the darknetxx-datadrivers library](/doc/sections/en_US/5-39-enabling-the-darknetxx-datadrivers-library.md)
section). The `libdarknetxx-0.12.so` library loads the
`libDarknetxxDataDrivers-0.12.so.0` one at the run time, so the loader has to
find both of them, while no public header of the darknetxx library names a
type of the darknetxx-datadrivers one, so the program of the darknetxx
library includes no header of it.

### The options

| CMake option | Default | Effect |
| --- | --- | --- |
| `ENABLE_DARKNETXX_EXECUTABLES` | `ON` for the top level project, `OFF` for the project of another one | Builds and installs the `darknetxx` executable and the GUI ones (the `darknetxx-train-gui`, the `darknetxx-recognition-gui` and the network editor ones). `OFF` builds and installs the library alone. |
| `DARKNETXX_LIB_INSTALL_DEV_FILES` | `ON` | Installs the public headers, the CMake package files and the `libdarknetxx-0.12.so` namelink beside the library. `OFF` installs the library runtime alone. |
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
| `<prefix>/lib/libdarknetxx-0.12.so.0.12.0` and it's `.so.0.12` soname link | yes | yes |
| `<prefix>/lib/libdarknetxx-0.12.so` namelink | yes | no |
| `<prefix>/include/darknetxx-0.12/*.h` public headers | yes | no |
| `<prefix>/lib/cmake/darknetxx-0.12/*.cmake` package files | yes | no |
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
target_link_libraries(my-trainer darknetxx-0.12::darknetxx-0.12)
```

```
# the configure of the project of the FetchContent above

cmake -S . -B build -DCMAKE_PREFIX_PATH=/opt/darknetxx
```

### find_package

```cmake
find_package(darknetxx-0.12 REQUIRED CONFIG)

add_executable(my-trainer main.cpp)
target_link_libraries(my-trainer darknetxx-0.12::darknetxx-0.12)
```

### The code

The program reaches the library through the
[`LibraryFacade`](/src/lib/facade/public/LibraryFacade.h) and includes the
headers by the library prefixed name, which another library of the same names
does not shadow:

```cpp
#include <darknetxx-0.12/LibraryFacade.h>

int main()
{
  auto ctx = darknetxx012::LibraryFacade::create_library_context();

  ctx->convert_from = "weights";
  ctx->convert_src = "yolov4.weights";
  ctx->convert_cfg = "yolov4.cfg";
  ctx->convert_to = "dxxwjz1";
  ctx->convert_dst = "yolov4.dxxwjz1";

  auto lib = darknetxx012::LibraryFacade::create_library(ctx);

  return lib != nullptr && lib->libcall(ctx) ? 0 : 1;
}
```

The `train_cfg` context of the very same `create_library` call trains the
network and the `detect_cfg` one detects, while the
`LibraryFacade::create_trainer` trains the training project the
`LibraryFacade::open_training_project` opens of a path (a YOLO4 folder, a
dxxwjz2p, a dxxwjz3p or a dxxwjz4p project file) in the background and reports the iterations to the
`training_listener` of the context, and the
`LibraryFacade::training_chart` gives the PNG image of the training chart of a
dxxwjz2, dxxwjz3 or dxxwjz4 weights file (the empty one for a file of no training record), the
`LibraryFacade::network_cfg` the darknet cfg text of the network a dxxwjz2,
dxxwjz2p, dxxwjz3, dxxwjz3p, dxxwjz4 or dxxwjz4p file keeps (the empty one for the rest), the
`LibraryFacade::network_description` the cfg text with the class names and the
`LibraryFacade::save_network` saves the edited cfg text and class names into
such a file (the network editor binaries save by it). A program
of an own logger hands it over by the `LibraryFacade::init_logger` before any
other call, the library logs into the `darknetxx-0.12.0.log` file of the
working directory and the console otherwise.
