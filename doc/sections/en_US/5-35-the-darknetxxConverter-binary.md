## The darknetxxConverter binary

Beside the `darknetxx` application the project builds a second binary,
`darknetxxConverter`, which converts the network weights files between the
original darknet weights format and the dxxwjz1 one and does nothing else. It
is the very same conversion the main binary offers through its own [command
line](/doc/sections/en_US/6-2-4-converting-the-network-weights-files.md) -
the flags, the formats and the behaviour are one and the same, because the
code behind them is one and the same - and what the second binary adds is a
deployment option rather than a feature: **no training nor detection is
compiled into it**, so a script or a data preparation pipeline which only has
to turn the weights files of one format into the other one runs the binary
with nothing else in it.

### Usage

The binary accepts the very same conversion parameters the main one does, so
the [Converting the network weights files](/doc/sections/en_US/6-2-4-converting-the-network-weights-files.md)
section describes it in full:

```
# from the build dir

./src/darknetxxConverter \
  --convert-from weights \
  --convert-src ./yolov3.weights \
  --convert-to dxxwjz1 \
  --convert-dst ./yolov3.dxxwjz1 \
  --convert-cfg ./yolov3.cfg
```

`--help` and `--version` work as they do for the main binary and name this
one, while the help lists the conversion options alone. A command line which
asks for no conversion has nothing to start - there is no training nor
detection behind this binary - so it is answered with the help message and a
non zero exit status.

### Building it

Both binaries are built by default, so a plain build of this project produces
`darknetxx` and `darknetxxConverter` side by side, and both are installed by
the install step.

| CMake option | Default | Effect |
| --- | --- | --- |
| `ENABLE_DARKNETXX_CONVERTER` | `ON` | Builds the `darknetxxConverter` binary. Set it to `OFF` to build the main application alone. |
| `PROJECT_CONVERTER_BINARY_NAME` | `<project binary name>Converter` | The name of the second binary and of its CMake target. |

```
# inside the project root directory

cmake -B build -S . -DENABLE_DARKNETXX_CONVERTER=OFF
```

The dxxwjz1 files are the gzip compressed JSON documents, so both binaries
require the [zlib](/doc/sections/en_US/5-23-enabling-the-zlib-library.md) and
the [nlohmann JSON](/doc/sections/en_US/5-18-enabling-the-nlohmann-json-library.md)
libraries, the system ones or the ones the configure fetches.

### How the two binaries share their code

The second binary is not a copy of anything. Both binaries are entered through
one and the same `src/main.cpp`, the `src/app` component hands its sources to
both of them (see [src/app/CMakeLists.txt](/src/app/CMakeLists.txt)), and the
network infrastructure the conversion reads and writes the files with - the
networks of the cfg files, the original weights files and the dxxwjz1 ones -
is the `DarknetXXNetworksCore` library, the one the component tests link.

Exactly one translation unit differs: `src/app/Application.cpp`, the class
which starts the training and the detection (and with it the
`DarknetController` and the OpenMP controller only that class needs). It is
handed to the main target alone, while
[src/darknetxxConverter-target.cmake](/src/darknetxxConverter-target.cmake)
compiles its target with the `DARKNETXX_CONVERTER_ONLY` preprocessor definition
instead. That definition is what tells the sources both binaries share about
the missing class: the `ApplicationFactory` neither includes nor creates it,
and the help and the version messages name the binary being run (see
[src/app/ApplicationBinaryName.h](/src/app/ApplicationBinaryName.h)). Nothing
else in the project branches on it.

The conversion itself is the one of the `WeightsConverter` of the network
infrastructure (see the [Network weights files](/doc/sections/en_US/4-7-4-network-weights-files.md)
section): the `app::converter::ApplicationConverterComponent` runs the
`app::converter::ApplicationFormatsConverter`, which hands the command line of
the context to it. The binary is covered by the `ITEST_converter` and the
`ITEST_converter_bash` integration tests (see the
[Enabling integration testing](/doc/sections/en_US/5-3-3-enabling-integration-testing.md)
section).
