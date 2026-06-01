# Run the executable

## IDE run

Of course, if your IDE supports CMake build system integration you may just press the `Build` and/or `Run` button somewhere in the IDE window with opened project and it's done! No need to perform a search and execute commands.

## Command line run

### Searching for the generated executable

To find executable in the project build directory execute the `find` command:

```
find . -name 'darknetxx'
```

The `darknetxx` is a default executable name. Replace it with your custom one if you change the name of the project in the root `CMakeLists.txt` file (the `PROJECT_NAME` and/or `PROJECT_BINARY_NAME` variable).

### Starting the generated executable

If executable compiles and is present in the build directory start it in the terminal with path found from a previous subsection by a command:

```
# from the build dir
./src/darknetxx
```

Once again, the `darknetxx` is the **default** name of the project. Replace it with our own custom one if it was changed in the project's root `CMakeLists.txt` file (the `PROJECT_NAME` and/or `PROJECT_BINARY_NAME` variable).

## Available command line parameters

The generated executable accepts a set of command line parameters that are parsed by the `CommandLineParser` class and described by the `ApplicationHelpPrinter` class. The always up to date list, as supported by the current build, may be printed at any moment with the `--help` (or `-h`) flag:

```
# from the build dir
./src/darknetxx --help
```

The options are grouped below by their purpose. The options with a value placeholder (for example `<N>` or `<FILE>`) expect that value to be provided right after the flag, for example `--threads 4`. The flags without a placeholder are toggles and take no value. Passing an unknown parameter prints this help and stops the execution.

### General options

| Option | Value | Description |
| --- | --- | --- |
| `--help`, `-h` | | print the help message and exit |
| `--version`, `-v` | | print application version, build git commit and configure date, then exit |
| `--log-file`, `-l` | `<FILE>` | write the application log into the given file |

### Network and model options

| Option | Value | Description |
| --- | --- | --- |
| `--train-cfg` | `<FILE>` | training network configuration (`*.cfg`) file |
| `--detect-cfg` | `<FILE>` | detection network configuration (`*.cfg`) file |
| `--weights` | `<FILE>` | network weights file to load |
| `--image` | `<FILE>` | image file to run the detection on |
| `--gpus` | `<LIST>` | comma separated list of GPU indexes to use |
| `--threads` | `<N>` | number of worker threads to use |
| `--width` | `<N>` | override the network input width |
| `--height` | `<N>` | override the network input height |
| `--num-of-clusters` | `<N>` | number of anchor clusters (default 5) |

### Training options

| Option | Value | Description |
| --- | --- | --- |
| `--clear` | | clear (reset) the training iteration counters |
| `--map` | | calculate mAP during the training |
| `--mAP-epochs` | `<N>` | calculate mAP every given number of epochs (default 4) |
| `--dont-resize-network` | | do not resize the network during the training |
| `--net-resize-nth` | `<N>` | resize the network every Nth training iteration (default 10) |
| `--data-reload-nth` | `<N>` | reload the training data every Nth iteration |
| `--dont-reload-data-every-iteration` | | do not reload the training data on every iteration |
| `--stop-less-avg-loss` | `<VALUE>` | stop the training once the average loss reaches the given value or less |
| `--chart-path` | `<FILE>` | path to save the training chart into |
| `--dont-save-charts-every-iter` | | do not save the training chart on every iteration |
| `--draw-precision` | | draw the precision curve on the training chart |

### Detection and output options

| Option | Value | Description |
| --- | --- | --- |
| `--thresh` | `<VALUE>` | detection confidence threshold (default 0.25) |
| `--iou-thresh` | `<VALUE>` | intersection over union threshold (default 0.5) |
| `--hier` | `<VALUE>` | hierarchical detection threshold (default 0.5) |
| `--points` | `<N>` | number of points to use for the mAP calculation |
| `--letter-box` | | resize the image keeping the aspect ratio (letter box) |
| `--ext-output` | | print extended output with the bounding box coordinates |
| `--save-labels` | | save the detected labels |
| `--dontdraw-bbox` | | do not draw the detected bounding boxes |
| `--prefix` | `<PREFIX>` | prefix for the saved output file names |
| `--out` | `<FILE>` | file to write the results into |
| `--out-filename` | `<FILE>` | output video/image file name |
| `--benchmark` | | benchmark the detection performance |

### Camera, video and streaming options

| Option | Value | Description |
| --- | --- | --- |
| `--show` | | show the GUI window (enabled by default) |
| `--dont-show` | | do not show the GUI window |
| `--show-imgs` | | show the augmented training images |
| `--cam-index` | `<N>` | camera (web cam) index to capture from |
| `--frame-skip` | `<N>` | number of frames to skip between the detections |
| `--avg-frames` | `<N>` | number of frames to average the detections over (default 3) |
| `--time-limit-sec` | `<SEC>` | stop the processing after the given number of seconds |
| `--mjpeg-port` | `<PORT>` | port to stream the MJPEG result on |
| `--json-port` | `<PORT>` | port to stream the JSON result on |
| `--http-post-host` | `<HOST>` | host to send the HTTP POST results to |
| `--json-file-output` | `<FILE>` | file to write the JSON results into |

To introduce a new command line parameter of your own examine the [Introducing custom command line parameters](/doc/sections/en_US/2-project-structure.md#introducing-custom-command-line-parameters) section.

## Tests run

### Run tests by the ctest

If enabled by the developer through the `ENABLE_UNIT_TESTS` variable and successfully build, the one may run all available test by the `ctest` command from a project build directory (GNU/Linux based):

```
# run from the project build directory

ctest
```

Alternatively, run the `ctest` command from any location by specifying the test dir (GNU/Linux based):

```
ctest --tests-dir /path/to/the/project/build/directory
```

In order to run particular test execute the `ctest` command with test's name after the `-R` command line flag. For example, for the `UTEST_ApplicationFactory` test it'll look something like this:

```
# from the project build directory 

ctest -R UTEST_ApplicationFactory
```

### Manual tests run

Currently tests are separated in different files named with `UTEST_*` pattern for the unit tests.

You may search for all compiled available tests by a next command:

```
# from the project root or a build directory

find -type f -executable -name 'UTEST_*'
```

After that, choose particular test of interest and execute it manually if needed. For example, for the `ApplicationFactory` class pass it's UT relative file system path from a project's root directory into the command line and hit enter (GNU/Linux based):

```
# from the project root

./build/src/app/tests/unit/ApplicationFactory/UTEST_ApplicationFactory
```
