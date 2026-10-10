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
| `--train-cfg` | `<FILE>` | training network configuration (`*.cfg`, `*.dxxwjz2` or `*.dxxwjz3`) file, or the `*.dxxwjz2p` or `*.dxxwjz3p` project file, the training of which trains the images of it's annotations and updates it |
| `--detect-cfg` | `<FILE>` | detection network configuration (`*.cfg`, `*.dxxwjz2`, `*.dxxwjz2p`, `*.dxxwjz3` or `*.dxxwjz3p`) file |
| `--names` | `<FILE>` | class names file (overrides the classes of the dxxwjz2 and dxxwjz3 files and the cfg `[net]` `names=` key) |
| `--weights` | `<FILE>` | network weights file to load, of any format (see the [Network weights files](/doc/sections/en_US/4-7-4-network-weights-files.md) section); the `*.dxxwjz2`, the `*.dxxwjz2p`, the `*.dxxwjz3` and the `*.dxxwjz3p` files alone are the network the detection runs with, or the network to train for the file of no weights |
| `--image` | `<FILE>` | image file to run the detection on |
| `--gpus` | `<LIST>` | comma separated list of GPU indexes to use |
| `--threads` | `<N>` | number of worker threads to use |

## Network weights files conversion options

The four first options go together and convert the network weights file with no training nor detection started, while the `--convert-to dxxwjz2` (or `dxxwjz3`), the `--convert-dst` and the `--convert-cfg` ones alone make the network to train of the cfg file (the dxxwjz2 or the dxxwjz3 file of no weights), see the [Converting the network weights files](/doc/sections/en_US/6-2-4-converting-the-network-weights-files.md) section.

| Option | Value | Description |
| --- | --- | --- |
| `--convert-from` | `<FORMAT>` | the network weights file format to read: `weights`, `dxxwjz1`, `dxxwjz2`, `dxxwjz2p`, `dxxwjz3`, `dxxwjz3p` or `yolo4` (the YOLO4 training folder) |
| `--convert-src` | `<FILE>` | the network weights file to read, the folder of the `yolo4` format |
| `--convert-to` | `<FORMAT>` | the network weights file format to write: `weights`, `dxxwjz1`, `dxxwjz2`, `dxxwjz2p`, `dxxwjz3`, `dxxwjz3p` or `yolo4` |
| `--convert-dst` | `<FILE>` | the network weights file to write (replaced), the new folder of the `yolo4` format |
| `--convert-cfg` | `<FILE>` | the network cfg file of the weights: required to read the `weights` format, validates the rest, the `dxxwjz2` and the `dxxwjz3` ones keep it |
| `--convert-names` | `<FILE>` | the class names file the `dxxwjz2` and the `dxxwjz3` formats keep (overrides the cfg `[net]` `names=` key) |

## Training options

| Option | Value | Description |
| --- | --- | --- |
| `--clear` | | clear (reset) the training iteration counters |
| `--map` | | calculate mAP during the training (the class names of the dxxwjz2 weights need no names file), the mAPs go into the training record of the dxxwjz2 files |
| `--mAP-epochs` | `<N>` | calculate mAP every given number of epochs (default 4) |
| `--dont-resize-network` | | do not resize the network during the training |
| `--net-resize-nth` | `<N>` | resize the network every Nth training iteration (default 10) |
| `--data-reload-nth` | `<N>` | load a new training batch every Nth iteration only, reusing it in between (takes precedence over the flag below) |
| `--dont-reload-data-every-iteration` | | keep training on the loaded batch instead of loading a new one every iteration (a network resize still reloads it) |
| `--stop-less-avg-loss` | `<VALUE>` | stop the training once the average loss reaches the given value or less |
| `--save-format` | `<FORMAT>` | the network weights file format the training saves: `weights`, `dxxwjz1`, `dxxwjz2`, `dxxwjz2p`, `dxxwjz3` (the default) or `dxxwjz3p` |
| `--chart-path` | `<FILE>` | the chart image the training chart continues for the weights of no training record (the training record of the dxxwjz2 and dxxwjz3 weights redraws the whole chart instead); the chart of every training start is saved as the new `chart_<base>_<YYYY-MM-DD--HH-MM-SS>.png` file of the start date and time |
| `--draw-chart` | `<FILE>` | draw the training chart of the record of the dxxwjz2 or dxxwjz3 `--weights` file into the PNG file, with no training nor detection |
| `--save-charts-every-iter` | | save the training chart on every iteration, not every 100 (the cfg `savechart=` key overrides both, see below) |
| `--dont-save-charts-every-iter` | | save the training chart every 100 iterations (the default) |
| `--draw-precision` | | draw the mAP curve on every iteration once it's calculated, not only on the calculations |

## Detection and output options

| Option | Value | Description |
| --- | --- | --- |
| `--thresh` | `<VALUE>` | detection confidence threshold (default 0.25) |
| `--iou-thresh` | `<VALUE>` | intersection over union threshold (default 0.5) |
| `--hier` | `<VALUE>` | hierarchical detection threshold (default 0.5) |
| `--points` | `<N>` | recall points of the training mAP calculation (`101` for MS COCO, `11` for PascalVOC 2007, `0` for the area under the curve) |
| `--letter-box` | | resize the image keeping the aspect ratio (letter box) |
| `--ext-output` | | print extended output with the bounding box coordinates |
| `--save-labels` | | save the detected labels |
| `--out` | `<FILE>` | file to write the results into |
| `--benchmark` | | benchmark the detection performance (implies `--dont-show`) |
| `--benchmark-layers` | | benchmark every network layer performance (GPU builds only) |

## Window options

| Option | Value | Description |
| --- | --- | --- |
| `--show` | | show the GUI windows (enabled by default, reverts `--dont-show`) |
| `--dont-show` | | do not show the GUI windows |
| `--show-imgs` | | show the augmented training images |

## Not supported yet options

The options of the original Darknet video demo, results streaming and anchors calculation modes, which the `darknetxx` has no code of yet. They are reserved and rejected with an error when given, instead of being silently ignored.

| Option | Value | Original meaning |
| --- | --- | --- |
| `--width`, `--height` | `<N>` | the anchors calculation input size |
| `--num-of-clusters` | `<N>` | the number of the anchor clusters |
| `--prefix` | `<PREFIX>` | the prefix of the saved video frames |
| `--out-filename` | `<FILE>` | the output video file |
| `--dontdraw-bbox` | | no bounding boxes on the video frames |
| `--cam-index` | `<N>` | the camera (web cam) to capture from |
| `--frame-skip` | `<N>` | the frames to skip between the detections |
| `--avg-frames` | `<N>` | the frames to average the detections over |
| `--time-limit-sec` | `<SEC>` | the video processing time limit |
| `--mjpeg-port` | `<PORT>` | the port to stream the MJPEG result on |
| `--json-port` | `<PORT>` | the port to stream the JSON result on |
| `--http-post-host` | `<HOST>` | the host to send the HTTP POST results to |
| `--json-file-output` | `<FILE>` | the file to write the video JSON results into |

## Data files of the network configuration

The original Darknet took the class names, the training and validation image lists and the backup directory from a separate `*.data` file. The `darknetxx` reads them from the `[net]` section of the network configuration (`*.cfg`) file instead:

| Key | Default | Description |
| --- | --- | --- |
| `names=` | `data/names.list` | class names file, one name per line |
| `train=` | `data/train.txt` | list of the training images |
| `valid=` | the `train=` value | list of the validation images for the mAP calculation (`--map`) |
| `backup=` | `backup` | directory to save the trained weights into |

The `[net]` section tells how often the training saves its files too:

| Key | Default | Description |
| --- | --- | --- |
| `saveweights=` | `1000`, `10000` for the `max_batches=` of 10000 or more | iterations between the numbered `<base>_<iteration>_<avg loss>` weights files, e.g. the `net_1200_0.734521` one |
| `savelast=` | `100` | iterations between the saves of the `<base>_last` weights file, the one a stopped training saves and the [darknetxx-train-gui](/doc/sections/en_US/5-36-the-GUI-binaries.md#the-darknetxx-train-gui-binary) resumes first (saved only while it's less than the `saveweights=` one) |
| `savechart=` | none | iterations between the saves of the training chart image, which the darknetxx-train-gui shows; none leaves them to the `--save-charts-every-iter` flag (every 100 iterations, every iteration of the flag and of the darknetxx-train-gui) |

The relative paths are resolved against the current working directory. The `--names` command line parameter takes precedence over the classes of the dxxwjz2 weights files and over the `names=` key, so a stock configuration file may be used for the detection as is, for example:

```
darknetxx --detect-cfg yolov4.cfg --weights yolov4.weights --names coco.names --image dog.jpg
```

The dxxwjz2 file keeps the network cfg and the class names beside the weights, so it's detected with alone, and the dxxwjz2 file of no weights is trained alone (the dxxwjz3 file the same way, of the JSON of the network cfg):

```
darknetxx --weights yolov4_final.dxxwjz2 --image dog.jpg
darknetxx --weights network-to-train.dxxwjz2
```

To introduce a new command line parameter of your own examine the [Application runtime components](/doc/sections/en_US/4-7-1-application-runtime-components.md) section.
