# Network weights files

The `darknetxx` reads and writes the weights of the networks in two formats:

- the **original darknet weights file** (`*.weights`), the binary file the original Darknet reads and writes, which the training and the detection of the `darknetxx` take (the `--weights` command line parameter) and the training saves its backups into;
- the **dxxwjz1 file** (`*.dxxwjz1`, the **d**arknet**xx** **w**eights **j**son **z**ipped file of the first version), the gzip compressed JSON document of the same weights, readable by any JSON tool and by a human.

The two formats convert into each other with no loss: the original weights file converted to the dxxwjz1 one and back is the same file byte to byte (see the [Converting the network weights files](/doc/sections/en_US/6-2-4-converting-the-network-weights-files.md) section and the [The darknetxxConverter binary](/doc/sections/en_US/5-35-the-darknetxxConverter-binary.md) one).

The components of the files live in the `darknet_adaptor::loaders` namespace under the [`src/darknet-adaptor/adaptors/NetworksLoaders`](/src/darknet-adaptor/adaptors/NetworksLoaders) directory:

| Directory | Holds |
| --- | --- |
| [`NetworksLoaders`](/src/darknet-adaptor/adaptors/NetworksLoaders) | the `IWeightsReader` and the `IWeightsWriter` interfaces every format implements |
| [`weights-model`](/src/darknet-adaptor/adaptors/NetworksLoaders/weights-model) | the format neutral model of the weights: the arrays of every layer kind and their order (`WeightsSchema`), the arrays of a network in that order (`WeightsLayout`), the content of a file (`WeightsDocument`) and its validation (`WeightsDocumentValidator`, `WeightsReport`) |
| [`original-weights`](/src/darknet-adaptor/adaptors/NetworksLoaders/original-weights) | the original weights file: the `OrigWeightsReader`, the `OrigWeightsWriter` and the `OrigWeightsValidator` |
| [`dxxwjz1/v1`](/src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1) | the dxxwjz1 file of the first version: the `Dxxwjz1Reader`, the `Dxxwjz1Writer` and the `Dxxwjz1Validator` with the support classes of theirs (the `dxxwjz1::v1` namespace) |
| [`weights-converter`](/src/darknet-adaptor/adaptors/NetworksLoaders/weights-converter) | the `WeightsConverter` of the files between the formats |

The standalone source of the diagram below is
[`doc/diagrams/network-weights-files-classes.puml`](/doc/diagrams/network-weights-files-classes.puml).

@startuml
hide empty members

interface IWeightsReader {
  +load(net, filename, cutoff) : bool
  +load(net, filename) : bool
  +read(filename, net, document) : bool
}

interface IWeightsWriter {
  +save(net, filename, cutoff, save_ema) : bool
  +save(net, filename) : bool
  +write(document, filename) : bool
}

package "weights-model" {
  class WeightsSchema {
    +{static} name(kind / sub / type) : string
    +{static} parse(name, kind / sub / type) : bool
    +{static} sublayers(type) : subs
    +{static} arrays(type) : kinds
    +{static} rank(type, sub, kind) : int
  }

  class WeightsLayout {
    +{static} of(net, options) : WeightsLayoutEntries
    +{static} floats(layout) : uint64
    +{static} transpose_matrix(a, rows, cols)
  }

  class WeightsDocument {
    +header : WeightsHeader
    +layers : WeightsLayer[]
    +trailing : bytes
    +normalize()
    +file_size() : uint64
  }

  class WeightsDocumentValidator {
    +{static} validate(document) : WeightsReport
    +{static} validate(document, net) : WeightsReport
  }

  class WeightsReport {
    +errors() / warnings()
    +ok() : bool
    +log(subject)
  }
}

package "original-weights" {
  class OrigWeightsReader
  class OrigWeightsWriter
  class OrigWeightsValidator {
    +{static} inspect(filename, net, inspection) : bool
    +{static} validate(filename, net) : WeightsReport
    +{static} validate(document, net) : WeightsReport
  }
}

package "dxxwjz1::v1" {
  class Dxxwjz1Reader {
    +{static} apply(document, net, cutoff) : int
  }
  class Dxxwjz1Writer {
    +Dxxwjz1Writer(level)
  }
  class Dxxwjz1Validator {
    +{static} validate(filename, net) : WeightsReport
  }
  class Dxxwjz1Parser {
    +{static} parse(json, document, report) : bool
    +{static} parse_file(filename, document, report) : bool
  }
  class Dxxwjz1JsonEmitter {
    +begin(header)
    +layer(index, type, arrays)
    +end(trailing)
  }
  class Dxxwjz1FloatCodec {
    +{static} encode(value) : string
    +{static} decode_number(text, value) : bool
    +{static} decode_bits(text, value) : bool
  }
  class Dxxwjz1Base64
  class Dxxwjz1Format <<constants>>
}

package "weights-converter" {
  class WeightsConverter {
    +convert(conversion) : bool
    +{static} parse_format(name, format) : bool
    #create_reader(format) : IWeightsReaderPtr
    #create_writer(format) : IWeightsWriterPtr
    #create_network(cfg) : ANetworkAdaptorPtr
  }
}

package "zlibi" {
  class ZlibController
  class "GzipOStream / GzipIStream" as GzipStreams
}

IWeightsReader <|.. OrigWeightsReader
IWeightsReader <|.. Dxxwjz1Reader
IWeightsWriter <|.. OrigWeightsWriter
IWeightsWriter <|.. Dxxwjz1Writer

OrigWeightsReader ..> OrigWeightsValidator : splits the file by
OrigWeightsReader ..> WeightsLayout
OrigWeightsValidator ..> WeightsLayout : the saved and the loaded arrays
OrigWeightsWriter ..> WeightsDocumentValidator

Dxxwjz1Reader ..> Dxxwjz1Validator
Dxxwjz1Validator ..> Dxxwjz1Parser
Dxxwjz1Validator ..> WeightsDocumentValidator
Dxxwjz1Parser ..> Dxxwjz1FloatCodec
Dxxwjz1Parser ..> Dxxwjz1Base64
Dxxwjz1Parser ..> GzipStreams : reads through
Dxxwjz1Writer ..> Dxxwjz1JsonEmitter
Dxxwjz1Writer ..> ZlibController : writes through
Dxxwjz1JsonEmitter ..> Dxxwjz1FloatCodec
Dxxwjz1Reader ..> WeightsLayout : applies by

WeightsDocumentValidator ..> WeightsLayout
WeightsDocumentValidator ..> WeightsSchema
WeightsLayout ..> WeightsSchema
WeightsDocument ..> WeightsSchema : the file order

WeightsConverter ..> IWeightsReader : reads the source document
WeightsConverter ..> IWeightsWriter : writes the destination
WeightsConverter ..> "OrigDefaultLoader" : parses the network cfg
"app::converter::ApplicationFormatsConverter" ..> WeightsConverter : converts the command line
@enduml

## The readers and the writers

Every format implements the two interfaces:

- **`IWeightsReader`** ([`IWeightsReader.h`](/src/darknet-adaptor/adaptors/NetworksLoaders/IWeightsReader.h)) - the `load(net, filename, cutoff)` method loads the weights of the layers before the cutoff into the network arrays (the `load_weights_upto()` of the original Darknet), the `read(filename, net, document)` one reads the whole file into the format neutral `WeightsDocument`.
- **`IWeightsWriter`** ([`IWeightsWriter.h`](/src/darknet-adaptor/adaptors/NetworksLoaders/IWeightsWriter.h)) - the `save(net, filename, cutoff, save_ema)` method saves the weights of the layers before the cutoff (the `save_weights_upto()` of the original Darknet: the current version, the seen of the current iteration, the EMA arrays of the convolutional layers for the `save_ema`), the `write(document, filename)` one writes the document.

The readers and the writers of both formats load and save the same arrays of the network the same way, so either file of a network loads the same network. The `read()` and the `write()` of the documents are what converts the files between the formats.

## The original darknet weights file

The file is the int32 `major`, `minor` and `revision` version triple, the count of the images the network has seen (the uint64 `seen` of the `(major * 10 + minor) >= 2` versions, the uint32 one of the older versions), followed by the float arrays of the layers with the weights one after another, in the order of the layers and of the table below. The file keeps no structure of its own: which float belongs to which array is told by the network alone.

- **`OrigWeightsReader`** ([`OrigWeightsReader.h`](/src/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsReader.h)) - the C++ port of the original `load_weights()`, `load_weights_upto()` and the per layer routines, which reads exactly the bytes the original did: it skips the `dontload` layers and the batch normalization arrays of the `dontloadscales` ones, transposes the weights of the `flipped` convolutional layers and the connected weights of the files of the major or minor version above 1000, fills the arrays of a truncated file with the whole floats it holds and stops after the layer reading past the end of the file. Its `read()` splits the file into the arrays of the network the file is of (the network is required).
- **`OrigWeightsWriter`** ([`OrigWeightsWriter.h`](/src/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsWriter.h)) - the C++ port of the original `save_weights()`, `save_weights_upto()` and the per layer routines, byte to byte compatible with the original files. Its `write()` writes the header, every array of the document and its trailing bytes, which makes the file the document was read of.
- **`OrigWeightsValidator`** ([`OrigWeightsValidator.h`](/src/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsValidator.h)) - compares the size of the file with the arrays of the network: the arrays the `save_weights()` writes (the file darknet saves) and the ones the `load_weights()` reads, which leave out the `dontload` and the `dontloadscales` arrays (the file made for the load of such a network). The file of the size of neither is truncated or holds extra bytes, which it reports as the warnings, while the gzip compressed file (a dxxwjz1 one given as the original one) and the file shorter than the header are the errors. Its `validate(document, net)` checks a document to be written as the original weights file of the network: the file keeps no names of its arrays, so the arrays of the document have to be the network ones from the first one on, all of them complete but the last one (the truncated file). The document which misses an array in the middle or holds a shorter array followed by others (a dxxwjz1 file edited by hand, for example) is the error, since the loaders would read the arrays after the gap into the wrong ones.

## The dxxwjz1 file format of the first version

The dxxwjz1 file is the gzip compressed ([RFC 1952](https://www.rfc-editor.org/rfc/rfc1952)) UTF-8 [JSON](https://www.rfc-editor.org/rfc/rfc8259) document of the network weights. The stock gzip tools unpack it (`gzip -dc model.dxxwjz1`), while a file of the plain JSON text is read as well, with a warning. The document of a network of a convolutional layer, a maxpool one and a recurrent one:

```
{
  "format": "dxxwjz1",
  "version": 1,
  "generator": "darknetxx 0.11.0",
  "header": {"major": 0, "minor": 2, "revision": 5, "seen": 64},
  "layers": [
    {"index": 0, "type": "convolutional", "arrays": {
      "biases": [0.5,-0.25],
      "scales": [1,1],
      "rolling_mean": [0.0078125,-0.125],
      "rolling_variance": [0.75,0.5],
      "weights": [0.125,"0x7fc00000",-0.0,1e-45,2.5e-05,-3.4028235e+38]
    }},
    {"index": 1, "type": "maxpool"},
    {"index": 2, "type": "rnn", "sublayers": {
      "input_layer": {"arrays": {
        "biases": [0.1],
        "weights": [0.2,0.3]
      }},
      "self_layer": {"arrays": {
        "biases": [0.4],
        "weights": [0.5]
      }},
      "output_layer": {"arrays": {
        "biases": [0.6],
        "weights": [0.7]
      }}
    }}
  ],
  "trailing": "AAE="
}
```

The keys of the document:

| Key | Value | Required |
| --- | --- | --- |
| `format` | the `"dxxwjz1"` string | yes |
| `version` | the `1` integer, the readers refuse the other versions | yes |
| `generator` | the program which wrote the file, informational | no |
| `header` | the header of the original weights file: the `major`, the `minor` and the `revision` int32 numbers and the `seen` uint64 one, the 32 bits one for the `(major * 10 + minor) < 2` versions | yes |
| `layers` | the layer objects in the ascending order of their indexes | yes |
| `trailing` | the base64 text ([RFC 4648](https://www.rfc-editor.org/rfc/rfc4648)) of the bytes the original weights file keeps after the arrays, which the loaders ignore | no |

The keys of a layer object are the `index` (the index of the layer in the network, unique), the `type` (the name of the cfg section of the layer without the brackets: `convolutional`, `maxpool`, `yolo` and the rest; `reorg3d` and `reorg` name the two reorg layer types, `activation` the `[activation]` one) and, for the layers with the weights, the `arrays` object of the arrays of the layer or the `sublayers` object of the sub-layers of the recurrent layers, each with its own `arrays` object. The arrays and the sub-layers are named after the fields of the darknet layers:

| Layer type | Sub-layers | Arrays |
| --- | --- | --- |
| `convolutional` | | `biases`, `scales`, `rolling_mean`, `rolling_variance`, `weights` |
| `connected` | | `biases`, `weights`, `scales`, `rolling_mean`, `rolling_variance` |
| `batchnorm` | | `biases`, `scales`, `rolling_mean`, `rolling_variance` |
| `shortcut`, `implicit` | | `weights` |
| `local` | | `biases`, `weights` |
| `rnn`, `crnn` | `input_layer`, `self_layer`, `output_layer` | the ones of the `connected` (`rnn`) or of the `convolutional` (`crnn`) layer |
| `gru` | `input_z_layer`, `input_r_layer`, `input_h_layer`, `state_z_layer`, `state_r_layer`, `state_h_layer` | the ones of the `connected` layer |
| `lstm` | `wf`, `wi`, `wg`, `wo`, `uf`, `ui`, `ug`, `uo` | the ones of the `connected` layer |
| `conv_lstm` | `vf`, `vi`, `vo` (the peephole layers alone), `wf`, `wi`, `wg`, `wo` (no `wi`, `wg` and `wo` of the bottleneck layers), `uf`, `ui`, `ug`, `uo` | the ones of the `convolutional` layer |

The order of the table is the order of the arrays in the original weights file. The batch normalization arrays (the `scales`, the `rolling_mean` and the `rolling_variance`) belong to the layers of the `batch_normalize=1` option, the convolutional layers sharing the weights of another layer keep no arrays, the shortcut layers keep the `weights` of the `weights_type` option alone. The writers write the arrays in that order and the readers take the keys of any order.

The values of an array are:

- the JSON numbers of the finite floats: the shortest decimal text the float round trips of (`0.1`, `-2.5e-05`, `1e-45` of the smallest denormal), the `-0.0` of the negative zero (the `-0` is the integer zero of the JSON). The readers take any JSON number of the float range and read the float nearest to it;
- the JSON strings of the `"0x"` and the 8 hexadecimal digits of the bit pattern of the float for the infinities and the NaNs (`"0x7f800000"`, `"0xffc12345"`), which keeps the sign and the payload of the NaNs. The readers take the bit pattern string of any float.

So every float of the file is kept bit by bit.

The readers load the file the way the original weights file loads, the header version included: the `seen` and the current iteration of the network, the transposed connected weights of the major or minor version above 1000, no `dontload` layers, no batch normalization arrays of the `dontloadscales` layers, the `flipped` convolutional weights transposed. Unlike the positions of the original file the arrays are found by their layers and their names, so the skipped arrays of one layer shift no arrays of the others. An array shorter than the network one fills its beginning and an absent array keeps the network values (the document of a truncated file, of a file saved up to a layer), which the validator reports as the warnings. The readers skip the unknown keys with a warning, so the later versions may add keys the first one ignores.

The support classes of the [`dxxwjz1/v1`](/src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1) directory:

- **`Dxxwjz1Format`** - the constants of the format: the name, the version, the file name extension and the JSON keys.
- **`Dxxwjz1FloatCodec`** - the JSON text of the floats described above (the C++17 `std::to_chars()` and `std::from_chars()`, independent of the locale).
- **`Dxxwjz1Base64`** - the base64 text of the trailing bytes.
- **`Dxxwjz1JsonEmitter`** - writes the JSON text of the document into a stream a layer at a time, a layer on a line and an array on a line, straight from the network arrays or the document ones.
- **`Dxxwjz1Parser`** - reads the JSON text into the document by the [nlohmann json](https://github.com/nlohmann/json) SAX parser a value at a time, so no JSON tree of the millions of the floats is made, and checks the structure of the document on the way: the JSON syntax, the format and the version, the types of the values, the names of the layer types, of the sub-layers and of the arrays, the repeated keys. The parse runs in the "C" numeric locale of its thread, so a document reads the same whatever locale the application runs in (the comma and the multibyte decimal points included), while the locale of the application and of its other threads is kept.
- **`Dxxwjz1Reader`**, **`Dxxwjz1Writer`** - the reader and the writer (see above). The writer compresses the text into the file as it's written, with the fastest zlib compression level unless another one is given to its constructor: the zlib default level takes about 6 times the time for about 12% smaller files, the best one about 30 times the time for about 14% smaller ones, while the readers take the file of any level.
- **`Dxxwjz1Validator`** - the validator of the files (see below).

The files are compressed and decompressed by the `zlibi::ZlibController` (see the [Supporting components](/doc/sections/en_US/4-7-3-supporting-components.md) section) a chunk at a time, so neither the whole file nor the whole JSON text is kept in the memory. The text takes about 12 bytes of every float and the compressed file about 1.35 times the size of the original weights file (about 1.2 times the size of it recompressed with the best level, `gzip -dc model.dxxwjz1 | gzip -9 > smaller.dxxwjz1`).

## The format neutral model

- **`WeightsSchema`** ([`WeightsSchema.h`](/src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsSchema.h)) - the arrays and the sub-layers of every layer type, their order and their names (the table above).
- **`WeightsLayout`** ([`WeightsLayout.h`](/src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsLayout.h)) - the arrays of a network in the order of the files: the arrays the saves write (the EMA ones of the EMA save) and the ones the loads read with their transpositions, up to a layer.
- **`WeightsDocument`** ([`WeightsDocument.h`](/src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h)) - the content of a weights file: the header, the layers with their arrays and the bytes after the arrays. The arrays in their order are the payload of the original weights file, so the document of the file keeps every byte of it: the truncated arrays, the part of a float the truncated file ends in and the extra bytes included.
- **`WeightsDocumentValidator`** ([`WeightsDocumentValidator.h`](/src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocumentValidator.h)) - validates a document alone and against a network, with the findings in the `WeightsReport`.

The validators report the errors, which keep a file from being loaded or converted, and the warnings about the files, which load, but not the way their authors might have expected:

| Validator | Errors | Warnings |
| --- | --- | --- |
| `OrigWeightsValidator` | the file unable to be read, shorter than the header or gzip compressed; for the document written as the original weights file of the network: the array missing in the middle, the shorter array followed by others, the array of no network layer | the truncated file, the part of a float at the end, the extra bytes, the file made for the load of the `dontload` and the `dontloadscales` layers |
| `Dxxwjz1Validator` | the file unable to be read, the corrupted or the truncated gzip data, the JSON syntax errors, the other format or version, the wrong structure, the documents the `WeightsDocumentValidator` rejects | the plain JSON file, the unknown keys, the ones of the `WeightsDocumentValidator` |
| `WeightsDocumentValidator` | the 32 bits seen of the seen beyond the 32 bits, the repeated layers, the arrays the layers of their types have none; for the network: the layers beyond its layers or of the other types, the arrays it has none, the arrays longer than its ones | the arrays shorter than the network ones, the network arrays the document has none |

## The conversion

The **`WeightsConverter`** ([`WeightsConverter.h`](/src/darknet-adaptor/adaptors/NetworksLoaders/weights-converter/WeightsConverter.h)) converts a file of one format into the other one (or into the same one): the network of the cfg file is parsed (the original weights file requires it, the dxxwjz1 one is validated against it when it's given), the source is validated and read into the document, the document is written into the temporary file of the destination directory, which replaces the destination once complete. A failed conversion leaves the destination as it was and no temporary file behind, and the stop request of the application (a Ctrl+C) stops the conversion between its steps.

The original weights file is split into the arrays the `save_weights()` writes of the network, unless the file is of the size of the arrays the `load_weights()` reads. The original weights file converted to the dxxwjz1 one and back is the same file byte to byte, whatever it holds: the header of any version, the truncated arrays, the extra bytes. The dxxwjz1 file converted into the original weights one with the cfg file is checked by the `OrigWeightsValidator` to hold the arrays of the network one after another (see above), while without the cfg file there is no network to check the document against, so its arrays are written as they are.

## Using the formats

The training and the detection of the `darknetxx` read the original weights files (the `--weights` command line parameter), the dxxwjz1 ones convert to them by the converter. The C++ code loads and saves the weights of a network with either format through the interfaces:

```
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Reader.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Validator.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Writer.h"

using namespace darknet_adaptor::loaders;

// net is the network view of an ANetworkAdaptor (the networkptr its get()
// returns)
dxxwjz1::v1::Dxxwjz1Writer{}.save(*net, "backup.dxxwjz1");
dxxwjz1::v1::Dxxwjz1Reader{}.load(*net, "backup.dxxwjz1");

// the file checked against the network before the load
const WeightsReport report =
    dxxwjz1::v1::Dxxwjz1Validator::validate("backup.dxxwjz1", net.get());
```
