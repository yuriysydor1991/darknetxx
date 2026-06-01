# Available command line parameters

The generated executable accepts a set of command line parameters that are parsed by the `CommandLineParser` class and described by the `ApplicationHelpPrinter` class. The always up to date list, as supported by the current build, may be printed at any moment with the `--help` (or `-h`) flag:

```
# from the build dir
./src/darknetxx --help
```

The options are grouped below by their purpose. The options with a value placeholder (for example `<N>` or `<FILE>`) expect that value to be provided right after the flag, for example `--threads 4`. The flags without a placeholder are toggles and take no value. Passing an unknown parameter prints this help and stops the execution.

## General options

| Option | Value | Description |
| --- | --- | --- |
| `--help`, `-h` | | print the help message and exit |
| `--version`, `-v` | | print application version, build git commit and configure date, then exit |
| `--log-file`, `-l` | `<FILE>` | write the application log into the given file |

## Network and model options

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

## Training options

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

## Detection and output options

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

## Camera, video and streaming options

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

To introduce a new command line parameter of your own examine the [Application runtime components](/doc/sections/en_US/4-7-1-application-runtime-components.md) section.
