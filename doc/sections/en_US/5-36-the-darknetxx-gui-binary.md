## The darknetxx-gui binary

Beside the `darknetxx` application the project builds the `darknetxx-gui`
binary: the very same training behind a GTK 4 window. It trains the network of
a YOLO4 folder, the one the [ImagesAnnotator](https://github.com/yuriysydor1991/ImagesAnnotator)
exports, and resumes the training the folder holds the weights of.

### Usage

```
# from the build dir

./src/darknetxx-gui
```

The window starts with the entry of the YOLO4 folder, filled with the current
working directory, and the `Choose…` button of the folder chooser dialog.
The chosen folder, or the one the entry names on the `Enter` key press, is
trained right away:

* the progress bar shows the done and all the iterations of the training;
* the current loss is shown in the big font, the average loss, the learning
  rate and the hours left beside it;
* the training chart below is updated on every iteration;
* the `Stop` button saves the weights and stops the training after the current
  iteration, and turns into the `Resume` one, which continues the training
  from the saved weights.

The training is resumed whenever the folder holds the weights of a previous
one: the newest `backup/<cfg name>_last` file a stopped training saves, or
the newest other `backup/<cfg name>_*` one, of any weights file format (the
training saves the `*.dxxwjz2` files, which keep the losses, the mAPs and the
time of every training resumed). The chart of the whole training record of the
dxxwjz2 weights is shown as soon as the folder is opened, before the first
iteration of the resumed training, which goes on drawing it (the chart image
of the previous training is continued for the weights of no record). The window closed during a training waits
for the training to save the weights. An OS stop signal (a SIGINT or a
SIGTERM, for example) closes the window the same way.

The binary accepts the very same command line the main one does (see the
[Available command line parameters](/doc/sections/en_US/6-2-3-available-command-line-parameters.md)
section), so the training options, `--threads` or `--net-resize-nth` for
example, apply to the trainings of the window, while the folder names the
network configuration, the weights and the class names files.

### The YOLO4 folder

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

### Building it

The binary is built by default and requires the GTKmm 4 development files
(the `libgtkmm-4.0-dev` package of the Debian and the Ubuntu 24.04 or newer,
see the [Required tools for the GNU/Linux based OS](/doc/sections/en_US/3-1-required-tools-for-the-GNU-Linux-based-OS.md)
section).

| CMake option | Default | Effect |
| --- | --- | --- |
| `ENABLE_DARKNETXX_GUI` | `ON` | Builds the `darknetxx-gui` binary, it's desktop entry and icon. Set it to `OFF` to build without the GTKmm 4. |
| `DARKNETXX_GUI_BINARY_NAME` | `<project binary name>-gui` | The name of the GUI binary and of it's CMake target. |

```
# inside the project root directory

cmake -B build -S . -DENABLE_DARKNETXX_GUI=OFF
```

The install step installs the desktop entry and the icon of the binary, named
after the `ua.org.kytok.template.darknetxx` application id, so the desktop
menus list it.

### Showing the window from the packages and the containers

| Package or container | What shows the window |
| --- | --- |
| DEB | the GTKmm 4 runtime library dependency, which the package computes of the binaries |
| snap | the `darknetxx-gui` app of the GNOME extension, which brings the GTK 4 runtime and the desktop, the wayland, the x11 and the opengl plugs |
| flatpak | the GNOME runtime with the GTKmm 4 module, the `darknetxx-gui` command and the Wayland, the X11, the IPC and the DRI permissions |
| Docker | the `docker-single-run-darknetxx-cpu-train-gui` and the `docker-single-run-darknetxx-gpu-train-gui` targets |

The Docker targets run the [scripts/docker/gui-run.sh](/scripts/docker/gui-run.sh)
script, which starts the `darknetxx-gui` of the training image on the X11
display of the host (allowed to the local containers while the run lasts) and
on the Wayland one of the session, when there is one, with the
`YOLO4_TRAINING_DATA_DIR` folder mounted as the working directory (see the
[Enabling the Docker container build and run](/doc/sections/en_US/5-12-enabling-the-docker-container-build-and-run.md)
section):

```
# inside the project root directory

cmake -B build -S . -DENABLE_DOCKER=ON -DENABLE_DOCKER_DARKNETXX_CPU_TRAIN=ON \
  -DYOLO4_TRAINING_DATA_DIR=/path/to/the/yolo4/folder
cmake --build build --target docker-single-run-darknetxx-cpu-train-gui
```

The Ubuntu 22.04 based images have no GTKmm 4 packages, so they build without
the GUI binary.

### How it is built of the project components

The binary is the main one with the `src/gui` component: it's
`app::GuiApplication` class (see [src/darknetxx-gui-target.cmake](/src/darknetxx-gui-target.cmake))
shows the window of the `darknetxxGtkmm4::GtkmmIniter`, while the
`DARKNETXX_GUI` definition tells the `ApplicationFactory` to create it. The
window trains by the `darknetxx011::ITrainer` the `darknetxx011::LibraryFacade`
of the darknetxx library creates (see the
[The darknetxx library](/doc/sections/en_US/4-7-5-the-darknetxx-library.md)
section), which runs the training of the library in a thread of it's own and
stops it by the stop request of the library context, the one the stop flag of
the application context raises too. The training reports every iteration to
the `darknetxx011::ITrainingListener` of the library context, which the window
implements.
