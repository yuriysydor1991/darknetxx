### Converting the network weights files

Beside the training and the detection the executable carries a converter of
the network weights files between the six formats of the project: the
original darknet weights file (`*.weights`), the dxxwjz1 file (`*.dxxwjz1`,
the gzip compressed JSON document of the weights), the dxxwjz2 one
(`*.dxxwjz2`, the dxxwjz1 document of the network cfg, the class names and the
training record beside the weights), the dxxwjz2p project (`*.dxxwjz2p`,
the dxxwjz2 document of the YOLO4 annotations of the images too), the dxxwjz3
one (`*.dxxwjz3`, the dxxwjz2 document of nothing but the JSON data, the
network cfg of the JSON options of it's sections) and the dxxwjz3p project
(`*.dxxwjz3p`, the dxxwjz2p project of the dxxwjz3 document), and of the YOLO4
training folders into the dxxwjz2p and the dxxwjz3p projects and back (see the
[Network weights files](/doc/sections/en_US/4-7-4-network-weights-files.md)
section). It is asked for by the four command line flags, and it starts no
training nor detection: the application tells what it is converting through the
log messages alone, so it fits a script and a build pipeline.

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

The project also builds a second binary, `darknetxxConverter`, which carries
this very conversion and no training nor detection at all - the same flags,
the same formats, the same behaviour. See [The darknetxxConverter
binary](/doc/sections/en_US/5-35-the-darknetxxConverter-binary.md).

| Option | Value | Description |
| --- | --- | --- |
| `--convert-from` | `<FORMAT>` | the network weights file format to read |
| `--convert-src` | `<FILE>` | the network weights file to read, the folder of the `yolo4` format |
| `--convert-to` | `<FORMAT>` | the network weights file format to write |
| `--convert-dst` | `<FILE>` | the network weights file to write, replaced if it exists; the new folder of the `yolo4` format |
| `--convert-cfg` | `<FILE>` | the network cfg file of the weights (or the dxxwjz2 file of the network) |
| `--convert-names` | `<FILE>` | the class names file of the network the dxxwjz2 file keeps |
| `--dump-cfg` | `<FILE>` | the dxxwjz2, dxxwjz2p, dxxwjz3 or dxxwjz3p file the network cfg of which is printed into the standard output, alone (see [below](#printing-the-network-cfg-of-the-file)) |

The four first flags go together, or the `--convert-to`, the `--convert-dst`
and the `--convert-cfg` ones alone make the network to train of the cfg file.
Naming some of them and not the others is an error, as are the `--convert-cfg`
and the `--convert-names` without them and the conversion flags combined with
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

The dxxwjz2, the dxxwjz2p, the dxxwjz3 and the dxxwjz3p files keep the network
cfg beside the weights, which the `--dump-cfg` flag prints into the standard
output as the darknet (YOLO4) cfg text, so the network of the file is
examined, compared with another one or trained anew with no conversion:

```
# from the build dir

# the network cfg the dxxwjz3 file keeps
./src/darknetxxConverter --dump-cfg ./yolov4.dxxwjz3

# into the cfg file of the very same network
./src/darknetxxConverter --dump-cfg ./yolov4.dxxwjz3 > ./yolov4.cfg
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
exit status. The `darknetxx` binary and the GUI ones print the cfg the very
same way.
