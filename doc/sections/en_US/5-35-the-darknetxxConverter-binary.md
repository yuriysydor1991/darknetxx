## The darknetxxConverter binary

Beside the `darknetxx` application the project builds a second binary,
`darknetxxConverter`, which converts the network weights files between the
original darknet weights format, the dxxwjz1, the dxxwjz2, the dxxwjz2p, the
dxxwjz3 and the dxxwjz3p ones, the YOLO4 training folders into the dxxwjz2p and
the dxxwjz3p projects and back, and does
nothing else. It
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
  --convert-to dxxwjz2 \
  --convert-dst ./yolov3.dxxwjz2 \
  --convert-cfg ./yolov3.cfg \
  --convert-names ./coco.names

# the network to train of the cfg file: the dxxwjz2 file of no weights
./src/darknetxxConverter \
  --convert-to dxxwjz2 \
  --convert-dst ./to_train.dxxwjz2 \
  --convert-cfg ./yolov3.cfg \
  --convert-names ./coco.names

# the YOLO4 folder to the dxxwjz2p project of it's images
./src/darknetxxConverter \
  --convert-from yolo4 \
  --convert-src ./cars \
  --convert-to dxxwjz2p \
  --convert-dst ./cars.dxxwjz2p
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
| `DARKNETXX_CONVERTER_BINARY_NAME` | `<project binary name>Converter` | The name of the second binary and of its CMake target. |

```
# inside the project root directory

cmake -B build -S . -DENABLE_DARKNETXX_CONVERTER=OFF
```

The dxxwjz1, the dxxwjz2, the dxxwjz2p, the dxxwjz3 and the dxxwjz3p files are
the gzip compressed JSON documents, so both binaries
require the [zlib](/doc/sections/en_US/5-23-enabling-the-zlib-library.md) and
the [nlohmann JSON](/doc/sections/en_US/5-18-enabling-the-nlohmann-json-library.md)
libraries, the system ones or the ones the configure fetches.

### How the two binaries share their code

The second binary is not a copy of anything. Both binaries are entered through
one and the same `src/main.cpp`, the `src/app` component hands its sources to
both of them (see [src/app/CMakeLists.txt](/src/app/CMakeLists.txt)) and both
of them link the darknetxx library (see the
[The darknetxx library](/doc/sections/en_US/4-7-5-the-darknetxx-library.md)
section), the `darknetxx011::LibraryFacade` of which the `app::Application`
performs the conversion by, the very same way it performs the training and
the detection of the main binary.

The [src/darknetxxConverter-target.cmake](/src/darknetxxConverter-target.cmake)
compiles the converter target with the `DARKNETXX_CONVERTER_ONLY` preprocessor
definition, which tells the `ApplicationFactory` there is no default
application to create, the help message to list the conversion parameters
alone and the help and the version messages the name of the binary being run
(see [src/app/ApplicationBinaryName.h](/src/app/ApplicationBinaryName.h)).
Nothing else in the project branches on it.

The conversion itself is the one of the `WeightsConverter` of the network
infrastructure (see the [Network weights files](/doc/sections/en_US/4-7-4-network-weights-files.md)
section), which the `darknetxx_impl::WeightsConversionLib` of the library
hands the command line parameters of the context and the stop request of the
application to. The binary is covered by the `ITEST_converter` and the
`ITEST_converter_bash` integration tests (see the
[Enabling integration testing](/doc/sections/en_US/5-3-3-enabling-integration-testing.md)
section).
