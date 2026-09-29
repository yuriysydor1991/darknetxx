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
