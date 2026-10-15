## The GUI binaries

Beside the `darknetxx` application the project builds two binaries of the
very same library works behind the GTK 4 windows:

* the `darknetxx-train-gui`, which trains the network of a YOLO4 folder or of
  a dxxwjz2p or dxxwjz3p project file (see the
  [The darknetxx-train-gui binary](#the-darknetxx-train-gui-binary) section);
* the `darknetxx-recognition-gui`, which detects the objects in an image or
  classifies the whole image by the network of a weights file of any format
  (see the [The darknetxx-recognition-gui binary](#the-darknetxx-recognition-gui-binary)
  section).

The network editor binaries of the GTK 4 and of the Qt 6 beside them create
and edit the networks of the dxxwjz3p projects (see the
[The network editor binaries](/doc/sections/en_US/5-38-the-network-editor-binaries.md)
section).

Both accept the very same command line the main binary does (see the
[Available command line parameters](/doc/sections/en_US/6-2-3-available-command-line-parameters.md)
section). An OS stop signal (a SIGINT or a SIGTERM, for example) closes their
windows the way the user does.

### The darknetxx-train-gui binary

It trains the network of a training project: the YOLO4 folder, or the dxxwjz2p
project file, the one of everything of the YOLO4 folder but the images (see the
[The dxxwjz2p project file format](/doc/sections/en_US/4-7-4-network-weights-files.md#the-dxxwjz2p-project-file-format)
section), or the dxxwjz3p one of the JSON data alone (see the
[The dxxwjz3p project file format](/doc/sections/en_US/4-7-4-network-weights-files.md#the-dxxwjz3p-project-file-format)
section), the ones the
[ImagesAnnotator](https://github.com/yuriysydor1991/ImagesAnnotator) exports
(see the [The files of the ImagesAnnotator](/doc/sections/en_US/4-7-4-network-weights-files.md#the-files-of-the-imagesannotator)
section), and resumes the training the project holds the weights of.

```
# from the build dir

./src/darknetxx-train-gui
```

The window starts with the entry of the training project, filled with the
current working directory, the `Folder…` button of the YOLO4 folder chooser
dialog and the `Project…` button of the dxxwjz2p and dxxwjz3p file chooser one. The chosen
folder or file, or the one the entry names on the `Enter` key press, is
trained right away:

* the progress bar shows the done and all the iterations of the training and the done percent of them, e.g. `1200 / 6000 iterations (20.0%)`;
* the current loss is shown in the big font, the average loss, the learning
  rate and the hours left beside it;
* the training chart below is updated on every iteration (every `savechart=`
  ones of the cfg of that key, see the [Data files of the network configuration](/doc/sections/en_US/6-2-3-available-command-line-parameters.md#data-files-of-the-network-configuration)
  section);
* the `Stop` button saves the weights and stops the training after the current
  iteration, and turns into the `Resume` one, which continues the training
  from the saved weights.

The training is resumed whenever the folder holds the weights of a previous
one: the newest `backup/<cfg name>_last` file a stopped training saves, or
the newest other `backup/<cfg name>_*` one, of any weights file format (the
training saves the `*.dxxwjz3` files, which keep the losses, the mAPs and the
time of every training resumed). The chart of the whole training record of the
dxxwjz3 weights is shown as soon as the folder is opened, before the first
iteration of the resumed training, which goes on drawing it (the chart image
of the previous training is continued for the weights of no record). The window closed during a training waits
for the training to save the weights, an OS stop signal closes it the same
way.

The training options of the command line, `--threads` or `--net-resize-nth`
for example, apply to the trainings of the window, while the project names the
network configuration, the weights and the class names files.

#### The YOLO4 folder

| Path | Content |
| --- | --- |
| `cfg/*.cfg` | the network configuration, the first one by the name is trained |
| `data/obj.names` | the class names, one per line |
| `data/train.txt` | the training images list (the `train=` key of the cfg `[net]` section names another one) |
| `backup/` | the trained weights (the `backup=` key of the cfg `[net]` section names another directory), created when missing |

The training runs with the folder as the working directory, so the relative
paths of the cfg and of the images list are relative to the folder, and the
chart of every training start is saved into it as the new
`chart_<cfg name>_<YYYY-MM-DD--HH-MM-SS>.png` file of the start date and time,
the newest of which the window shows.

#### The dxxwjz2p project

The project file (the dxxwjz3p one the same way) keeps the network, the class names, the latest weights with
the training record and the annotations of the images, which the training
reads in place of the lists and the label files: the images stay where they
are, the folders the project names relative to it beside it move with it.
Every save of the training updates the project with the latest weights, so the
training of the project is resumed of the project itself, while the backups of
the save format go into the backup folder of the cfg `backup=` key beside the
project, which is created when missing, and the charts of the training starts
are saved beside the project. The YOLO4 folder converts into the project and
back with the converter (see the
[Converting the network weights files](/doc/sections/en_US/6-2-4-converting-the-network-weights-files.md)
section).

### The darknetxx-recognition-gui binary

It recognizes the images by the network of a weights file the way the network
does: the network of the YOLO, the Gaussian YOLO, the region or the detection
layers detects the objects in the image, the boxes of which it draws over the
image, while the network of none of them (the softmax classifier, for
example) classifies the whole image.

```
# from the build dir

./src/darknetxx-recognition-gui
```

The window has the entries of the files of the recognition, each one with the
`Choose…` button of the file chooser dialog:

| Entry | File |
| --- | --- |
| `Weights file` | the weights of the network: the original darknet `*.weights` file, the `*.dxxwjz1`, the `*.dxxwjz2`, the `*.dxxwjz2p`, the `*.dxxwjz3` or the `*.dxxwjz3p` one (see the [Network weights files](/doc/sections/en_US/4-7-4-network-weights-files.md) section) |
| `Network cfg` | the network configuration, none for the `*.dxxwjz2`, the `*.dxxwjz2p`, the `*.dxxwjz3` and the `*.dxxwjz3p` weights files, which keep their network, the trainings of the `darknetxx-train-gui` save those |
| `Class names` | the class names, none for the ones of the `*.dxxwjz2`, the `*.dxxwjz2p`, the `*.dxxwjz3` and the `*.dxxwjz3p` weights files or of the `names=` key of the cfg `[net]` section; the classes of no name are named `class <index>` |
| `Image` | the image to recognize, of any format the OpenCV reads |

The chosen file, the `Enter` key press of an entry, the `Recognize` button and
the changed `Threshold` recognize the image in a thread of it's own, so the
window stays responsive, and the entries changed meanwhile are recognized
right after it:

* the image is shown with the objects boxed and labelled by the names of their
  classes and their probabilities, the small images enlarged so the labels
  stay readable;
* the list beside it tells every object or class, the most probable first,
  with it's box in the image pixels the darknet way, e.g.
  `dog: 97.5% (left_x: 12 top_y: 40 width: 128 height: 96)`;
* the status line tells the count of the objects and the time the network
  took, or the reason the recognition failed (the image unable to be read, the
  weights file of no network and no cfg file given, the weights of another
  network and the like);
* the objects and the classes are the ones more probable than the threshold.

The network is loaded once for it's files, so the next images of the very
same files are recognized right away. The `--weights`, the `--detect-cfg`, the
`--names`, the `--image` and the `--thresh` command line parameters fill the
entries in, and the image of the weights given is recognized as soon as the
window shows:

```
# from the build dir

./src/darknetxx-recognition-gui --weights /path/to/the/backup/digits_last.dxxwjz3 \
  --image /path/to/the/image.png --thresh 0.5
```

The `--threads` one tells the threads of the recognitions.

### Building them

The binaries are built by default and require the GTKmm 4 development files
(the `libgtkmm-4.0-dev` package of the Debian and the Ubuntu 24.04 or newer,
see the [Required tools for the GNU/Linux based OS](/doc/sections/en_US/3-1-required-tools-for-the-GNU-Linux-based-OS.md)
section).

| CMake option | Default | Effect |
| --- | --- | --- |
| `ENABLE_DARKNETXX_GUI` | `ON` | Builds both GUI binaries and the GTK 4 network editor one, their desktop entries and icons. Set it to `OFF` to build without the GTKmm 4. |
| `DARKNETXX_TRAIN_GUI_BINARY_NAME` | `<project binary name>-train-gui` | The name of the GUI training binary and of it's CMake target. |
| `DARKNETXX_RECOGNITION_GUI_BINARY_NAME` | `<project binary name>-recognition-gui` | The name of the GUI recognition binary and of it's CMake target. |

```
# inside the project root directory

cmake -B build -S . -DENABLE_DARKNETXX_GUI=OFF
```

The install step installs the desktop entries of the binaries (the
[misc/desktop/darknetxx-gui.desktop.in](/misc/desktop/darknetxx-gui.desktop.in)
template) and their icon, the Darknet logo (the
[misc/desktop/darknet-logo.png](/misc/desktop/darknet-logo.png), the
`darknet.ico` of the Darknet sources on a white disc), named after their
application ids, the `ua.org.kytok.template.darknetxx` one of the training
binary and the `ua.org.kytok.template.darknetxx.recognition` one of the
recognition binary, so the desktop menus list both. The binaries embed the
logo too and show it as the icon of their windows even uninstalled, and on
the MS Windows they carry the `darknet.ico` one.

### Showing the windows from the packages and the containers

| Package or container | What shows the windows |
| --- | --- |
| DEB | the GTKmm 4 runtime library dependency, which the package computes of the binaries |
| snap | the `darknetxx-train-gui`, the `darknetxx-recognition-gui` and the `darknetxx-net-editor-gui` apps of the GNOME extension, which brings the GTK 4 runtime and the desktop, the wayland, the x11 and the opengl plugs |
| flatpak | the GNOME runtime with the GTKmm 4 module, the `darknetxx-train-gui` command (the desktop entries of the recognition and the network editor binaries start the other ones) and the Wayland, the X11, the IPC and the DRI permissions |
| Docker | the `docker-single-run-darknetxx-cpu-train-gui` and the `docker-single-run-darknetxx-gpu-train-gui` targets of the training binary |

Every package carries the desktop entries and the icons as well: the DEB, the
RPM and the FreeBSD pkg ones install them, the snap names them in the
`desktop` keys of it's GUI apps and takes the logo as it's `icon`, the flatpak
exports them, since their names start with it's id, and the WIX MSI one
creates the Start Menu and the desktop shortcuts of the binaries instead (see
the [Enabling WIX MSI package generation with cpack](/doc/sections/en_US/5-21-enabling-WIX-MSI-package-generation-with-cpack.md)
section).

The Docker targets run the [scripts/docker/gui-run.sh](/scripts/docker/gui-run.sh)
script, which starts the `darknetxx-train-gui` of the training image on the
X11 display of the host (allowed to the local containers while the run lasts)
and on the Wayland one of the session, when there is one, with the
`YOLO4_TRAINING_DATA_DIR` folder mounted as the working directory (see the
[Enabling the Docker container build and run](/doc/sections/en_US/5-12-enabling-the-docker-container-build-and-run.md)
section):

```
# inside the project root directory

cmake -B build -S . -DENABLE_DOCKER=ON -DENABLE_DOCKER_DARKNETXX_CPU_TRAIN=ON \
  -DYOLO4_TRAINING_DATA_DIR=/path/to/the/yolo4/folder
cmake --build build --target docker-single-run-darknetxx-cpu-train-gui
```

The container gets every CPU core and the whole memory of the docker host
(the `--cpus` and the `--memory` of it's `docker info`), and the training runs
with as many `--threads`. The `DOCKER_GUI_TRAINING_ARGS` CMake list holds the
further `darknetxx-train-gui` arguments (the `--threads` of it's own among
them), while the
[scripts/docker/cpu-train-gui.sh](/scripts/docker/cpu-train-gui.sh) script
configures and runs the CPU training of a folder in one go:

```
# inside the project root directory

scripts/docker/cpu-train-gui.sh /path/to/the/yolo4/folder -DDOCKER_GUI_TRAINING_ARGS=--map
```

The Ubuntu 22.04 based images have no GTKmm 4 packages, so they build without
the GUI binaries.
The images with the GUI build the OpenCV with the Qt 5 windows, as the distro
OpenCV has them, since the GTK 4 window aborts on start next to the GTK 3 ones.

### How they are built of the project components

The binaries are the main one with the `src/gui` component (see
[src/darknetxx-gui-targets.cmake](/src/darknetxx-gui-targets.cmake)): their
`app::TrainGuiApplication` and `app::RecognitionGuiApplication` classes, the
`app::GuiApplication` ones, show the windows of the
`darknetxxGtkmm4::TrainGtkmmIniter` and the
`darknetxxGtkmm4::RecognitionGtkmmIniter`, while the `DARKNETXX_TRAIN_GUI`
and the `DARKNETXX_RECOGNITION_GUI` definitions tell the `ApplicationFactory`
which one to create. Each window is a `darknetxxGtkmm4::AWindowContext` of
it's widgets and of the event handlers of them, the
`darknetxxGtkmm4::window::event_handlers::ChooserEventsHandler` of every file
or folder entry and the `StopSignalEventsHandler` of the OS stop signal among
them.

The training window trains the `darknetxx011::ATrainingProject` the
`LibraryFacade::open_training_project()` opens of the entry path (the
`darknetxx011::Yolo4Folder` of a folder, the project of a `*.dxxwjz2p` or a
`*.dxxwjz3p` file) by the `darknetxx011::ITrainer` the
`darknetxx011::LibraryFacade` of the darknetxx library creates (see the
[The darknetxx library](/doc/sections/en_US/4-7-5-the-darknetxx-library.md)
section), which runs the training of the library in a thread of it's own and
stops it by the stop request of the library context, the one the stop flag of
the application context raises too. The training reports every iteration to
the `darknetxx011::ITrainingListener` of the library context, which the window
implements.

The recognition window recognizes by the `darknetxx011::IRecognizer` of the
library facade in a thread of it's own and shows the
`darknetxx011::Recognition` it gives: the `darknetxx011::RecognizedObject`
list and the PNG image of the objects boxed.
