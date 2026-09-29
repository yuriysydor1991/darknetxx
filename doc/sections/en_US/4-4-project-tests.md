## Project tests

Project already contains some simple test samples. Based on them you may implement full test of the introduced code. 

Besides the unit and the component tests of separate classes and functions, the project holds an integration test of the whole `darknetxx` executable, which trains a tiny network to recognize the digits and checks its detections. The test comes in two flavors: a GoogleTest one and a bash script one with no test framework (see the [Enabling integration testing](/doc/sections/en_US/5-3-3-enabling-integration-testing.md) section).

The network infrastructure (see the [Darknet adaptor components](/doc/sections/en_US/4-7-2-darknet-adaptor-components.md) section) comes with it's component tests:

- `CTEST_OrigWeightsFile` covers the weights files reader and writer (the `OrigWeightsFile` class and the `save_weights`, `save_weights_upto`, `load_weights` and `load_weights_upto` functions of the Darknet C API). It generates thousands of networks and their weights files: every weights layer kind with the combinations of it's options, every header version, the resolutions from the 2x2 pixels up to the DCI 4K ones, the random networks and the real world sized ones up to the 248 MB of the YOLOv3 weights. Every file is checked byte by byte against the layout the test computes independently of the Darknet code.
- `CTEST_OrigDefaultLoader` covers the network cfg parser: the reference networks and their weights, the generated networks of every layer kind checked against their specifications, the memory of the layers (the buffers of the training and the inference networks, the aliased and the shared ones), the cfg errors and the networks the loader hands to the `DarknetContext` for the training, the mAP calculation and the detection. `CTEST_CfgSection` covers the cfg file reader.
- `CTEST_ALayerAdaptor` and `CTEST_ANetworkAdaptor` cover the memory of the layers and the networks. The latter resizes the networks of every resizable layer kind and the generated ones back and forth and checks the geometry and every buffer size of their layers against the fresh parse of the new size, and the computation of every layer against the fresh network one, value by value. It computes every generated network, fuses the batch normalization without changing the network output and checks the binary weights of the XNOR layers.
- `CTEST_NetworksCApi` covers the Darknet C API implemented by the C++ adaptors, the errors which end the process included.

The generated networks and their weights files come from the `DarknetXXWeightsFactory` library of the [`src/tests/component/weights-factory`](/src/tests/component/weights-factory) directory, which the component tests share. The `DARKNETXX_WEIGHTS_TESTS_SEED` and the `DARKNETXX_WEIGHTS_TESTS_RANDOM_COUNT` environment variables change the seed and the count of the random networks (160 by default), while the `DARKNETXX_WEIGHTS_TESTS_KEEP_DIR` one names the directory to keep the generated cfg and weights files of the `CTEST_OrigWeightsFile` test in, for example:

```
# run from the project build directory

DARKNETXX_WEIGHTS_TESTS_RANDOM_COUNT=5000 ctest -R "CTEST_OrigWeightsFile_random" --output-on-failure
```

The memory tests are the most useful in the build of the sanitizers (see the [Enabling sanitizers](/doc/sections/en_US/5-13-enabling-sanitizers.md) section), which reports every access past a buffer the computation of the generated networks makes.

See the [Project build](/doc/sections/en_US/5-project-build.md) subsection to enable unit testing with CMake.
