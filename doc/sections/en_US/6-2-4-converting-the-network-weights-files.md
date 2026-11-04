### Converting the network weights files

Beside the training and the detection the executable carries a converter of
the network weights files between the eight formats of the project: the
original darknet weights file (`*.weights`), the dxxwjz1 file (`*.dxxwjz1`,
the gzip compressed JSON document of the weights), the dxxwjz2 one
(`*.dxxwjz2`, the dxxwjz1 document of the network cfg, the class names and the
training record beside the weights), the dxxwjz2p project (`*.dxxwjz2p`,
the dxxwjz2 document of the YOLO4 annotations of the images too), the dxxwjz3
one (`*.dxxwjz3`, the dxxwjz2 document of nothing but the JSON data, the
network cfg of the JSON options of it's sections) and the dxxwjz3p project
(`*.dxxwjz3p`, the dxxwjz2p project of the dxxwjz3 document), the dxxwjz4
one (`*.dxxwjz4`, the dxxwjz3 document of the description text of any length)
and the dxxwjz4p project (`*.dxxwjz4p`, the whole training project in one
file: the ZIP archive of the single JSON document and of the images of the
annotations in it's `data/` folder), and of the YOLO4 training folders into the
dxxwjz2p, the dxxwjz3p and the dxxwjz4p projects and back (see the
[Network weights files](/doc/sections/en_US/4-7-4-network-weights-files.md)
section). The formats and the converter are the ones of the
[darknetxx-datadrivers](https://github.com/yuriysydor1991/darknetxx-datadrivers)
library the project is built against (see the
[Enabling the darknetxx-datadrivers library](/doc/sections/en_US/5-39-enabling-the-darknetxx-datadrivers-library.md)
section), the executable hands them the darknet network of the cfg file the
files of no structure of their own are read by. It is asked for by the four
command line flags, and it starts no training nor detection: the application
tells what it is converting through the log messages alone, so it fits a
script and a build pipeline.

```
# from the build dir

# the original weights file to the dxxwjz1 one
./src/darknetxx \
  --convert-from weights \
  --convert-src ./yolov3.weights \
  --convert-to dxxwjz1 \
  --convert-dst ./yolov3.dxxwjz1 \
  --convert-cfg ./yolov3.cfg

# and back
./src/darknetxx \
  --convert-from dxxwjz1 \
  --convert-src ./yolov3.dxxwjz1 \
  --convert-to weights \
  --convert-dst ./yolov3.weights

# the original weights file to the dxxwjz2 one of it's network and classes
./src/darknetxx \
  --convert-from weights \
  --convert-src ./yolov3.weights \
  --convert-to dxxwjz2 \
  --convert-dst ./yolov3.dxxwjz2 \
  --convert-cfg ./yolov3.cfg \
  --convert-names ./coco.names

# the network to train of the cfg file: the dxxwjz2 file of no weights
./src/darknetxx \
  --convert-to dxxwjz2 \
  --convert-dst ./yolov3.dxxwjz2 \
  --convert-cfg ./yolov3.cfg \
  --convert-names ./coco.names

# the YOLO4 folder to the dxxwjz2p project beside the folder of the images
./src/darknetxx \
  --convert-from yolo4 \
  --convert-src ./cars \
  --convert-to dxxwjz2p \
  --convert-dst ./cars.dxxwjz2p

# the weights file of the inference of the project, of no annotations
./src/darknetxx \
  --convert-from dxxwjz2p \
  --convert-src ./cars.dxxwjz2p \
  --convert-to dxxwjz2 \
  --convert-dst ./cars.dxxwjz2

# the project to the new YOLO4 folder of the copies of the images
./src/darknetxx \
  --convert-from dxxwjz2p \
  --convert-src ./cars.dxxwjz2p \
  --convert-to yolo4 \
  --convert-dst ./cars-yolo4
```

The darknetxx-datadrivers library installs the `darknetxxConverter` binary
beside itself, which carries this very conversion and no training nor
detection at all - the same flags, the same formats, the same behaviour, so
every command of this section runs with the `darknetxxConverter` in place of
the `./src/darknetxx` as well. See [The darknetxxConverter
binary](/doc/sections/en_US/5-35-the-darknetxxConverter-binary.md).

| Option | Value | Description |
| --- | --- | --- |
| `--convert-from` | `<FORMAT>` | the network weights file format to read |
| `--convert-src` | `<FILE>` | the network weights file to read, the folder of the `yolo4` format |
| `--convert-to` | `<FORMAT>` | the network weights file format to write |
| `--convert-dst` | `<FILE>` | the network weights file to write, replaced if it exists; the new folder of the `yolo4` format |
| `--convert-cfg` | `<FILE>` | the network cfg file of the weights (or the dxxwjz2 file of the network) |
| `--convert-names` | `<FILE>` | the class names file of the network the dxxwjz2 file keeps |
| `--convert-description` | `<TEXT>` | the description text of any length the dxxwjz4 and the dxxwjz4p files keep, over the one of the source |
| `--dump-cfg` | `<FILE>` | the dxxwjz2, dxxwjz2p, dxxwjz3, dxxwjz3p, dxxwjz4 or dxxwjz4p file the network cfg of which is printed into the standard output, alone (see [below](#printing-the-network-cfg-of-the-file)) |

The four first flags go together, or the `--convert-to`, the `--convert-dst`
and the `--convert-cfg` ones alone make the network to train of the cfg file.
Naming some of them and not the others is an error, as are the `--convert-cfg`,
the `--convert-names` and the `--convert-description` without them and the
conversion flags combined with
the `--train-cfg` or the `--detect-cfg` ones; the help message is printed
instead.

A `FORMAT` is one of the names below:

| `FORMAT` | The file it names |
| --- | --- |
| `weights` | the original darknet weights file |
| `dxxwjz1` | the darknetxx weights JSON zipped file of the first version |
| `dxxwjz2` | the darknetxx weights JSON zipped file of the second version |
| `dxxwjz2p` | the darknetxx weights JSON zipped project file, the dxxwjz2 one of the YOLO4 annotations of the images |
| `dxxwjz3` | the darknetxx weights JSON zipped file of the third version, the dxxwjz2 one of nothing but the JSON data: the network cfg of the JSON options of it's sections |
| `dxxwjz3p` | the darknetxx weights JSON zipped project file of the third version, the dxxwjz2p one of the dxxwjz3 document |
| `dxxwjz4` | the darknetxx weights JSON zipped file of the fourth version, the dxxwjz3 one of the description text of any length |
| `dxxwjz4p` | the darknetxx weights JSON zipped project file of the fourth version, the whole training project in one file: the ZIP archive of the single JSON document of the network, the annotations and the description and of the images of the annotations in it's data folder |
| `yolo4` | the YOLO4 training folder (the conversion alone): the `cfg/*.cfg` network, the `data/obj.names` classes, the image lists and labels and the latest `backup/` weights |

The original weights file keeps no structure of its own - it is the float
arrays of the layers one after another - so reading it requires the network
cfg file the weights are of, the `--convert-cfg` one. The dxxwjz1 and the
dxxwjz2 files keep the layers and the names of their arrays, so they convert
without the cfg file, while the cfg file given is the network the file is
validated against: the layers of the other types, the arrays the network has
none of and the arrays longer than the network ones fail the conversion.

The dxxwjz2 file keeps the network description beside the weights, which the
destination of the dxxwjz2 format takes of the conversion: the text of the
`--convert-cfg` file (over the cfg the dxxwjz2 source keeps - one of them is
required, so the dxxwjz1 file converts to the dxxwjz2 one with the cfg file
alone) and the class names of the `--convert-names` file (over the classes of
the dxxwjz2 source, over the `names=` file of the cfg). The training record of
the dxxwjz2 source - the losses, the mAPs and the time of it's training - goes
on into the dxxwjz2 destination, while the original weights and the dxxwjz1
files keep the weights alone: the dxxwjz2 file converted to them and back keeps
the same weights, the training record is lost.

The dxxwjz3 and the dxxwjz3p files convert the way the dxxwjz2 and the
dxxwjz2p ones do: they keep the same network description, the cfg one of the
JSON of it's sections, which the conversion into the dxxwjz2 file turns back
into the cfg text (of no comments, see the
[The dxxwjz3 file format of the third version](/doc/sections/en_US/4-7-4-network-weights-files.md#the-dxxwjz3-file-format-of-the-third-version)
section).

The dxxwjz4 and the dxxwjz4p files keep the description text beside the
dxxwjz3 and the dxxwjz3p content: the text of the `--convert-description`
(over the description of the source), the comments and the description of the
network and of it's data of any length, the lines, the quotes and the UTF-8
text of any language included. The other formats keep no description, the
conversion into them warns of the description dropped.

```
# the reference network of it's description
./src/darknetxx \
  --convert-from dxxwjz3 \
  --convert-src ./yolov4.dxxwjz3 \
  --convert-to dxxwjz4 \
  --convert-dst ./yolov4.dxxwjz4 \
  --convert-description "$(cat ./yolov4-notes.txt)"
```

The dxxwjz4p project keeps the images themselves too: the conversion into it
packs the copies of the images of the annotations of the source (the
dxxwjz2p, the dxxwjz3p, the dxxwjz4p project or the `yolo4` folder) into the
`data/` folder of the archive beside the single `project.json` document, so
the destination is the whole project, which trains on any machine alone. The
dxxwjz4p source is read of the folder it's unpacked into (the
`darknetxx-dxxwjz4p-<name>-<random>` one of the temporary directory, removed
at the end of the conversion), so it's conversion into the dxxwjz2p or the
dxxwjz3p project copies the images beside the destination first, the
`data/` folder of the archive beside the `cars.dxxwjz3p` file, and refers to
them; an existing image of another content there fails the conversion and is
kept.

```
# the whole training project of the YOLO4 folder in one file
./src/darknetxx \
  --convert-from yolo4 \
  --convert-src ./cars \
  --convert-to dxxwjz4p \
  --convert-dst ./cars.dxxwjz4p \
  --convert-description "The cars of the parking lot cameras, autumn 2026"

# the dxxwjz3p project of the data/ folder beside it
./src/darknetxx \
  --convert-from dxxwjz4p \
  --convert-src ./cars.dxxwjz4p \
  --convert-to dxxwjz3p \
  --convert-dst ./unpacked/cars.dxxwjz3p
```

The dxxwjz2p project keeps the annotations of the images beside the dxxwjz2
content, which the destination of the dxxwjz2p format takes of the dxxwjz2p or
the `yolo4` source: the dxxwjz2p project converted into the dxxwjz2 file is the
weights file of the inference of it, of no annotations. The YOLO4 folder
source is read the way the training reads it: the first `cfg/*.cfg` network,
the `data/obj.names` classes, the images of the `train=` and the `valid=` lists
of the cfg with their sizes and the boxes of their label files, the latest
weights of the backup folder (of the network of the folder cfg for the
original weights file). The YOLO4 folder destination is the new folder of the
`cfg/<name>.cfg` network (of the name of the folder up to its first dot), the
`data/obj.names`, `data/obj.data`, `data/train.txt` and `data/val.txt` files,
the copies of the images in the `data/obj/` folder with their labels and the
`backup/<name>_last.dxxwjz3` weights; the existing folder of files is no
destination, the conversion into it fails and keeps it.

The conversion of no source makes the network to train: the dxxwjz2 file of
the network description of the `--convert-cfg` file and of the class names
alone, of no weights, which the training of the file alone trains from scratch
(see the [Network weights files](/doc/sections/en_US/4-7-4-network-weights-files.md)
section). Such file converts into the dxxwjz2 one alone: the original weights
and the dxxwjz1 files of it would keep nothing but a header, so the conversion
fails.

The conversion into the original weights file checks one thing more: the file
keeps no names of its arrays, so a dxxwjz1 document which misses an array in
the middle or holds a shorter array followed by others (a file edited by hand,
for example) fails it, since the arrays after the gap would load into the
wrong arrays of the network. Without the cfg file there is no network to check
the document against and its arrays are written as they are.

Every conversion keeps every byte of the weights: the original weights file
converted to the dxxwjz1 or the dxxwjz2 one and back is the same file,
whatever it holds -
the header of any version (the 32 bits seen of the old versions, the
transposed connected weights of the versions above 1000), the truncated
arrays, the extra bytes after the arrays. A file of either format converts
into the same format as well, which validates it and makes the canonical file
of it.

The problems of the source are reported: the errors (a file unable to be read,
the corrupted or the truncated gzip data, a JSON document of the wrong
structure, a file of another network) fail the conversion, while the warnings
(the truncated file, the extra bytes, the unknown keys of the JSON document)
are logged and the conversion goes on. The destination is written into a
temporary file of its directory - the directory is made if needed - which
replaces the destination once complete, so a failed conversion writes nothing
into the destination.

The exit status is zero when the conversion succeeded, and non zero otherwise:
for an unknown format name, an unreadable or invalid source, a destination that
can not be written. A stop signal (a Ctrl+C) stops the conversion between its
steps with the `128 + signal number` exit status, the one the shell reports for
a process the signal killed.

#### Printing the network cfg of the file

The dxxwjz2, the dxxwjz2p, the dxxwjz3, the dxxwjz3p, the dxxwjz4 and the
dxxwjz4p files keep the network cfg beside the weights, which the `--dump-cfg` flag prints into the standard
output as the darknet (YOLO4) cfg text, so the network of the file is
examined, compared with another one or trained anew with no conversion:

```
# from the build dir

# the network cfg the dxxwjz3 file keeps
./src/darknetxx --dump-cfg ./yolov4.dxxwjz3

# into the cfg file of the very same network, by the binary of the library
darknetxxConverter --dump-cfg ./yolov4.dxxwjz3 > ./yolov4.cfg
```

The standard output gets the cfg alone - the log messages of the errors go into
the standard error output - so the output redirected into a file is the cfg
file. The dxxwjz2 file and project print the very cfg text they keep, the
comments included, while the dxxwjz3 ones print the cfg of the JSON options of
their sections: a section header or an option on a line, the empty line
between the sections, no comments and no repeated keys (see the
[The dxxwjz3 file format of the third version](/doc/sections/en_US/4-7-4-network-weights-files.md#the-dxxwjz3-file-format-of-the-third-version)
section). Either cfg makes the very same network: the dxxwjz3 file of the
printed cfg and of the weights of the file is the same file. The class names
the files keep go into the `data/obj.names` file of the conversion into the
`yolo4` folder.

The flag goes alone: it's combination with the conversion flags, the
`--train-cfg`, the `--detect-cfg` or the `--weights` one is an error. The
original weights and the dxxwjz1 files keep no network cfg, so their print, as
the print of a file unable to be read, prints nothing and ends with a non zero
exit status. The `darknetxx` binary, the GUI ones and the `darknetxxConverter`
one of the library print the cfg the very same way.
