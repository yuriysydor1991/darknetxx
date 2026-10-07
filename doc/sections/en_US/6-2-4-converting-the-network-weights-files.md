### Converting the network weights files

Beside the training and the detection the executable carries a converter of
the network weights files between the three formats of the project: the
original darknet weights file (`*.weights`), the dxxwjz1 file (`*.dxxwjz1`,
the gzip compressed JSON document of the weights) and the dxxwjz2 one
(`*.dxxwjz2`, the dxxwjz1 document of the network cfg, the class names and the
training record beside the weights, see the
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
```

The project also builds a second binary, `darknetxxConverter`, which carries
this very conversion and no training nor detection at all - the same flags,
the same formats, the same behaviour. See [The darknetxxConverter
binary](/doc/sections/en_US/5-35-the-darknetxxConverter-binary.md).

| Option | Value | Description |
| --- | --- | --- |
| `--convert-from` | `<FORMAT>` | the network weights file format to read |
| `--convert-src` | `<FILE>` | the network weights file to read |
| `--convert-to` | `<FORMAT>` | the network weights file format to write |
| `--convert-dst` | `<FILE>` | the network weights file to write, replaced if it exists |
| `--convert-cfg` | `<FILE>` | the network cfg file of the weights (or the dxxwjz2 file of the network) |
| `--convert-names` | `<FILE>` | the class names file of the network the dxxwjz2 file keeps |

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
