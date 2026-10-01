### Converting the network weights files

Beside the training and the detection the executable carries a converter of
the network weights files between the two formats of the project: the
original darknet weights file (`*.weights`) and the dxxwjz1 file (`*.dxxwjz1`,
the gzip compressed JSON document of the weights, see the
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
| `--convert-cfg` | `<FILE>` | the network cfg file of the weights |

The four first flags go together. Naming some of them and not the others is an
error, as is the `--convert-cfg` without them and the conversion flags combined
with the `--train-cfg` or the `--detect-cfg` ones; the help message is printed
instead.

A `FORMAT` is one of the names below:

| `FORMAT` | The file it names |
| --- | --- |
| `weights` | the original darknet weights file |
| `dxxwjz1` | the darknetxx weights JSON zipped file of the first version |

The original weights file keeps no structure of its own - it is the float
arrays of the layers one after another - so reading it requires the network
cfg file the weights are of, the `--convert-cfg` one. The dxxwjz1 file keeps
the layers and the names of its arrays, so it converts without the cfg file,
while the cfg file given is the network the file is validated against: the
layers of the other types, the arrays the network has none of and the arrays
longer than the network ones fail the conversion.

The conversion into the original weights file checks one thing more: the file
keeps no names of its arrays, so a dxxwjz1 document which misses an array in
the middle or holds a shorter array followed by others (a file edited by hand,
for example) fails it, since the arrays after the gap would load into the
wrong arrays of the network. Without the cfg file there is no network to check
the document against and its arrays are written as they are.

Every conversion keeps every byte of the file: the original weights file
converted to the dxxwjz1 one and back is the same file, whatever it holds -
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
