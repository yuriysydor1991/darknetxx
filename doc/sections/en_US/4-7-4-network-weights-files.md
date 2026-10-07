# Network weights files

The `darknetxx` reads and writes the weights of the networks in three formats:

- the **original darknet weights file** (`*.weights`), the binary file the original Darknet reads and writes;
- the **dxxwjz1 file** (`*.dxxwjz1`, the **d**arknet**xx** **w**eights **j**son **z**ipped file of the first version), the gzip compressed JSON document of the same weights, readable by any JSON tool and by a human;
- the **dxxwjz2 file** (`*.dxxwjz2`, the file of the second version), the dxxwjz1 document of everything the network is detected and trained with beside its weights: the text of the network cfg file, the names of the classes with their indexes, the loss of every training iteration in the order of the training and the time the training took.

The training and the detection of the `darknetxx` read the weights files of every format (the `--weights` command line parameter, the format of a file is told by its name extension), the training saves its weights into the dxxwjz2 files by default (see the [Training with the formats](#training-with-the-formats) section below) and the dxxwjz2 file alone is the network the detection runs with.

The weights of the formats convert into each other with no loss: the original weights file converted to the dxxwjz1 or the dxxwjz2 one and back is the same file byte to byte (see the [Converting the network weights files](/doc/sections/en_US/6-2-4-converting-the-network-weights-files.md) section and the [The darknetxxConverter binary](/doc/sections/en_US/5-35-the-darknetxxConverter-binary.md) one), while the network description and the training record are kept by the dxxwjz2 files alone.

The components of the files live in the `darknet_adaptor::loaders` namespace under the [`src/lib/darknet-adaptor/adaptors/NetworksLoaders`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders) directory:

| Directory | Holds |
| --- | --- |
| [`NetworksLoaders`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders) | the `IWeightsReader`, the `IWeightsWriter` and the `IWeightsFileFormat` interfaces every format implements |
| [`weights-model`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model) | the format neutral model of the weights: the arrays of every layer kind and their order (`WeightsSchema`), the arrays of a network in that order (`WeightsLayout`), the content of a file (`WeightsDocument` with the `NetworkDescription` and the `TrainingRecord`), the reads of the cfg and the class names files (`NetworkFiles`) and the validation (`WeightsDocumentValidator`, `WeightsReport`) |
| [`original-weights`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/original-weights) | the original weights file: the `OrigWeightsReader`, the `OrigWeightsWriter`, the `OrigWeightsValidator` and the `OrigWeightsFileFormat` |
| [`dxxwjz1/v1`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1) | the dxxwjz1 file of the first version: the `Dxxwjz1Reader`, the `Dxxwjz1Writer`, the `Dxxwjz1Validator` and the `Dxxwjz1FileFormat` with the support classes of theirs (the `dxxwjz1::v1` namespace) |
| [`dxxwjz2/v2`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2) | the dxxwjz2 file of the second version: the `Dxxwjz2Reader`, the `Dxxwjz2Writer`, the `Dxxwjz2Validator` and the `Dxxwjz2FileFormat` with the support classes of theirs (the `dxxwjz2::v2` namespace), which extend the dxxwjz1 ones |
| [`weights-formats`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-formats) | the formats of their names and file name extensions (`WeightsFileFormats`) and the files networks are made of: the cfg and the dxxwjz2 ones (`NetworkSource`) |
| [`weights-converter`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-converter) | the `WeightsConverter` of the files between the formats and the `WeightsConversion` it performs |

The journal of the training, the `TrainingJournal` of the [`TrainingJournal`](/src/lib/darknet-adaptor/adaptors/TrainingJournal) directory, keeps the network description and the training record the weights files of the training keep, the `TrainingChart` of the [`TrainingChart`](/src/lib/darknet-adaptor/adaptors/TrainingChart) directory draws the chart of the record (see the [Training with the formats](#training-with-the-formats) section).

The standalone source of the diagram below is
[`doc/diagrams/network-weights-files-classes.puml`](/doc/diagrams/network-weights-files-classes.puml).

@startuml
hide empty members

interface IWeightsReader {
  +load(net, filename, cutoff) : bool
  +load(net, filename) : bool
  +load(net, filename, cutoff, rest) : bool
  +read(filename, net, document) : bool
  +peek(filename, rest, weights) : bool
}

interface IWeightsWriter {
  +save(net, filename, cutoff, save_ema) : bool
  +save(net, filename) : bool
  +save(net, filename, cutoff, save_ema, rest) : bool
  +write(document, filename) : bool
}

interface IWeightsFileFormat {
  +name() / extension() / description() : string
  +keeps_structure() : bool
  +keeps_network() : bool
  +create_reader() : IWeightsReaderPtr
  +create_writer() : IWeightsWriterPtr
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
    +network : NetworkDescription
    +training : TrainingRecord
    +normalize()
    +file_size() : uint64
  }

  class NetworkDescription {
    +cfg() : string
    +classes() : string[]
    +empty() : bool
  }

  class TrainingRecord {
    +seconds() : double
    +record(iteration, loss, avg_loss)
    +record_map(iteration, map)
    +iterations() / losses() / avg_losses()
    +map_iterations() / maps()
    +truncate_after(iteration)
  }

  class NetworkFiles {
    +{static} read_text(path, text) : bool
    +{static} read_classes(path, classes) : bool
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
  class OrigWeightsFileFormat
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
    #create_emitter(out, rest) : emitter
    #check(rest, filename) : bool
  }
  class Dxxwjz1FileFormat
  class Dxxwjz1Validator {
    +{static} validate(filename, net) : WeightsReport
  }
  class Dxxwjz1Parser {
    +{static} parse(json, document, report) : bool
    +{static} parse_file(filename, document, report) : bool
    +{static} parse_file(filename, factory, document, report) : bool
  }
  class Dxxwjz1SaxHandler {
    +complete(parsed) : bool
    #format_name() / format_version()
    #required_keys() : string[]
    #extension(name) : delegate
    #extension_end(name) : bool
  }
  class Dxxwjz1JsonEmitter {
    +begin(header)
    +layer(index, type, arrays)
    +end(trailing)
    #sections()
  }
  class Dxxwjz1FloatCodec {
    +{static} encode(value) : string
    +{static} decode_number(text, value) : bool
    +{static} decode_bits(text, value) : bool
  }
  class Dxxwjz1JsonString {
    +{static} quote(text) : string
  }
  class Dxxwjz1Base64
  class Dxxwjz1Format <<constants>>
}

package "dxxwjz2::v2" {
  class Dxxwjz2Reader
  class Dxxwjz2Writer
  class Dxxwjz2FileFormat
  class Dxxwjz2Validator {
    +{static} validate(filename, net) : WeightsReport
    +{static} validate(document, net) : WeightsReport
  }
  class Dxxwjz2Parser {
    +{static} parse_file(filename, document, report) : bool
    +{static} peek(filename, rest, weights, report) : bool
  }
  class Dxxwjz2SaxHandler
  class Dxxwjz2SubtreeBuilder
  class Dxxwjz2MetadataReader {
    +network(value, description) : bool
    +training(value, record) : bool
  }
  class Dxxwjz2JsonEmitter
  class Dxxwjz2Format <<constants>>
}

package "weights-formats" {
  class WeightsFileFormats {
    +{static} all() : formats
    +{static} named(name) : IWeightsFileFormatPtr
    +{static} of_file(path) : IWeightsFileFormatPtr
  }
  class NetworkSource {
    +{static} describes(path) : bool
    +{static} read(path, description) : bool
    +{static} base_of(path, iteration) : string
  }
}

package "weights-converter" {
  class WeightsConversion {
    +from / to : IWeightsFileFormatPtr
    +src / dst / cfg / names : string
    +stopped() : bool
  }
  class WeightsConverter {
    +convert(conversion) : bool
    #create_network(cfg) : ANetworkAdaptorPtr
  }
}

class TrainingJournal {
  +record(iteration, loss, avg_loss)
  +record_map(iteration, map)
  +save(net, stem, ema) : bool
  +rest() : WeightsDocument
}

class TrainingChart {
  +{static} render(rest, png) : bool
  +{static} render_file(weights, png) : bool
}

package "zlibi" {
  class ZlibController
  class "GzipOStream / GzipIStream" as GzipStreams
}

IWeightsReader <|.. OrigWeightsReader
IWeightsReader <|.. Dxxwjz1Reader
IWeightsWriter <|.. OrigWeightsWriter
IWeightsWriter <|.. Dxxwjz1Writer
IWeightsFileFormat <|.. OrigWeightsFileFormat
IWeightsFileFormat <|.. Dxxwjz1FileFormat
IWeightsFileFormat <|.. Dxxwjz2FileFormat

Dxxwjz1Reader <|-- Dxxwjz2Reader
Dxxwjz1Writer <|-- Dxxwjz2Writer
Dxxwjz1SaxHandler <|-- Dxxwjz2SaxHandler
Dxxwjz1JsonEmitter <|-- Dxxwjz2JsonEmitter

OrigWeightsReader ..> OrigWeightsValidator : splits the file by
OrigWeightsReader ..> WeightsLayout
OrigWeightsValidator ..> WeightsLayout : the saved and the loaded arrays
OrigWeightsWriter ..> WeightsDocumentValidator

Dxxwjz1Reader ..> Dxxwjz1Validator
Dxxwjz1Validator ..> Dxxwjz1Parser
Dxxwjz1Validator ..> WeightsDocumentValidator
Dxxwjz1Parser ..> Dxxwjz1SaxHandler
Dxxwjz1Parser ..> GzipStreams : reads through
Dxxwjz1SaxHandler ..> Dxxwjz1FloatCodec
Dxxwjz1SaxHandler ..> Dxxwjz1Base64
Dxxwjz1Writer ..> Dxxwjz1JsonEmitter
Dxxwjz1Writer ..> ZlibController : writes through
Dxxwjz1JsonEmitter ..> Dxxwjz1FloatCodec
Dxxwjz1JsonEmitter ..> Dxxwjz1JsonString
Dxxwjz1Reader ..> WeightsLayout : applies by

Dxxwjz2Reader ..> Dxxwjz2Validator
Dxxwjz2Validator ..> Dxxwjz2Parser
Dxxwjz2Parser ..> Dxxwjz1Parser : parses by
Dxxwjz2Parser ..> Dxxwjz2SaxHandler
Dxxwjz2SaxHandler ..> Dxxwjz2SubtreeBuilder : the network and the training trees
Dxxwjz2SaxHandler ..> Dxxwjz2MetadataReader
Dxxwjz2Writer ..> Dxxwjz2JsonEmitter

WeightsDocumentValidator ..> WeightsLayout
WeightsDocumentValidator ..> WeightsSchema
WeightsLayout ..> WeightsSchema
WeightsDocument ..> WeightsSchema : the file order
WeightsDocument *-- NetworkDescription
WeightsDocument *-- TrainingRecord

WeightsFileFormats o-- IWeightsFileFormat
NetworkSource ..> WeightsFileFormats : peeks the dxxwjz2 files by
NetworkSource ..> NetworkFiles : reads the cfg files by

WeightsConverter ..> WeightsConversion
WeightsConverter ..> IWeightsFileFormat : reads the source, writes the destination by
WeightsConverter ..> NetworkSource : the cfg the destination keeps
WeightsConverter ..> "OrigDefaultLoader" : parses the network cfg
TrainingJournal ..> IWeightsFileFormat : saves the training weights by
TrainingChart ..> TrainingRecord : draws the chart of
TrainingChart ..> WeightsFileFormats : peeks the dxxwjz2 files by
"OrigDefaultLoader" ..> NetworkSource : parses the network of
"OrigDefaultLoader" ..> TrainingJournal : starts the journal of the training
"darknetxx_impl::WeightsConversionLib" ..> WeightsConverter : converts the library context
@enduml

## The readers and the writers

Every format implements the three interfaces:

- **`IWeightsReader`** ([`IWeightsReader.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/IWeightsReader.h)) - the `load(net, filename, cutoff)` method loads the weights of the layers before the cutoff into the network arrays (the `load_weights_upto()` of the original Darknet), the `load(net, filename, cutoff, rest)` one keeps the rest of the file as well: the document without the arrays, of the network description and the training record of the formats which keep them. The `read(filename, net, document)` method reads the whole file into the format neutral `WeightsDocument`, the `peek(filename, rest, weights)` one reads the rest of the file up to its first weights array and tells the file holds weights (the dxxwjz2 files alone, the rest of the formats keep no network description to peek).
- **`IWeightsWriter`** ([`IWeightsWriter.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/IWeightsWriter.h)) - the `save(net, filename, cutoff, save_ema)` method saves the weights of the layers before the cutoff (the `save_weights_upto()` of the original Darknet: the current version, the seen of the current iteration, the EMA arrays of the convolutional layers for the `save_ema`), the `save(net, filename, cutoff, save_ema, rest)` one saves the network description and the training record of the rest beside them, which the formats of the weights alone leave out. The `write(document, filename)` method writes the document.
- **`IWeightsFileFormat`** ([`IWeightsFileFormat.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/IWeightsFileFormat.h)) - the format itself: its name (the one of the command line), its file name extension and description, whether its files keep the structure of the network (the layers and the names of the arrays, so they're read without the network) and the network description, the reader and the writer of its files. The `WeightsFileFormats` of the [`weights-formats`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-formats) directory finds the formats of their names and of the file name extensions (any extension but the `.dxxwjz1` and the `.dxxwjz2` ones is the original weights file), the `NetworkSource` reads the network description of the file a network is made of: the text of the cfg file or the description the dxxwjz2 file keeps.

The readers and the writers of the formats load and save the same arrays of the network the same way, so any file of a network loads the same network. The `read()` and the `write()` of the documents are what converts the files between the formats.

## The original darknet weights file

The file is the int32 `major`, `minor` and `revision` version triple, the count of the images the network has seen (the uint64 `seen` of the `(major * 10 + minor) >= 2` versions, the uint32 one of the older versions), followed by the float arrays of the layers with the weights one after another, in the order of the layers and of the table below. The file keeps no structure of its own: which float belongs to which array is told by the network alone.

- **`OrigWeightsReader`** ([`OrigWeightsReader.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsReader.h)) - the C++ port of the original `load_weights()`, `load_weights_upto()` and the per layer routines, which reads exactly the bytes the original did: it skips the `dontload` layers and the batch normalization arrays of the `dontloadscales` ones, transposes the weights of the `flipped` convolutional layers and the connected weights of the files of the major or minor version above 1000, fills the arrays of a truncated file with the whole floats it holds and stops after the layer reading past the end of the file. Its `read()` splits the file into the arrays of the network the file is of (the network is required).
- **`OrigWeightsWriter`** ([`OrigWeightsWriter.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsWriter.h)) - the C++ port of the original `save_weights()`, `save_weights_upto()` and the per layer routines, byte to byte compatible with the original files. Its `write()` writes the header, every array of the document and its trailing bytes, which makes the file the document was read of.
- **`OrigWeightsValidator`** ([`OrigWeightsValidator.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsValidator.h)) - compares the size of the file with the arrays of the network: the arrays the `save_weights()` writes (the file darknet saves) and the ones the `load_weights()` reads, which leave out the `dontload` and the `dontloadscales` arrays (the file made for the load of such a network). The file of the size of neither is truncated or holds extra bytes, which it reports as the warnings, while the gzip compressed file (a dxxwjz1 one given as the original one) and the file shorter than the header are the errors. Its `validate(document, net)` checks a document to be written as the original weights file of the network: the file keeps no names of its arrays, so the arrays of the document have to be the network ones from the first one on, all of them complete but the last one (the truncated file). The document which misses an array in the middle or holds a shorter array followed by others (a dxxwjz1 file edited by hand, for example) is the error, since the loaders would read the arrays after the gap into the wrong ones.

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

The support classes of the [`dxxwjz1/v1`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1) directory:

- **`Dxxwjz1Format`** - the constants of the format: the name, the version, the file name extension and the JSON keys.
- **`Dxxwjz1FloatCodec`** - the JSON text of the floats described above (the C++17 `std::to_chars()` and `std::from_chars()`, independent of the locale).
- **`Dxxwjz1Base64`** - the base64 text of the trailing bytes.
- **`Dxxwjz1JsonString`** - the JSON string text of any text: the bytes which aren't UTF-8 are replaced with the U+FFFD characters.
- **`Dxxwjz1JsonEmitter`** - writes the JSON text of the document into a stream a layer at a time, a layer on a line and an array on a line, straight from the network arrays or the document ones. The emitters of the later versions write their members by its `sections()`.
- **`Dxxwjz1SaxHandler`** - builds the document of the events of the [nlohmann json](https://github.com/nlohmann/json) SAX parser a value at a time, so no JSON tree of the millions of the floats is made, and checks the structure of the document on the way: the JSON syntax, the format and the version, the types of the values, the names of the layer types, of the sub-layers and of the arrays, the repeated keys. The handlers of the later versions tell their own format name, version and required keys and take the values of their own document keys (see the dxxwjz2 format below).
- **`Dxxwjz1Parser`** - reads the JSON text into the document by the handler. The parse runs in the "C" numeric locale of its thread, so a document reads the same whatever locale the application runs in (the comma and the multibyte decimal points included), while the locale of the application and of its other threads is kept.
- **`Dxxwjz1Reader`**, **`Dxxwjz1Writer`** - the reader and the writer (see above). The writer compresses the text into the file as it's written, with the fastest zlib compression level unless another one is given to its constructor: the zlib default level takes about 6 times the time for about 12% smaller files, the best one about 30 times the time for about 14% smaller ones, while the readers take the file of any level.
- **`Dxxwjz1Validator`** - the validator of the files (see below).
- **`Dxxwjz1FileFormat`** - the `IWeightsFileFormat` of the dxxwjz1 files.

The files are compressed and decompressed by the `zlibi::ZlibController` (see the [Supporting components](/doc/sections/en_US/4-7-3-supporting-components.md) section) a chunk at a time, so neither the whole file nor the whole JSON text is kept in the memory. The text takes about 12 bytes of every float and the compressed file about 1.35 times the size of the original weights file (about 1.2 times the size of it recompressed with the best level, `gzip -dc model.dxxwjz1 | gzip -9 > smaller.dxxwjz1`).

## The dxxwjz2 file format of the second version

The dxxwjz2 file is the dxxwjz1 document (see above) of everything the network is detected and trained with: the network description and the training record beside the weights. The document of the network of the two classes trained for three iterations:

```
{
  "format": "dxxwjz2",
  "version": 2,
  "generator": "darknetxx 0.11.0",
  "network": {"cfg": "[net]\nbatch=64\nwidth=416\n...", "classes": [
    {"index": 0, "name": "cat"},
    {"index": 1, "name": "dog"}
  ]},
  "training": {"seconds": 3725,
    "iteration": [1,2,3],
    "loss": [812.5,790.25,701],
    "avg_loss": [812.5,810.275,799.3475],
    "map_iteration": [3],
    "map": [0.0125]
  },
  "header": {"major": 0, "minor": 2, "revision": 5, "seen": 192},
  "layers": [
    {"index": 0, "type": "convolutional", "arrays": {...}},
    ...
  ]
}
```

The keys of the document beside the dxxwjz1 ones:

| Key | Value | Required |
| --- | --- | --- |
| `format` | the `"dxxwjz2"` string | yes |
| `version` | the `2` integer, the readers refuse the other versions | yes |
| `network` | the network description: the `cfg` text of the network cfg file (required, not empty) and the `classes` array of the class objects of the `index` integer and the `name` string, every index from zero once in any order | yes |
| `training` | the training record: the `seconds` count the training took and the `iteration` integers, the `loss` and the `avg_loss` floats of every training iteration in the order of the training, the three arrays of the same size; the `map_iteration` integers and the `map` floats of every mAP calculation (the `--map` trainings alone write them), the two arrays together, of the same size and of the ascending iterations | no |
| `header`, `layers`, `trailing` | the weights, the dxxwjz1 ones (see above); the absent `header` is the one of the current version and the zero seen | no |

So the cfg file of the network is recovered of the file with any JSON tool (`gzip -dc net.dxxwjz2 | jq -r .network.cfg > net.cfg`). The JSON text is UTF-8, so the bytes of the cfg file or of the class names which aren't (the Latin-1 comment of the cfg file, for example) are kept as the U+FFFD replacement characters. The losses are the floats of the dxxwjz1 arrays: the shortest decimal text the float round trips of or the `"0x"` bit pattern string of the non finite ones (the NaN loss of the diverged training).

**The file of no weights is the network to train.** The dxxwjz2 file of no weights (no float in its arrays) loads no weights, the network keeps its initialization and no network array the file has none of is warned of: the training trains such a file, while the detection refuses it (see the [Training with the formats](#training-with-the-formats) section). The file of the `network` alone (of no layers: the one the converter makes of a cfg file, see the [conversion](#the-conversion), or a tool exports) is the network to train from scratch: neither the seen of its header nor its training record is resumed, and the writers write it with neither the `header` nor the `layers` member.

The readers check the members of the network and of the training the way the dxxwjz1 ones are checked (the types of the values, the indexes of the classes, the sizes of the columns, the repeated keys), skip their unknown keys with a warning and warn of the class names of a count other than the `classes=` of the network. The classes of the dxxwjz2 version are built on the dxxwjz1 ones, the [`dxxwjz2/v2`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2) directory keeps what they add:

- **`Dxxwjz2Format`** - the constants of the format: the name, the version, the file name extension and the JSON keys it adds.
- **`Dxxwjz2JsonEmitter`** - the `Dxxwjz1JsonEmitter` of the network and the training members, which it writes between the generator and the header (its `sections()`): a class on a line and a column of the losses on a line.
- **`Dxxwjz2SaxHandler`** - the `Dxxwjz1SaxHandler` of the dxxwjz2 format name, version and required keys, which hands the values of the network and the training keys over to the `Dxxwjz2SubtreeBuilder` (the JSON trees of them, small beside the weights, the floats of the exact values of their texts) and the `Dxxwjz2MetadataReader` (the `NetworkDescription` and the `TrainingRecord` of the trees). Its peek stops at the first weight once the network is read, so the network description of the file is read without its weights.
- **`Dxxwjz2Parser`** - the parse of the files and of the texts by the `Dxxwjz1Parser` with the dxxwjz2 handler, the peek of the files.
- **`Dxxwjz2Reader`**, **`Dxxwjz2Writer`** - the `Dxxwjz1Reader` and the `Dxxwjz1Writer` of the dxxwjz2 validation and emitter: the network description and the training record go into the rest of the load and come of the rest of the save and of the document written. The writer refuses the rest of no cfg text, since the file keeps the network description.
- **`Dxxwjz2Validator`** - the validator of the files: the `Dxxwjz1Validator` checks, the cfg text, the class names of the network.
- **`Dxxwjz2FileFormat`** - the `IWeightsFileFormat` of the dxxwjz2 files.

## The format neutral model

- **`WeightsSchema`** ([`WeightsSchema.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsSchema.h)) - the arrays and the sub-layers of every layer type, their order and their names (the table above).
- **`WeightsLayout`** ([`WeightsLayout.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsLayout.h)) - the arrays of a network in the order of the files: the arrays the saves write (the EMA ones of the EMA save) and the ones the loads read with their transpositions, up to a layer.
- **`WeightsDocument`** ([`WeightsDocument.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h)) - the content of a weights file: the header, the layers with their arrays and the bytes after the arrays. The arrays in their order are the payload of the original weights file, so the document of the file keeps every byte of it: the truncated arrays, the part of a float the truncated file ends in and the extra bytes included.
- **`NetworkDescription`** ([`NetworkDescription.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/NetworkDescription.h)) and **`TrainingRecord`** ([`TrainingRecord.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/TrainingRecord.h)) - the network and the training of the document the dxxwjz2 files keep: the cfg text and the class names, the seconds, the losses of the iterations and the mAPs of the mAP calculations.
- **`NetworkFiles`** ([`NetworkFiles.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/NetworkFiles.h)) - reads the text of the cfg file and the names of the darknet class names file (a name on a line, the CR LF line ends included).
- **`WeightsDocumentValidator`** ([`WeightsDocumentValidator.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocumentValidator.h)) - validates a document alone and against a network, with the findings in the `WeightsReport`.

The validators report the errors, which keep a file from being loaded or converted, and the warnings about the files, which load, but not the way their authors might have expected:

| Validator | Errors | Warnings |
| --- | --- | --- |
| `OrigWeightsValidator` | the file unable to be read, shorter than the header or gzip compressed; for the document written as the original weights file of the network: the array missing in the middle, the shorter array followed by others, the array of no network layer | the truncated file, the part of a float at the end, the extra bytes, the file made for the load of the `dontload` and the `dontloadscales` layers |
| `Dxxwjz1Validator` | the file unable to be read, the corrupted or the truncated gzip data, the JSON syntax errors, the other format or version, the wrong structure, the documents the `WeightsDocumentValidator` rejects | the plain JSON file, the unknown keys, the ones of the `WeightsDocumentValidator` |
| `Dxxwjz2Validator` | the ones of the `Dxxwjz1Validator`, the document of no cfg text, the wrong network or training members | the ones of the `Dxxwjz1Validator` (but the network arrays of the file of no weights), the class names of the count other than the classes of the network |
| `WeightsDocumentValidator` | the 32 bits seen of the seen beyond the 32 bits, the repeated layers, the arrays the layers of their types have none; for the network: the layers beyond its layers or of the other types, the arrays it has none, the arrays longer than its ones | the arrays shorter than the network ones, the network arrays the document has none |

## The conversion

The **`WeightsConverter`** ([`WeightsConverter.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-converter/WeightsConverter.h)) performs the **`WeightsConversion`** ([`WeightsConversion.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-converter/WeightsConversion.h)) of a file of one format into another one (or into the same one): the network of the cfg file is parsed (the original weights file requires it, the rest is validated against it when it's given), the source is validated and read into the document, the document is written into the temporary file of the destination directory, which replaces the destination once complete. A failed conversion leaves the destination as it was and no temporary file behind, and the stop request of the application (a Ctrl+C) stops the conversion between its steps.

The original weights file is split into the arrays the `save_weights()` writes of the network, unless the file is of the size of the arrays the `load_weights()` reads. The original weights file converted to the dxxwjz1 or the dxxwjz2 one and back is the same file byte to byte, whatever it holds: the header of any version, the truncated arrays, the extra bytes. The file converted into the original weights one with the cfg file is checked by the `OrigWeightsValidator` to hold the arrays of the network one after another (see above), while without the cfg file there is no network to check the document against, so its arrays are written as they are.

The dxxwjz2 destination keeps the network description: the cfg text of the cfg file of the conversion (a cfg or a dxxwjz2 file) over the one of the dxxwjz2 source, which one of them is required, and the class names of the class names file of the conversion over the classes of the source, over the `names=` file of the network cfg. The training record of the dxxwjz2 source goes on into the dxxwjz2 destination, while the formats of the weights alone keep neither of them.

The conversion of no source file makes the network to train of the cfg file: the dxxwjz2 file of the network description alone (the cfg text and the class names, the way above), of no layers. Such file converted into the formats of the weights alone would keep nothing but a header, so that conversion fails, while the weights files of the networks of no weights arrays convert back and forth as the rest.

## Training with the formats

The training saves its weights into the dxxwjz2 files by default (the `--save-format` command line parameter names the format of them, the `LibraryContext::save_format` of the library): the `<base>_<iteration>`, the `<base>_last`, the `<base>_final`, the `<base>_best` and the `<base>_ema` files of the backup directory, with the extension of the format. The C training code saves them by the `weights_saver` of its context, which the `DarknetContext` points to the **`TrainingJournal`** ([`TrainingJournal.h`](/src/lib/darknet-adaptor/adaptors/TrainingJournal/TrainingJournal.h)) of the training:

- the journal starts with the network description of the training: the text of the network cfg file of the training (or of the dxxwjz2 one) and the class names of the `--names` file, of the dxxwjz2 weights file, of the dxxwjz2 network file or of the `names=` file of the cfg, the first of them given;
- it resumes the training record of the dxxwjz2 weights file the training starts from up to the iteration of the weights (the record of the iterations after it is dropped, the `--clear` drops the whole record with the iterations), the seconds of it's clock start at the seconds of the record;
- it records the losses of every iteration and the mAP of every mAP calculation once, in the ascending order of the iterations, the iteration saved before it's report included, and saves every weights file of the time of the training so far.

So the file of a training resumed of a dxxwjz2 one keeps the losses and the mAPs of every training iteration in their order and the whole time of all the trainings, and the average loss of the resumed training goes on of the last one of the record. The chart of every training start is the new `chart_<base>_<YYYY-MM-DD--HH-MM-SS>.png` file of the date and the time of the start, so no chart of the trainings resumed before is overwritten. The whole record of the resumed weights (the average losses and the mAP line) is redrawn on it and the chart is saved at once, then every 100 iterations (every iteration of the `--save-charts-every-iter`), at the end and at the stop of the training; the `--chart-path` image goes on for the weights of no record alone (the original and the dxxwjz1 ones). The chart of the record of a dxxwjz2 file is drawn at any moment as well (the `--draw-chart` command line parameter, the `TrainingChart` class and the `LibraryFacade::training_chart()` of the library, the chart the GUI shows as soon as the training folder is opened).

The mAP calculation of the training (the `--map`) takes the class names the same way, so the class names of the dxxwjz2 file need no names file; the names of a count other than the classes of the network fail the training.

The detection and the training read the weights files of every format, the format told by the file name extension, and the file of the network is a cfg file or a dxxwjz2 one:

| Command line | The network | The weights |
| --- | --- | --- |
| `--train-cfg net.cfg --weights w` | of the `net.cfg` | of the `w` file of any format, the training record of the dxxwjz2 one resumed |
| `--train-cfg net.dxxwjz2` | of the cfg the file keeps | of the file, none for the file of no weights (the training from scratch) |
| `--detect-cfg net.cfg --weights w` | of the `net.cfg` | of the `w` file of any format, the dxxwjz2 one of no weights refused |
| `--detect-cfg net.dxxwjz2` | of the cfg the file keeps | of the file |
| `--weights net.dxxwjz2` alone | of the cfg the file keeps | the detection of the weights of the file, the training of the network of the file of no weights |

The backups of a training are named after the file of it's network: the `<base>` of the `net.cfg` and of the `net.dxxwjz2` files is the `net`, and the one of the backup files of a training (the `net_last`, the `net_final`, the `net_best`, the `net_ema` and the `net_<iteration>` dxxwjz2 files of the iteration of their weights) is the `net` too, so the training resumed of the backup file alone goes on with the same files.

## Using the formats

The C++ code loads and saves the weights of a network with any format through the interfaces:

```
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2Reader.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2Validator.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-formats/WeightsFileFormats.h"

using namespace darknet_adaptor::loaders;

// net is the network view of an ANetworkAdaptor (the networkptr its get()
// returns), the rest keeps the network description and the training record
WeightsDocument rest;
rest.network = NetworkDescription{cfg_text, {"cat", "dog"}};

const IWeightsFileFormatPtr format = WeightsFileFormats::named("dxxwjz2");

format->create_writer()->save(*net, "backup.dxxwjz2", net->n, false, rest);
WeightsFileFormats::of_file("backup.dxxwjz2")
    ->create_reader()
    ->load(*net, "backup.dxxwjz2", net->n, rest);

// the file checked against the network before the load
const WeightsReport report =
    dxxwjz2::v2::Dxxwjz2Validator::validate("backup.dxxwjz2", net.get());
```
