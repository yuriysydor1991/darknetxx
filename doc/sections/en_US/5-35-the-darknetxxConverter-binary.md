## The darknetxxConverter binary

The `darknetxxConverter` binary, the converter of the network weights files
between the original darknet weights format, the dxxwjz1, the dxxwjz2, the
dxxwjz2p, the dxxwjz3, the dxxwjz3p, the dxxwjz4 and the dxxwjz4p ones and of
the YOLO4 training folders into the dxxwjz2p, the dxxwjz3p and the dxxwjz4p
projects and back, which prints the YOLO4 network cfg the dxxwjz2, dxxwjz2p,
dxxwjz3, dxxwjz3p, dxxwjz4 and dxxwjz4p files keep and does nothing else, is
**no binary of this project any more**: it moved into the
[darknetxx-datadrivers](https://github.com/yuriysydor1991/darknetxx-datadrivers)
library together with the formats it converts (see the
[Enabling the darknetxx-datadrivers library](/doc/sections/en_US/5-39-enabling-the-darknetxx-datadrivers-library.md)
section), which builds and installs it beside itself. It keeps the name and
the command line it had here - the same `--convert-*` and `--dump-cfg` flags,
the same formats, the same behaviour - so the scripts and the data
preparation pipelines calling it keep working, and **no training nor detection
is compiled into it**, nor any darknet code at all.

The main `darknetxx` binary of this project keeps the very same conversion
command line (see the
[Converting the network weights files](/doc/sections/en_US/6-2-4-converting-the-network-weights-files.md)
section): a build of this project converts the files with no
`darknetxxConverter` binary at all, and the help message of the `darknetxx`
lists the conversion options beside the training and the detection ones.

### Getting it

The binary comes with the library: the
[misc/ci/install-darknetxx-datadrivers.sh](/misc/ci/install-darknetxx-datadrivers.sh)
script, or the CMake build of the library itself, installs it into the `bin`
directory of the install prefix, where it finds the library of it's prefix
on it's own:

```
# inside the project root directory

misc/ci/install-darknetxx-datadrivers.sh /opt/darknetxx

/opt/darknetxx/bin/darknetxxConverter --help
```

The snap package of this project carries it as the `darknetxxConverter` app
of the snap (see the
[Enabling the snap packager](/doc/sections/en_US/5-19-enabling-the-snap-package-manager.md)
section), the Docker images of the project install it with the library into
their `/usr` prefix (see the
[Enabling the Docker container build and run](/doc/sections/en_US/5-12-enabling-the-docker-container-build-and-run.md)
section), while the DEB package of this project packs the shared library alone
(see the
[Enabling DEB package generation with cpack](/doc/sections/en_US/5-10-enabling-DEB-package-generation-with-cpack.md)
section): the binary comes with the packages of the library itself.

### Usage

The binary accepts the conversion parameters the `darknetxx` one does, so the
[Converting the network weights files](/doc/sections/en_US/6-2-4-converting-the-network-weights-files.md)
section describes it in full:

```
darknetxxConverter \
  --convert-from weights \
  --convert-src ./yolov3.weights \
  --convert-to dxxwjz2 \
  --convert-dst ./yolov3.dxxwjz2 \
  --convert-cfg ./yolov3.cfg \
  --convert-names ./coco.names

# the network to train of the cfg file: the dxxwjz3 file of no weights
darknetxxConverter \
  --convert-to dxxwjz3 \
  --convert-dst ./to_train.dxxwjz3 \
  --convert-cfg ./yolov3.cfg \
  --convert-names ./coco.names

# the YOLO4 folder to the dxxwjz2p project of it's images
darknetxxConverter \
  --convert-from yolo4 \
  --convert-src ./cars \
  --convert-to dxxwjz2p \
  --convert-dst ./cars.dxxwjz2p

# the network cfg the dxxwjz3 file keeps into the cfg file
darknetxxConverter --dump-cfg ./yolov3.dxxwjz3 > ./yolov3.cfg
```

`--help` and `--version` name this binary and the version of the library,
while the help lists the general and the conversion options alone. A command line which asks
for no conversion nor cfg print has nothing to start - there is no training
nor detection behind this binary - so it is answered with the help message and
a non zero exit status.

### How the two converters differ

The original darknet weights file keeps no structure of it's own - it is the
float arrays of the layers one after another - so both converters read it by
the network of the cfg file (the `--convert-cfg` one), and they make that
network two different ways:

- the `darknetxx` binary builds the real darknet network of the cfg, the very
  one it trains and detects with (the
  `darknet_adaptor::loaders::NetworkWeightsConverter` of the
  [`src/NetworksLoaders/network-weights`](/src/NetworksLoaders/network-weights)
  directory);
- the `darknetxxConverter` binary of the library, which carries no darknet
  code, makes the structure of the network the cfg sections describe (the
  `CfgNetworkStructure` of the library): the layers, the sizes of their inputs
  and outputs and the float arrays of their weights, of no memory of them.

The `CTEST_NetworkWeights` component test of this project checks the two are
the layout of the very same arrays for the cfg files the repository carries
(the ones of the [misc/examples/cfg](/misc/examples/cfg) directory and of the
network loader tests, every one the darknet builds the network of), so both
binaries read and write the files byte to byte the same way (see the
[Network weights files](/doc/sections/en_US/4-7-4-network-weights-files.md#the-conversion)
section). The tests of the `darknetxxConverter` binary itself are the ones of
the library, while the `ITEST_converter` and the `ITEST_converter_bash`
integration tests of this project drive the conversion command line of the
`darknetxx` binary (see the
[Enabling integration testing](/doc/sections/en_US/5-3-3-enabling-integration-testing.md)
section).

The `ENABLE_DARKNETXX_CONVERTER` option and the
`DARKNETXX_CONVERTER_BINARY_NAME` cache variable, which built the binary in
this project, are gone with it.
