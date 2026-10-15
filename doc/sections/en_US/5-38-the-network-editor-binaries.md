## The network editor binaries

Beside the training and the recognition GUI binaries (see the
[The GUI binaries](/doc/sections/en_US/5-36-the-GUI-binaries.md) section) the
project builds the network editors, the windows of one and the same editor
of two toolkits:

* the `darknetxx-net-editor-gui` of the GTK 4 (the GTKmm 4);
* the `darknetxx-net-editor-qt-gui` of the Qt 6.

Each one creates the network of the new dxxwjz3p project or edits the network
of the dxxwjz3p project (see the
[The dxxwjz3p project file format](/doc/sections/en_US/4-7-4-network-weights-files.md#the-dxxwjz3p-project-file-format)
section) or of another darknetxx file keeping the network (the dxxwjz3, the
dxxwjz2p and the dxxwjz2 ones): the options of the `[net]` section, the class
names and the layers, which it adds, removes and reorders.

```
# from the build dir

./src/darknetxx-net-editor-gui
./src/darknetxx-net-editor-qt-gui --train-cfg /path/to/the/project.dxxwjz3p
```

Both accept the very same command line the main binary does (see the
[Available command line parameters](/doc/sections/en_US/6-2-3-available-command-line-parameters.md)
section), the `--train-cfg` one names the project they open at once, the new
network is shown otherwise. An OS stop signal (a SIGINT or a SIGTERM, for
example) closes their windows the way the user does.

### The window

| Part | What it is |
| --- | --- |
| the toolbar | the `New`, the `Open…`, the `Save` and the `Save as…` buttons of the file of the network and the name of the file (the `New network` of none), the `*` of the changes of no save after it; the window title tells them too |
| `Network` | the static table of the main `[net]` options: a row of each one |
| `Classes` | the names of the classes the network detects, a name on a line (the empty lines name no class) |
| `Layers` | a row of each layer under the table, the drop down of the layer types and the `+` button below them |
| the status line | the outcome of the open and of the save: the reason of the failure, the weights the save dropped |

Every option row is the name of the option, the editor of it's value and the
description of it:

```
subdivisions  [16                ]  The mini batches of the batch: the batch / subdivisions images go
                                    through the network at once, more of them take less memory: an
                                    integer from 1 to 64, a divisor of the batch=64
```

* the typed values (the numbers, the lists of them, the paths) are the entry,
  which takes the characters of the value alone: the digits and the minus of
  the integers, the decimal point and the exponent of the real numbers too,
  the commas of the lists of them, no white space of the texts (the darknet
  drops it);
* the listed values (the activations, the learning rate policies, the 0 and 1
  flags and the like) are the drop down of them;
* the empty entry (the first item of the drop down) is the option of no line
  in the cfg, the default the darknet takes for it, which the entry tells
  (`default: 0.9`, `required` of the option the section can't do without,
  `unset` of the option of no default);
* the description tells what the option does and the values it takes, the
  limits of the moment among them, and the reason the value is wrong below it
  in red (the entry is red then too).

The limits of the options which depend on the rest of the network follow the
network at once: every edit checks every row anew.

| Option | The values it takes |
| --- | --- |
| `[net]` `subdivisions` | a divisor of the `batch` |
| `[net]` `burn_in` | up to the `max_batches` |
| `[net]` `steps`, `scales` | the ascending steps up to the `max_batches`, as many as the scales; both required by the `steps` policy |
| `[net]` `sequential_subdivisions` | up to the `subdivisions` |
| `[net]` `width`, `height` | required of the network of no `inputs`; the multiple of the factor the layers downsample the input by for the network of the detection layers (the 32 of the YOLOv4) |
| `[convolutional]` `filters` | the `(classes + 5) * masks` of the next `[yolo]` layer, the `(classes + 9) * masks` of the next `[Gaussian_yolo]` one, the `num * (classes + coords + 1)` of the next `[region]` one |
| `[convolutional]` `groups` | a divisor of the `filters` |
| `[yolo]`, `[Gaussian_yolo]`, `[region]` `classes` | the count of the class names |
| `[yolo]`, `[Gaussian_yolo]` `mask` | below the `num` |
| `[yolo]`, `[Gaussian_yolo]`, `[region]` `anchors` | the `2 * num` values |
| `[route]` `layers`, `[shortcut]` `from`, `[scale_channels]` `from`, `[sam]` `from` | the layers before it: the negative indexes count back from the layer, the rest are the absolute ones, the description names the layers they refer to |
| `[route]` `group_id` | below the `groups` |

The rest are the static ranges of the values (the `momentum` from 0 to 1, the
`size` from 1, for example). The options of a section the editor knows nothing
of stay as they are, told so.

### The layers

The row of a layer is the handle to drag it by, the index of it, the type and
the options of the cfg section of it and the `-` button removing the layer.
The `⚠ 2 wrong` beside the options tells the wrong ones, the first of them in
it's tooltip. The arrow of the row shows the option rows of the layer below
it, the same rows the `[net]` options have.

* The `+` button adds the layer of the type of the drop down beside it at the
  bottom of the layers, of the common options of the type (the 3 x 3
  convolution of the 32 filters of the batch normalization and the leaky
  activation, for example), and shows it's options.
* The `-` button of a row removes the layer.
* The handle of a row dragged onto the row of another layer moves the layer
  into the place of that one, the layers between them shift.

The layers before and after the removed and the moved ones get the new
indexes, so the references of the `[route]` and the `[shortcut]` layers (and
of the `[scale_channels]` and the `[sam]` ones) follow the layers they refer to:
the relative ones stay relative, the absolute ones absolute, while the
reference to a removed layer stays as it was and the relative one turned
forward becomes the absolute index of the layer after it, which the
description tells wrong.

### Saving the network

The `Save` button saves the network into the file it was opened of, the new
network asks for the file to save into first (the `Save as…` file chooser of
the dxxwjz3p projects, the `.dxxwjz3p` extension added to the name of none).
The `Save as…` one saves into another file, which is the file of the network
then.

The save refuses the network of the wrong options (the status line lists
them) and the network the darknet builds not or the training can't run (the
`[route]` layer of the inputs of the different sizes, for example), the file
stays as it was then. The saved file keeps the annotations, the training
record and the weights of the file it was opened of, but the weights of the
layers the edit changed: so the project of the edited `[net]` options alone
(the longer training of more `max_batches`, for example) goes on training of
it's weights, while the project of the edited head (one class more, for
example) is the network to train of the weights of the layers before the
edit, the transfer learning one. See the
[Saving the edited network](/doc/sections/en_US/4-7-4-network-weights-files.md#saving-the-edited-network)
section.

The window closed, the `New` and the `Open…` buttons pressed with the changes
of no save ask what to do with them first:

| Button | What it does |
| --- | --- |
| `Save and exit` (`Save as… and exit` of the new network) | saves the network the way the `Save` button does, then exits (starts the new one, opens another one); the failed or the canceled save keeps the window |
| `Cancel` | keeps the window and the changes |
| `Discard the changes` | exits (starts the new one, opens another one) of no save |

### The icons

The controls show the icons of the
[Font Awesome Free](https://fontawesome.com/) the binaries embed (the GResource
of the GTK one, the Qt resources of the Qt one), the way the `appFontAwesome`
branch of the [cpp-app-template](https://github.com/yuriysydor1991/cpp-app-template/tree/appFontAwesome)
does: the configure checks out the whole icon set of the release tag (a
shallow clone of about 155 MB), keeps it intact and embeds the few icons of
the controls alone. The GTK one draws them as the symbolic icons of the theme
colors, the Qt one in the color of the text. The icons carry their license,
the CC BY 4.0, which is installed as the
`share/doc/darknetxx/fontawesome-LICENSE.txt`.

| CMake option | Default | Effect |
| --- | --- | --- |
| `ENABLE_FONTAWESOME` | `ON` | Embeds the Font Awesome icons. `OFF` leaves the icons of the controls to the icon theme of the desktop (the freedesktop `document-save` and the like), no download. |
| `DARKNETXX_FONTAWESOME_DIR` | empty | The Font Awesome checkout to reuse instead of the download (an offline build, a build of more build directories). |
| `DARKNETXX_FONTAWESOME_GIT`, `DARKNETXX_FONTAWESOME_GIT_TAG` | the upstream repository, `7.3.1` | The repository and the release tag of the download. |

```
# inside the project root directory

git clone --depth 1 --branch 7.3.1 https://github.com/FortAwesome/Font-Awesome.git /opt/Font-Awesome
cmake -B build -S . -DDARKNETXX_FONTAWESOME_DIR=/opt/Font-Awesome
```

### Building them

The GTK 4 binary is built with the rest of the GUI binaries of the
`ENABLE_DARKNETXX_GUI` option (the GTKmm 4), the Qt 6 one as soon as the Qt 6
Widgets and Svg development files are found (the `qt6-base-dev` and the
`qt6-svg-dev` packages of the Debian and the Ubuntu, the `qt6-base` and the
`qt6-svg` ones of the FreeBSD).

| CMake option | Default | Effect |
| --- | --- | --- |
| `ENABLE_DARKNETXX_NET_EDITOR` | `ON` | Builds the network editor binaries. Set it to `OFF` to build none of them. |
| `ENABLE_DARKNETXX_NET_EDITOR_QT` | `ON` of the Qt 6 found | Builds the Qt 6 one. `ON` requires the Qt 6, `OFF` builds the GTK one alone. |
| `DARKNETXX_NET_EDITOR_ONLY` | `OFF` | Builds the network editor binaries alone (and the library they save the networks by): no `darknetxx`, `darknetxxConverter`, `darknetxx-train-gui` and `darknetxx-recognition-gui` binaries. |
| `DARKNETXX_NET_EDITOR_GUI_BINARY_NAME` | `<project binary name>-net-editor-gui` | The name of the GTK 4 binary and of it's CMake target. |
| `DARKNETXX_NET_EDITOR_QT_GUI_BINARY_NAME` | `<project binary name>-net-editor-qt-gui` | The name of the Qt 6 binary and of it's CMake target. |

```
# inside the project root directory

# the network editors alone
cmake -B build -S . -DDARKNETXX_NET_EDITOR_ONLY=ON

# no network editor
cmake -B build -S . -DENABLE_DARKNETXX_NET_EDITOR=OFF

# the Qt 6 network editor of no GTKmm 4 GUI binary
cmake -B build -S . -DENABLE_DARKNETXX_GUI=OFF
```

The install step installs their desktop entries and icons, named after their
application ids, the `ua.org.kytok.template.darknetxx.neteditor` one of the
GTK binary and the `ua.org.kytok.template.darknetxx.neteditorqt` one of the
Qt binary. The DEB, the RPM and the FreeBSD pkg packages carry both, the WIX
MSI one creates their shortcuts, the snap and the flatpak ones the GTK one
alone (their GNOME runtime brings no Qt 6; the flatpak one shows the icons of
it's theme, since it's build fetches nothing).

### How they are built of the project components

The binaries are the main one with the `src/gui` component, the way the rest
of the GUI binaries are (see the
[src/darknetxx-gui-targets.cmake](/src/darknetxx-gui-targets.cmake) and the
[cmake/darknetxx-gui-binaries.cmake](/cmake/darknetxx-gui-binaries.cmake) of
the GUI binaries of the build): their `app::NetEditorGuiApplication` and
`app::NetEditorQtGuiApplication` classes show the windows of the
`darknetxxGtkmm4::NetEditorGtkmmIniter` and the
`darknetxxQt6::NetEditorQtIniter`, the `DARKNETXX_NET_EDITOR_GUI` and the
`DARKNETXX_NET_EDITOR_QT_GUI` definitions tell the `ApplicationFactory` which
one to create.

Both windows are the views of the model of no toolkit of the
[`src/gui/net-editor`](/src/gui/net-editor) directory (the `net_editor`
namespace):

- **`NetworkSection`**, **`NetworkModel`** - the cfg sections of their
  `key=value` options in their order (read the darknet way: no white space, no
  comments) and the network of the `[net]` one, the layers and the class
  names, which adds, removes and moves the layers; the
  **`LayerReferences`** rewrites the references of the layers moved.
- **`AOption`** - an option the editor knows: the key, the default, what it
  does, the values it takes in the **`OptionContext`** (the network and the
  section of it) and the reason the value is wrong. The **`TextOption`**, the
  **`ChoiceOption`** and the **`NumberOption`** of the integers or the real
  numbers of a range, single or listed, are the kinds of them, the
  descendants of the last one the rules of the table above: the
  **`DivisorOption`**, the **`BoundedOption`**, the **`StepsOption`**, the
  **`ScalesOption`**, the **`InputSizeOption`**, the **`HeadFiltersOption`**,
  the **`HeadClassesOption`**, the **`AnchorsOption`** and the
  **`LayerIndexesOption`**.
- **`SectionSchema`**, **`SectionSchemas`** - the options of the `[net]`
  section and of every layer type the darknet parser makes, the ones the rows
  show always and the further ones the section of them shows, and the options
  of the new layer of the type.
- **`OptionPresenter`** - the row of an option the windows of both toolkits
  show: the value, the values listed, the input filter, the default, the
  description and the error; the **`NetworkValidator`** names the wrong
  options of the network.
- **`NetworkProject`** - the file of the network: the new network, the open,
  the save by the `LibraryFacade::network_description()` and the
  `LibraryFacade::save_network()` of the library (see the
  [The darknetxx library](/doc/sections/en_US/4-7-5-the-darknetxx-library.md)
  section) and the changes of no save.
- **`EditorIcon`**, **`EditorIcons`** - the Font Awesome and the icon theme
  icons of the controls.

The GTK window of the [`src/gui/gtkmm4/net-editor-window`](/src/gui/gtkmm4/net-editor-window)
directory is the `darknetxxGtkmm4::NetEditorWindowContext` of the
`NetEditorWindow.ui` (loaded by the `NetEditorWindowLoader`), of the
`NetworkEventsHandler` of the views of the network (the `OptionsGrid` of the
`OptionRow`s, the `LayerRow`s of the `Gtk::DragSource` of their handles and
the `Gtk::DropTarget` of the rows) and of the `ProjectEventsHandler` of the
file of it, the `GtkmmIcons` of the icons. The Qt window of the
[`src/gui/qt6`](/src/gui/qt6) directory is the
`darknetxxQt6::NetEditorMainWindow` of the `QtOptionsGrid` of the
`QtOptionRow`s (the `OptionValidator` input filter of their line edits) and of
the `LayerRowWidget`s of the `DragHandle`s, the `QtIcons` of the icons.

The `CTEST_NetEditorModel` and the `CTEST_NetworkProject` tests check the
model, the `CTEST_NetEditorWindow` one the GTK window (on the display of the
`GDK_BACKEND`, the Broadway one of no X11 and Wayland, it skips itself of
none) and the `CTEST_NetEditorQtWindow` one the Qt window (on the
`offscreen` platform of the Qt unless the `QT_QPA_PLATFORM` tells another
one), the `CTEST_NetworkRewriter` one the save of the library.
