### Enabling integration testing

The integration tests check the whole `darknetxx` executable the way it is used: they train a tiny network from scratch to detect the digits from `0` to `9` and then run the detection on every training image, expecting the digit of each one to be recognized. The training data set consists of the digit glyphs of the [misc/data/labels](/misc/data/labels) images, eight sizes of every digit, so the tests need no data of their own.

The same test comes in two flavors, which differ only in the way they drive the `darknetxx` executable and check its results:

| Test | Driven by |
| --- | --- |
| [ITEST_digits](/src/tests/integration/digits) | the [ITEST_digits.cpp](/src/tests/integration/digits/ITEST_digits.cpp) GoogleTest executable, which runs the `darknetxx` detection and parses its JSON results, while the [train-digits.cmake](/src/tests/integration/digits/train-digits.cmake) CMake script performs the training |
| [ITEST_digits_bash](/src/tests/integration/digits-bash) | the [ITEST_digits_bash.sh](/src/tests/integration/digits-bash/ITEST_digits_bash.sh) bash script alone, with no test framework: it runs both the `darknetxx` training and detection and parses the JSON results by the `awk` |

To enable the integration tests availability (for building and running) reconfigure the project with enabled `ENABLE_INTEGRATION_TESTS` variable as follows (GNU/Linux based):

```
# from the project root

cmake -B build -S . -DENABLE_INTEGRATION_TESTS=ON && cmake --build build
```

The flavors consist of the next CTest tests:

| Test | Performs |
| --- | --- |
| `ITEST_digits_training` | the training by the `darknetxx --train-cfg` run, with the weights of a previous run erased beforehand |
| `ITEST_digits.recognizes_digit_<N>.noArgs` | the `darknetxx --detect-cfg` detection on each of the eight images of the digit `<N>`, expecting the most confident detected object to be that digit |
| `ITEST_digits_dxxwjz3.<case>.noArgs` | the dxxwjz3 file of the training, the default format (the JSON network cfg, the class names, the losses of the 400 iterations of their indexes and the time), it's single chart of the training start, the detection with the dxxwjz3 file alone named after it's classes, the training resumed of it (the losses and the time added, the average loss going on), the mAPs of the `--map` training in the record, the chart of the file drawn by the `--draw-chart` and the training of the dxxwjz3 file of no weights the converter makes |
| `ITEST_digits_bash_training` | the same training as the `ITEST_digits_training` one does, by the bash script |
| `ITEST_digits_bash.recognizes_digit_<N>` | the same detection checks as the `ITEST_digits.recognizes_digit_<N>.noArgs` ones do, by the bash script |

The training is a CTest fixture of the detection tests of its flavor, so the `ctest` performs it first on its own, even when the tests are filtered by their names:

```
# run from the project build directory

# the GoogleTest flavor
ctest -R "ITEST_digits\." --output-on-failure

# the bash flavor
ctest -R "ITEST_digits_bash\." --output-on-failure
```

The training of each flavor takes about 15 seconds for the `Release` build, under a minute for the `Debug` one and about two minutes for the `Debug` one with the `ENABLE_SANITIZERS` option, while its detection tests take about half a minute more. The [debug-configure.sh](/scripts/build/debug-configure.sh) quick build script enables the `ENABLE_INTEGRATION_TESTS` option, so the `scripts/build/debug.sh --test` command performs the integration tests along with the rest of the tests (see the [Quick build scripts](/doc/sections/en_US/5-34-quick-build-scripts.md) section).

The build with the `ENABLE_SANITIZERS_THREADS` option passes the [misc/.tsan-suppress](/misc/.tsan-suppress) suppressions file to the ThreadSanitizer of the tests. It suppresses the known data race of the yolo layer training threads alone, which count the training statistics in a shared counter the same way the original Darknet does, so any other data race still fails the training.

Every file of a test - the generated data set, the network configuration, the trained weights, the training chart, the outputs of every `darknetxx` run and the detection results - stays inside the `src/tests/integration/digits` or the `src/tests/integration/digits-bash` directory of the build tree, the one of its flavor.

The bash flavor needs no build of its own, so its [ITEST_digits_bash.sh](/src/tests/integration/digits-bash/ITEST_digits_bash.sh) script checks any `darknetxx` executable by hand the same way, an installed one included. It takes the executable to check and the directory to keep every file of the test inside:

```
# from the project root

src/tests/integration/digits-bash/ITEST_digits_bash.sh train "$(command -v darknetxx)" /tmp/itest-digits misc/data/labels

for digit in 0 1 2 3 4 5 6 7 8 9; do
  src/tests/integration/digits-bash/ITEST_digits_bash.sh recognize "$(command -v darknetxx)" /tmp/itest-digits "${digit}"
done
```

#### The network weights files converter

The second pair of the integration tests checks the `darknetxxConverter` binary (see [The darknetxxConverter binary](/doc/sections/en_US/5-35-the-darknetxxConverter-binary.md) section) and the conversion command line of the `darknetxx` one the way a user runs them:

| Test | Performs |
| --- | --- |
| `*/ITEST_converter_<family>.converts_back_and_forth/*.noArgs` | the conversion of the weights files of every network variant the weights factory generates (every weights layer kind with the combinations of its options, the resolutions from the 2x2 pixels up to the DCI 4K ones, the random networks and the real world sized ones up to the YOLOv3) to the dxxwjz1 files and back: the file darknet saves and the file of every header version made for the load convert back into the same bytes, with the network cfg file and without it, the dxxwjz1 file converts into itself and loads the network the original file does |
| `*/ITEST_converter_dxxwjz2_<family>.converts_back_and_forth/*.noArgs` | the weights files of the random float bits (the NaNs of every payload, the infinities, the denormals) of every weights layer kind and of the random networks converted to the dxxwjz2 files and back, through the dxxwjz1 ones as well: the weights come back as the same bytes, the dxxwjz2 file loads the network the weights file does |
| `ITEST_converter.<case>.noArgs` | the reference weights files back and forth by both binaries, the gzip compressed JSON documents the binary writes (the network description of the dxxwjz2 one included), the help and the version messages, the failed conversions, which exit with the non zero status and write nothing |
| `ITEST_converter_bash.<case>` | the same checks of the reference weights files by the bash scripts of the [src/tests/integration/converter-bash](/src/tests/integration/converter-bash) directory alone, the stock `gzip` tool reading the dxxwjz1, the dxxwjz2, the dxxwjz2p and the dxxwjz3 files included, the dxxwjz3 file of the JSON network cfg converted into the dxxwjz2 one and back into the same file, the YOLO4 folder of the original weights converted into the dxxwjz2p project, it's dxxwjz2 weights and back into the folder of the same images, labels and weights |

```
# run from the project build directory

ctest -R "ITEST_converter" --output-on-failure
```

The variants of the tens of millions of the floats (the 4K resolution networks and the real world sized ones) convert for minutes in the `Debug` build, since every float of the network goes through the JSON text, so the `Release` build is the one to run the whole variant families quickly.
