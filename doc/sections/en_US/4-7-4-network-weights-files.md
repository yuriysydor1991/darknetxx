# Network weights files

The `darknetxx` reads and writes the weights of the networks in six formats:

- the **original darknet weights file** (`*.weights`), the binary file the original Darknet reads and writes;
- the **dxxwjz1 file** (`*.dxxwjz1`, the **d**arknet**xx** **w**eights **j**son **z**ipped file of the first version), the gzip compressed JSON document of the same weights, readable by any JSON tool and by a human;
- the **dxxwjz2 file** (`*.dxxwjz2`, the file of the second version), the dxxwjz1 document of everything the network is detected and trained with beside its weights: the text of the network cfg file, the names of the classes with their indexes, the loss of every training iteration in the order of the training and the time the training took;
- the **dxxwjz2p file** (`*.dxxwjz2p`, the dxxwjz2 **p**roject file), the dxxwjz2 document of the YOLO4 training project: the network, the weights and the training record beside the YOLO4 annotations of the images, everything the training needs but the images themselves (see the [The dxxwjz2p project file format](#the-dxxwjz2p-project-file-format) section);
- the **dxxwjz3 file** (`*.dxxwjz3`, the file of the third version), the dxxwjz2 document of nothing but the JSON data: the network cfg is the JSON structure of the options of its sections, the `net` object of the `[net]` ones, instead of the text of the cfg file, the layer sections keep the arrays of their layers (see the [The dxxwjz3 file format of the third version](#the-dxxwjz3-file-format-of-the-third-version) section);
- the **dxxwjz3p file** (`*.dxxwjz3p`, the dxxwjz3 **p**roject file), the dxxwjz2p project of the dxxwjz3 document, the whole training project of the JSON data alone (see the [The dxxwjz3p project file format](#the-dxxwjz3p-project-file-format) section).

The **YOLO4 training folder**, the one the ImagesAnnotator exports, converts into the dxxwjz2p project and back: it's the `yolo4` format of the conversion alone (see the [The YOLO4 training folder](#the-yolo4-training-folder) section).

The training and the detection of the `darknetxx` read the weights files of every format (the `--weights` command line parameter, the format of a file is told by its name extension), the training saves its weights into the dxxwjz3 files by default (see the [Training with the formats](#training-with-the-formats) section below) and the dxxwjz2, the dxxwjz2p, the dxxwjz3 and the dxxwjz3p files alone are the networks the detection runs with.

The weights of the formats convert into each other with no loss: the original weights file converted to the dxxwjz1, the dxxwjz2 or the dxxwjz3 one and back is the same file byte to byte (see the [Converting the network weights files](/doc/sections/en_US/6-2-4-converting-the-network-weights-files.md) section and the [The darknetxxConverter binary](/doc/sections/en_US/5-35-the-darknetxxConverter-binary.md) one), while the network description and the training record are kept by the dxxwjz2 and the dxxwjz3 files and their projects alone, the annotations of the images by the dxxwjz2p and the dxxwjz3p projects.

The components of the files live in the `darknet_adaptor::loaders` namespace under the [`src/NetworksLoaders`](/src/NetworksLoaders) directory:

| Directory | Holds |
| --- | --- |
| [`NetworksLoaders`](/src/NetworksLoaders) | the `IWeightsReader`, the `IWeightsWriter` and the `IWeightsFileFormat` interfaces every format implements |
| [`weights-model`](/src/NetworksLoaders/weights-model) | the format neutral model of the weights: the arrays of every layer kind and their order (`WeightsSchema`), the arrays of a network in that order (`WeightsLayout`), the content of a file (`WeightsDocument` with the `NetworkDescription`, the `TrainingRecord` and the `Yolo4Annotations` of the `Yolo4Image` and the `Yolo4Box`), the reads of the cfg and the class names files (`NetworkFiles`) and the validation (`WeightsDocumentValidator`, `WeightsReport`) |
| [`original-weights`](/src/NetworksLoaders/original-weights) | the original weights file: the `OrigWeightsReader`, the `OrigWeightsWriter`, the `OrigWeightsValidator` and the `OrigWeightsFileFormat` |
| [`dxxwjz/dxxwjz1`](/src/NetworksLoaders/dxxwjz/dxxwjz1) | the dxxwjz1 file of the first version: the `Dxxwjz1Reader`, the `Dxxwjz1Writer`, the `Dxxwjz1Validator` and the `Dxxwjz1FileFormat` with the support classes of theirs (the `dxxwjz1::v1` namespace) |
| [`dxxwjz/dxxwjz2`](/src/NetworksLoaders/dxxwjz/dxxwjz2) | the dxxwjz2 file of the second version: the `Dxxwjz2Reader`, the `Dxxwjz2Writer`, the `Dxxwjz2Validator` and the `Dxxwjz2FileFormat` with the support classes of theirs (the `dxxwjz2::v2` namespace), which extend the dxxwjz1 ones |
| [`dxxwjz/dxxwjz2p`](/src/NetworksLoaders/dxxwjz/dxxwjz2p) | the dxxwjz2p project file of the first version: the `Dxxwjz2pReader`, the `Dxxwjz2pWriter`, the `Dxxwjz2pValidator` and the `Dxxwjz2pFileFormat` with the support classes of theirs (the `dxxwjz2p::v1` namespace), which extend the dxxwjz2 ones |
| [`dxxwjz/dxxwjz3`](/src/NetworksLoaders/dxxwjz/dxxwjz3) | the dxxwjz3 file of the third version: the `Dxxwjz3Reader`, the `Dxxwjz3Writer`, the `Dxxwjz3Validator` and the `Dxxwjz3FileFormat` with the support classes of theirs (the `dxxwjz3::v3` namespace), which extend the dxxwjz2 ones |
| [`dxxwjz/dxxwjz3p`](/src/NetworksLoaders/dxxwjz/dxxwjz3p) | the dxxwjz3p project file of the first version: the `Dxxwjz3pReader`, the `Dxxwjz3pWriter`, the `Dxxwjz3pValidator` and the `Dxxwjz3pFileFormat` with the support classes of theirs (the `dxxwjz3p::v1` namespace), which extend the dxxwjz2p ones of the dxxwjz3 members |
| [`yolo4-folder`](/src/NetworksLoaders/yolo4-folder) | the YOLO4 training folder of the conversion: the `Yolo4FolderReader`, the `Yolo4FolderWriter` and the `Yolo4FolderFormat` with the `Yolo4FolderLayout` of the files of the folder (the `yolo4` namespace) |
| [`weights-formats`](/src/NetworksLoaders/weights-formats) | the formats of their names and file name extensions and the ones of the conversion (`WeightsFileFormats`) and the files networks are made of: the cfg, the dxxwjz2, the dxxwjz2p, the dxxwjz3 and the dxxwjz3p ones (`NetworkSource`) |
| [`weights-converter`](/src/NetworksLoaders/weights-converter) | the `WeightsConverter` of the files between the formats and the `WeightsConversion` it performs |

The journal of the training, the `TrainingJournal` of the [`TrainingJournal`](/src/darknet-adaptor/adaptors/TrainingJournal) directory, keeps the network description and the training record the weights files of the training keep, the `TrainingChart` of the [`TrainingChart`](/src/darknet-adaptor/adaptors/TrainingChart) directory draws the chart of the record (see the [Training with the formats](#training-with-the-formats) section), the `TrainingDataRegistry` of the [`TrainingData`](/src/darknet-adaptor/adaptors/TrainingData) directory hands the image lists and the labels of the dxxwjz2p and the dxxwjz3p projects over to the darknet C training code (see the [Training the dxxwjz2p project](#training-the-dxxwjz2p-project) section).

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
    +{static} section_type(header) : LAYER_TYPE
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
    +annotations : Yolo4Annotations
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

  class Yolo4Annotations {
    +images : Yolo4Image[]
    +training_paths() / validation_paths() : string[]
  }

  class Yolo4Image {
    +path : string
    +width / height : int
    +boxes : Yolo4Box[]
    +validation : bool
    +{static} label_of(path) : string
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
    #trailing(bytes) : string
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
  class Dxxwjz2SaxHandler {
    #metadata() : Dxxwjz2MetadataReader
    #builder(name) : Dxxwjz2SubtreeBuilder
    #trailing() : delegate
  }
  class Dxxwjz2SubtreeBuilder {
    #closed(value) : bool
  }
  class Dxxwjz2MetadataReader {
    +network(value, description) : bool
    +training(value, record) : bool
  }
  class Dxxwjz2JsonEmitter {
    #classes()
    #training()
  }
  class Dxxwjz2Format <<constants>>
}

package "dxxwjz2p::v1" {
  class Dxxwjz2pReader
  class Dxxwjz2pWriter
  class Dxxwjz2pFileFormat
  class Dxxwjz2pValidator {
    +{static} validate(filename, net) : WeightsReport
  }
  class Dxxwjz2pParser {
    +{static} parse_file(filename, document, report) : bool
    +{static} peek(filename, rest, weights, report) : bool
    +{static} directory_of(filename) : path
  }
  class Dxxwjz2pSaxHandler
  class Dxxwjz2pAnnotationsReader {
    +annotations(value, classes, annotations) : bool
  }
  class Dxxwjz2pJsonEmitter
  class Dxxwjz2pAnnotationsJson {
    +write(sink)
  }
  class Dxxwjz2pFormat <<constants>>
}

package "dxxwjz3::v3" {
  class Dxxwjz3Reader
  class Dxxwjz3Writer {
    +{static} keeps(description, layers, filename) : bool
  }
  class Dxxwjz3FileFormat
  class Dxxwjz3Validator {
    +{static} validate(filename, net) : WeightsReport
  }
  class Dxxwjz3Parser {
    +{static} parse_file(filename, document, report) : bool
    +{static} peek(filename, rest, weights, report) : bool
  }
  class Dxxwjz3SaxHandler
  class Dxxwjz3MetadataReader {
    +network(value, description) : bool
    +arrays(value, index, layers) : bool
  }
  class Dxxwjz3NetworkBuilder
  class Dxxwjz3JsonEmitter {
    #members()
  }
  class Dxxwjz3CfgJson {
    +net() : string
    +layers() : strings
    +types() : types
  }
  class Dxxwjz3ValueCodec {
    +{static} json_of(value) : string
    +{static} text_of(value, text) : bool
  }
  class Dxxwjz3Bytes {
    +{static} json_of(bytes) : string
  }
  class Dxxwjz3Format <<constants>>
}

package "dxxwjz3p::v1" {
  class Dxxwjz3pReader
  class Dxxwjz3pWriter
  class Dxxwjz3pFileFormat
  class Dxxwjz3pValidator {
    +{static} validate(filename, net) : WeightsReport
  }
  class Dxxwjz3pParser {
    +{static} parse_file(filename, document, report) : bool
    +{static} peek(filename, rest, weights, report) : bool
  }
  class Dxxwjz3pSaxHandler
  class Dxxwjz3pJsonEmitter
  class Dxxwjz3pFormat <<constants>>
}

package "yolo4" {
  class Yolo4FolderFormat
  class Yolo4FolderReader
  class Yolo4FolderWriter
  class Yolo4FolderLayout {
    +{static} cfg(root) : string
    +{static} names(root) : string
    +{static} latest_weights(root, cfg) : string
  }
}

package "weights-formats" {
  class WeightsFileFormats {
    +{static} all() : formats
    +{static} convertible() : formats
    +{static} named(name, formats) : IWeightsFileFormatPtr
    +{static} of_file(path) : IWeightsFileFormatPtr
  }
  class NetworkSource {
    +{static} describes(path) : bool
    +{static} read(path, description / rest) : bool
    +{static} base_of(path, iteration) : string
    +{static} option_of(path, key, def) : string
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

class TrainingDataRegistry {
  +{static} publish(annotations)
  +{static} withdraw()
  +{static} paths(name, found) : bool
  +{static} boxes(labelpath, found) : bool
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
IWeightsFileFormat <|.. Dxxwjz2pFileFormat
IWeightsFileFormat <|.. Dxxwjz3FileFormat
IWeightsFileFormat <|.. Dxxwjz3pFileFormat
IWeightsFileFormat <|.. Yolo4FolderFormat
IWeightsReader <|.. Yolo4FolderReader
IWeightsWriter <|.. Yolo4FolderWriter

Dxxwjz1Reader <|-- Dxxwjz2Reader
Dxxwjz1Writer <|-- Dxxwjz2Writer
Dxxwjz1SaxHandler <|-- Dxxwjz2SaxHandler
Dxxwjz1JsonEmitter <|-- Dxxwjz2JsonEmitter
Dxxwjz2Reader <|-- Dxxwjz2pReader
Dxxwjz2Writer <|-- Dxxwjz2pWriter
Dxxwjz2SaxHandler <|-- Dxxwjz2pSaxHandler
Dxxwjz2JsonEmitter <|-- Dxxwjz2pJsonEmitter
Dxxwjz2MetadataReader <|-- Dxxwjz2pAnnotationsReader
Dxxwjz2Reader <|-- Dxxwjz3Reader
Dxxwjz2Writer <|-- Dxxwjz3Writer
Dxxwjz2SaxHandler <|-- Dxxwjz3SaxHandler
Dxxwjz2JsonEmitter <|-- Dxxwjz3JsonEmitter
Dxxwjz2MetadataReader <|-- Dxxwjz3MetadataReader
Dxxwjz2pReader <|-- Dxxwjz3pReader
Dxxwjz2pWriter <|-- Dxxwjz3pWriter
Dxxwjz2pSaxHandler <|-- Dxxwjz3pSaxHandler
Dxxwjz3JsonEmitter <|-- Dxxwjz3pJsonEmitter
Dxxwjz2SubtreeBuilder <|-- Dxxwjz3NetworkBuilder

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

Dxxwjz2pReader ..> Dxxwjz2pValidator
Dxxwjz2pValidator ..> Dxxwjz2pParser
Dxxwjz2pValidator ..> Dxxwjz2Validator : validates the rest by
Dxxwjz2pParser ..> Dxxwjz2Parser : peeks by
Dxxwjz2pParser ..> Dxxwjz2pSaxHandler
Dxxwjz2pSaxHandler ..> Dxxwjz2pAnnotationsReader : the annotations tree
Dxxwjz2pWriter ..> Dxxwjz2pJsonEmitter
Dxxwjz2pJsonEmitter ..> Dxxwjz2pAnnotationsJson : the annotations

Dxxwjz3Reader ..> Dxxwjz3Validator
Dxxwjz3Validator ..> Dxxwjz3Parser
Dxxwjz3Validator ..> Dxxwjz2Validator : validates the rest by
Dxxwjz3Parser ..> Dxxwjz3SaxHandler
Dxxwjz3SaxHandler ..> Dxxwjz3NetworkBuilder : the network tree
Dxxwjz3SaxHandler ..> Dxxwjz3MetadataReader : the network tree
Dxxwjz3NetworkBuilder ..> Dxxwjz3MetadataReader : the arrays of the layers
Dxxwjz3SaxHandler ..> Dxxwjz3Bytes : the trailing bytes
Dxxwjz3MetadataReader ..> Dxxwjz3ValueCodec : the option values
Dxxwjz3Writer ..> Dxxwjz3JsonEmitter
Dxxwjz3JsonEmitter ..> Dxxwjz3CfgJson : the network cfg
Dxxwjz3JsonEmitter ..> Dxxwjz3Bytes : the trailing bytes
Dxxwjz3CfgJson ..> Dxxwjz3ValueCodec : the option values
Dxxwjz3CfgJson ..> "CfgSection" : reads the cfg text by

Dxxwjz3pReader ..> Dxxwjz3pValidator
Dxxwjz3pValidator ..> Dxxwjz3pParser
Dxxwjz3pParser ..> Dxxwjz3pSaxHandler
Dxxwjz3pSaxHandler ..> Dxxwjz3NetworkBuilder : the network tree
Dxxwjz3pSaxHandler ..> Dxxwjz3MetadataReader : the network tree
Dxxwjz3pSaxHandler ..> Dxxwjz3Bytes : the trailing bytes
Dxxwjz3pWriter ..> Dxxwjz3pJsonEmitter
Dxxwjz3pWriter ..> Dxxwjz3Writer : checks the network by
Dxxwjz3pJsonEmitter ..> Dxxwjz2pAnnotationsJson : the annotations

Yolo4FolderReader ..> Yolo4FolderLayout
Yolo4FolderWriter ..> Yolo4FolderLayout
Yolo4FolderReader ..> WeightsFileFormats : reads the latest weights by
Yolo4FolderWriter ..> Dxxwjz2Writer : writes the latest weights by

WeightsDocumentValidator ..> WeightsLayout
WeightsDocumentValidator ..> WeightsSchema
WeightsLayout ..> WeightsSchema
WeightsDocument ..> WeightsSchema : the file order
WeightsDocument *-- NetworkDescription
WeightsDocument *-- TrainingRecord
WeightsDocument *-- Yolo4Annotations
Yolo4Annotations *-- Yolo4Image

WeightsFileFormats o-- IWeightsFileFormat
NetworkSource ..> WeightsFileFormats : peeks the dxxwjz2 and the dxxwjz3 files and their projects by
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
"OrigDefaultLoader" ..> TrainingDataRegistry : the lists of the project training
TrainingDataRegistry ..> Yolo4Annotations : the lists and the labels of
TrainingJournal ..> WeightsFileFormats : updates the dxxwjz2p and the dxxwjz3p projects by
"darknetxx_impl::WeightsConversionLib" ..> WeightsConverter : converts the library context
@enduml

## The readers and the writers

Every format implements the three interfaces:

- **`IWeightsReader`** ([`IWeightsReader.h`](/src/NetworksLoaders/IWeightsReader.h)) - the `load(net, filename, cutoff)` method loads the weights of the layers before the cutoff into the network arrays (the `load_weights_upto()` of the original Darknet), the `load(net, filename, cutoff, rest)` one keeps the rest of the file as well: the document without the arrays, of the network description and the training record of the formats which keep them. The `read(filename, net, document)` method reads the whole file into the format neutral `WeightsDocument`, the `peek(filename, rest, weights)` one reads the rest of the file up to its first weights array (the whole network of the dxxwjz3 files and their projects, which keeps the weights) and tells the file holds weights (the dxxwjz2 and the dxxwjz3 files and their projects alone, the rest of the formats keep no network description to peek).
- **`IWeightsWriter`** ([`IWeightsWriter.h`](/src/NetworksLoaders/IWeightsWriter.h)) - the `save(net, filename, cutoff, save_ema)` method saves the weights of the layers before the cutoff (the `save_weights_upto()` of the original Darknet: the current version, the seen of the current iteration, the EMA arrays of the convolutional layers for the `save_ema`), the `save(net, filename, cutoff, save_ema, rest)` one saves the network description and the training record of the rest beside them, which the formats of the weights alone leave out. The `write(document, filename)` method writes the document.
- **`IWeightsFileFormat`** ([`IWeightsFileFormat.h`](/src/NetworksLoaders/IWeightsFileFormat.h)) - the format itself: its name (the one of the command line), its file name extension and description, whether its files keep the structure of the network (the layers and the names of the arrays, so they're read without the network) and the network description, the reader and the writer of its files. The `WeightsFileFormats` of the [`weights-formats`](/src/NetworksLoaders/weights-formats) directory finds the formats of their names and of the file name extensions (any extension but the `.dxxwjz1`, the `.dxxwjz2`, the `.dxxwjz2p`, the `.dxxwjz3` and the `.dxxwjz3p` ones is the original weights file) and the formats of the conversion: every one and the YOLO4 folder (its `convertible()`), the `NetworkSource` reads the network description of the file a network is made of: the text of the cfg file or the description the dxxwjz2 and the dxxwjz3 files and their projects keep, and the keys of the `[net]` section of it.

The readers and the writers of the formats load and save the same arrays of the network the same way, so any file of a network loads the same network. The `read()` and the `write()` of the documents are what converts the files between the formats.

## The original darknet weights file

The file is the int32 `major`, `minor` and `revision` version triple, the count of the images the network has seen (the uint64 `seen` of the `(major * 10 + minor) >= 2` versions, the uint32 one of the older versions), followed by the float arrays of the layers with the weights one after another, in the order of the layers and of the table below. The file keeps no structure of its own: which float belongs to which array is told by the network alone.

- **`OrigWeightsReader`** ([`OrigWeightsReader.h`](/src/NetworksLoaders/original-weights/OrigWeightsReader.h)) - the C++ port of the original `load_weights()`, `load_weights_upto()` and the per layer routines, which reads exactly the bytes the original did: it skips the `dontload` layers and the batch normalization arrays of the `dontloadscales` ones, transposes the weights of the `flipped` convolutional layers and the connected weights of the files of the major or minor version above 1000, fills the arrays of a truncated file with the whole floats it holds and stops after the layer reading past the end of the file. Its `read()` splits the file into the arrays of the network the file is of (the network is required).
- **`OrigWeightsWriter`** ([`OrigWeightsWriter.h`](/src/NetworksLoaders/original-weights/OrigWeightsWriter.h)) - the C++ port of the original `save_weights()`, `save_weights_upto()` and the per layer routines, byte to byte compatible with the original files. Its `write()` writes the header, every array of the document and its trailing bytes, which makes the file the document was read of.
- **`OrigWeightsValidator`** ([`OrigWeightsValidator.h`](/src/NetworksLoaders/original-weights/OrigWeightsValidator.h)) - compares the size of the file with the arrays of the network: the arrays the `save_weights()` writes (the file darknet saves) and the ones the `load_weights()` reads, which leave out the `dontload` and the `dontloadscales` arrays (the file made for the load of such a network). The file of the size of neither is truncated or holds extra bytes, which it reports as the warnings, while the gzip compressed file (a dxxwjz1 one given as the original one) and the file shorter than the header are the errors. Its `validate(document, net)` checks a document to be written as the original weights file of the network: the file keeps no names of its arrays, so the arrays of the document have to be the network ones from the first one on, all of them complete but the last one (the truncated file). The document which misses an array in the middle or holds a shorter array followed by others (a dxxwjz1 file edited by hand, for example) is the error, since the loaders would read the arrays after the gap into the wrong ones.

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

The support classes of the [`dxxwjz/dxxwjz1`](/src/NetworksLoaders/dxxwjz/dxxwjz1) directory:

- **`Dxxwjz1Format`** - the constants of the format: the name, the version, the file name extension and the JSON keys.
- **`Dxxwjz1FloatCodec`** - the JSON text of the floats described above (the C++17 `std::to_chars()` and `std::from_chars()`, independent of the locale).
- **`Dxxwjz1Base64`** - the base64 text of the trailing bytes.
- **`Dxxwjz1JsonString`** - the JSON string text of any text: the bytes which aren't UTF-8 are replaced with the U+FFFD characters.
- **`Dxxwjz1JsonEmitter`** - writes the JSON text of the document into a stream a layer at a time, a layer on a line and an array on a line, straight from the network arrays or the document ones. The emitters of the later versions write their members by its `sections()` and their trailing bytes by its `trailing()`, the dxxwjz3 ones their own document by its `begin()`, `layer()` and `end()` of the layer arrays of its `arrays()`.
- **`Dxxwjz1SaxHandler`** - builds the document of the events of the [nlohmann json](https://github.com/nlohmann/json) SAX parser a value at a time, so no JSON tree of the millions of the floats is made, and checks the structure of the document on the way: the JSON syntax, the format and the version, the types of the values, the names of the layer types, of the sub-layers and of the arrays, the repeated keys. The handlers of the later versions tell their own format name, version and required keys and take the values of their own document keys, the trailing one of the dxxwjz3 version included, while the dxxwjz3 ones skip the layers key as an unknown one (their `layers_member()`, see the dxxwjz2 and the dxxwjz3 formats below).
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

The readers check the members of the network and of the training the way the dxxwjz1 ones are checked (the types of the values, the indexes of the classes, the sizes of the columns, the repeated keys), skip their unknown keys with a warning and warn of the class names of a count other than the `classes=` of the network. The classes of the dxxwjz2 version are built on the dxxwjz1 ones, the [`dxxwjz/dxxwjz2`](/src/NetworksLoaders/dxxwjz/dxxwjz2) directory keeps what they add:

- **`Dxxwjz2Format`** - the constants of the format: the name, the version, the file name extension and the JSON keys it adds.
- **`Dxxwjz2JsonEmitter`** - the `Dxxwjz1JsonEmitter` of the network and the training members, which it writes between the generator and the header (its `sections()`): a class on a line and a column of the losses on a line, the cfg member of the network by its `cfg()`, the loss columns by its `losses()`.
- **`Dxxwjz2SaxHandler`** - the `Dxxwjz1SaxHandler` of the dxxwjz2 format name, version and required keys, which hands the values of the network and the training keys over to the `Dxxwjz2SubtreeBuilder` (the JSON trees of them, small beside the weights, the floats of the exact values of their texts) and the `Dxxwjz2MetadataReader` (the `NetworkDescription` and the `TrainingRecord` of the trees). Its peek stops at the first weight once the network is read, so the network description of the file is read without its weights. The handlers of the later versions build the network tree by the builder of their `builder()`, read it by the metadata reader of their `metadata()` and the trailing bytes by the handler of their `trailing()`.
- **`Dxxwjz2Parser`** - the parse of the files and of the texts by the `Dxxwjz1Parser` with the dxxwjz2 handler, the peek of the files.
- **`Dxxwjz2Reader`**, **`Dxxwjz2Writer`** - the `Dxxwjz1Reader` and the `Dxxwjz1Writer` of the dxxwjz2 validation and emitter: the network description and the training record go into the rest of the load and come of the rest of the save and of the document written. The writer refuses the rest of no cfg text, since the file keeps the network description.
- **`Dxxwjz2Validator`** - the validator of the files: the `Dxxwjz1Validator` checks, the cfg text, the class names of the network.
- **`Dxxwjz2FileFormat`** - the `IWeightsFileFormat` of the dxxwjz2 files.

## The dxxwjz2p project file format

The dxxwjz2p file is the dxxwjz2 document (see above) of the YOLO4 training project: the network description, the weights and the training record beside the YOLO4 annotations of the images, everything the training needs but the images themselves. So the project is the single dxxwjz2p file of the absolute image paths or the dxxwjz2p file of the image paths relative to it beside the folders of the images, of any names. The annotations member is the project document of the [ImagesAnnotator](https://github.com/yuriysydor1991/ImagesAnnotator) (of its 0.12.0 structure) with the validation flag of the images:

```
{
  "format": "dxxwjz2p",
  "version": 1,
  "generator": "darknetxx 0.11.0",
  "network": {"cfg": "[net]\nbatch=64\nwidth=416\n...", "classes": [
    {"index": 0, "name": "cat"},
    {"index": 1, "name": "dog"}
  ]},
  "training": {"seconds": 3725, "iteration": [1,2,3], ...},
  "annotations": {"ImagesAnnotator": {"version": "0.12.0"}, "annotations": [
    {"db": {"path": "/srv/shots"}, "annotations": [
      {"rel_path": "park.jpg", "width": 640, "height": 480, "annotations": [{"name": "dog", "rect": {"x": 200, "y": 130, "width": 48, "height": 52}}]}
    ]},
    {"db": {"path": "images"}, "annotations": [
      {"rel_path": "street.png", "width": 320, "height": 200, "validation": true, "annotations": []}
    ]}
  ]},
  "header": {"major": 0, "minor": 2, "revision": 5, "seen": 192},
  "layers": [...]
}
```

The keys of the document beside the dxxwjz2 ones:

| Key | Value | Required |
| --- | --- | --- |
| `format` | the `"dxxwjz2p"` string | yes |
| `version` | the `1` integer, the readers refuse the other versions | yes |
| `annotations` | the annotations of the images: the `ImagesAnnotator` object of the `version` of the structure and the `annotations` array of the folders of the images | yes |

The keys of the annotations:

| Object | Key | Value | Required |
| --- | --- | --- | --- |
| the folder | `db` | the object of the `path` of the folder: relative to the folder of the dxxwjz2p file or absolute | yes |
| | `annotations` | the image objects of the folder | yes |
| the image | `rel_path` | the path of the image relative to the folder, not empty | yes |
| | `width`, `height` | the size of the image in the pixels, the integers from zero, the `0` (unknown) by default; the image of the boxes requires both of them | no |
| | `validation` | the `true` of the image of the mAP calculation alone (the darknet `valid=` list), the `false` of the training one by default (the `train=` list) | no |
| | `annotations` | the box objects of the image, the empty array of the image of no object | yes |
| | `scale_factor` | the zoom of the ImagesAnnotator, skipped | no |
| the box | `name` | the class name of the box, one of the `network` classes | yes |
| | `rect` | the `x` and the `y` of the top left corner and the `width` and the `height` of the box in the pixels of the image, the JSON numbers | yes |
| | `scale_factor` | skipped | no |

The memory keeps the boxes the YOLO4 way (the class index and the center and the size of the box, the fractions of the size of the image), the rects of the file are the pixels of them: the shortest decimal text of the float of the pixels, so the integer rects of the ImagesAnnotator read and write back the same. The writers group the images by the folders of their paths, an image on a line: the folders below the one of the file are written relative to it, the rest are the absolute ones, so the folder of the project and of its images moves as a whole. No validation image makes the training images the ones of the mAP calculation, the darknet way.

The file is the dxxwjz2 one for everything else: the detection and the recognition run the network of the dxxwjz2p file alone, the converter makes the dxxwjz2 weights file of it of no annotations (see the [conversion](#the-conversion)), the dxxwjz2p file of no weights is the network to train. The readers check the annotations the way the dxxwjz2 members are checked: the types of the values, the required keys, the class names of the network (the annotations read before the network wait for it), the size of the images of the boxes and the image paths of the length of the darknet label paths; the unknown keys are skipped with a warning. The peek reads the annotations as well, the ones after the weights included (the writers write them before the header).

The classes of the dxxwjz2p version are built on the dxxwjz2 ones, the [`dxxwjz/dxxwjz2p`](/src/NetworksLoaders/dxxwjz/dxxwjz2p) directory keeps what they add:

- **`Dxxwjz2pFormat`** - the constants of the format: the name, the version, the file name extension, the version of the ImagesAnnotator structure and the JSON keys it adds.
- **`Dxxwjz2pJsonEmitter`** - the `Dxxwjz2JsonEmitter` of the annotations member, which it writes after the network and the training ones by the **`Dxxwjz2pAnnotationsJson`** (the one of the dxxwjz3p projects too): a folder of the images of a db path, an image on a line.
- **`Dxxwjz2pAnnotationsReader`** - the `Dxxwjz2MetadataReader` of the `Yolo4Annotations` of the annotations tree.
- **`Dxxwjz2pSaxHandler`** - the `Dxxwjz2SaxHandler` of the dxxwjz2p format name, version and required keys, which reads the annotations tree once the class names of the network are read. Its peek stops at the first weight once both the network and the annotations are read.
- **`Dxxwjz2pParser`** - the parse and the peek of the files by the dxxwjz2p handler of the folder of the file.
- **`Dxxwjz2pReader`**, **`Dxxwjz2pWriter`** - the `Dxxwjz2Reader` and the `Dxxwjz2Writer` of the annotations, which go into the rest of the load and come of the rest of the save and of the document written. The writer refuses the boxes the file can't keep: the ones of the classes of no name, of the images of no size, of no finite values.
- **`Dxxwjz2pValidator`** - the validator of the files: the `Dxxwjz2Validator` checks of the dxxwjz2p document.
- **`Dxxwjz2pFileFormat`** - the `IWeightsFileFormat` of the dxxwjz2p files.

## The dxxwjz3 file format of the third version

The dxxwjz3 file is the dxxwjz2 document (see above) of nothing but the JSON data: the network member keeps the JSON structure of the network cfg instead of the text of the cfg file, every layer of it keeps the arrays of the layer, and the trailing bytes are the JSON array of their values instead of the base64 text, so any JSON tool reads and edits every option of the network (`gzip -dc net.dxxwjz3 | jq .network.net.batch`). The document of the network of the two classes:

```
{
  "format": "dxxwjz3",
  "version": 3,
  "generator": "darknetxx 0.11.0",
  "network": {"net": {"batch": 64, "subdivisions": 16, "width": 416, "height": 416, "channels": 3, "hue": 0.1, "learning_rate": 0.0013, "max_batches": 6000, "policy": "steps", "steps": [4800,5400], "scales": [0.1,0.1]}, "layers": [
    {"type": "convolutional", "options": {"batch_normalize": 1, "filters": 32, "size": 3, "stride": 1, "pad": 1, "activation": "mish"}, "arrays": {
      "biases": [0.5,-0.25,...],
      "scales": [...],
      "rolling_mean": [...],
      "rolling_variance": [...],
      "weights": [0.125,"0x7fc00000",...]
    }},
    {"type": "maxpool", "options": {"size": 2, "stride": 2}},
    {"type": "route", "options": {"layers": [-1,-3]}},
    {"type": "yolo", "options": {"mask": [0,1,2], "anchors": [12,16,19,36,40,28], "classes": 2, "num": 3, "jitter": 0.3}}
  ], "classes": ["cat", "dog"]},
  "training": {"seconds": 3725,
    "loss": [812.5,790.25,701],
    "avg_loss": [812.5,810.275,799.3475],
    "map_iteration": [3],
    "map": [0.0125]
  },
  "header": {"major": 0, "minor": 2, "revision": 5, "seen": 192},
  "trailing": [0,1]
}
```

The keys of the document beside the dxxwjz2 ones (the header is the dxxwjz2 one, the file of no header keeps no weights; there is no layers key, the network layers keep the arrays):

| Key | Value | Required |
| --- | --- | --- |
| `format` | the `"dxxwjz3"` string | yes |
| `version` | the `3` integer, the readers refuse the other versions | yes |
| `network` | the network description: the `net` object of the options of the `[net]` section of the cfg (the `[network]` one), the `layers` array of the layer sections in the order of the cfg with the arrays of their layers, both required, and the `classes` array of the class name strings, the name of an index is the one of the class of the index | yes |
| `training` | the dxxwjz2 training record of no `iteration` column: the losses of an index are the ones of the iteration of the index + 1, so the `loss` of the 1000th iteration is the `loss[999]` (the NaN losses of the iterations of no record) | no |
| `trailing` | the bytes the original weights file keeps after the arrays: the array of the integers from 0 to 255 | no |

A layer of the network is the object of the `type` string, the name of the cfg section without the brackets as the cfg names it (the `conv` of the `[conv]` section, while the layers of the weights keep the names of the darknet layer types), and of the `options` object of the options of the section, none for the section of no options. The layer of the weights keeps them in the `arrays` object of the dxxwjz1 layers (the `sublayers` one of the recurrent layers, see the [The dxxwjz1 file format of the first version](#the-dxxwjz1-file-format-of-the-first-version) section): the arrays of the layer of the index of the object in the `layers` array, of the layer type the darknet parses the section of the `type` into (the `[conv]` and the `[convolutional]` sections are the convolutional layers, see the `WeightsSchema::section_type()`). The layer object of no arrays keeps no weights: the layers of no weights (the maxpool, the route, the yolo ones), every layer of the network to train and the layers beyond the cutoff of the save. So the readers read the network and its weights of the same objects, of no layer indexes and types to match, while the writers refuse the layer which isn't the one of the cfg section of its index (of no such section or of another type). The `layers` key of the document (the one of the dxxwjz1 and the dxxwjz2 documents) is the unknown key the readers skip with a warning. The options are the `key=value` lines of the section in their order, the values are the JSON values of the darknet ones:

| The cfg option | The JSON value |
| --- | --- |
| `batch=64`, `layers=-1` | the integer: `64`, `-1` (of the 64 bits range) |
| `hue=.1`, `decay=0.0005`, `exposure=1.50` | the fraction of the shortest fixed notation decimal of it's float (the darknet keeps the floats of the options): `0.1`, `0.0005`, `1.5` |
| `activation=leaky`, `train=data/train.txt` | the string: `"leaky"`, `"data/train.txt"` |
| `mask=0,1,2`, `scales=.1,.1` | the array of the comma separated values: `[0,1,2]`, `[0.1,0.1]` |
| `random` (the line of no `=`) | the `null`, the NULL value of the darknet, which takes the default of the option |

The number is the one of the text the darknet reads the same way: the `atoi()` and the `atof()` of the text and of the number are the same, so the `1e-3` stays the `"1e-3"` string (the `atoi()` of it is 1, the one of the `0.001` is 0), the `0x10` and the `nan` the strings too. The readers take the JSON numbers of any notation (the `"learning_rate": 1e-3` of a hand edit is the `0.001`) and any value the writers write, while the empty values, the strings of the white space (the darknet drops it of the cfg lines), the booleans, the objects and the arrays of anything but the numbers and the strings are the errors, so are the option keys of the white space, of the `=` and of the leading `#`, `;` or `[` and the section names of the white space or the brackets.

The readers turn the network member into the cfg text of the network: the `[net]` section of the `net` options, a section of each layer, a `key=value` line of each option (the key alone of the `null`), the sections separated by an empty line. So the rest of the darknetxx works with the cfg text of the dxxwjz3 file the way it does with the one of the dxxwjz2 file, and the cfg file is recovered of the file by the converter: the `--dump-cfg` flag prints it's cfg text (see the [Printing the network cfg of the file](/doc/sections/en_US/6-2-4-converting-the-network-weights-files.md#printing-the-network-cfg-of-the-file) section), the dxxwjz3 file converted into the dxxwjz2 one keeps it. The JSON keeps no comments and no white space of the cfg file, nor the repeated keys of a section, the darknet reads the first one of: the cfg text converted into the dxxwjz3 file and back is the same network of the normalized cfg text, which converts into the same dxxwjz3 file again. The writers refuse the cfg text the JSON can't keep: the one of no `[net]` section first, of an option before it, of a section header of no closing bracket (the cfg the darknet refuses too).

The readers build the tree of the network member a layer at a time: the arrays of a layer object go into the layers of the document once the object is complete, so no JSON tree of the weights of the whole network is made. The network member keeps both the network description and the weights, so the peek of the file reads the whole member: it takes about the time of the read of the whole file (the network of the 9 million floats takes about 2 seconds of the release build), while the peek of the dxxwjz2 file stops at its first weight.

The dxxwjz3 file is the dxxwjz2 one for everything else: the training and the detection read it, the training saves it (by default, the `--save-format dxxwjz3`), the converter converts it into the other formats and back of the weights of the same bytes, the file of no weights is the network to train. The classes of the dxxwjz3 version are built on the dxxwjz2 ones, the [`dxxwjz/dxxwjz3`](/src/NetworksLoaders/dxxwjz/dxxwjz3) directory keeps what they add:

- **`Dxxwjz3Format`** - the constants of the format: the name, the version, the file name extension and the JSON keys it adds.
- **`Dxxwjz3ValueCodec`** - the JSON values of the cfg options of the table above both ways: the JSON text of the option text, the cfg text of the JSON value.
- **`Dxxwjz3CfgJson`** - the JSON text of the cfg text, which it reads the darknet way (the `CfgSection::read_cfg()` of the `OrigDefaultLoader`): the members of the `[net]` options, the `type` and the `options` members of every layer section and the layer types of the sections.
- **`Dxxwjz3Bytes`** - the JSON array of the trailing bytes: the text of the bytes, the bytes of the SAX events of the array a value at a time (no JSON tree of the megabytes of the bytes of the original weights file of another network is made).
- **`Dxxwjz3Losses`** - the loss columns of the losses of an iteration at the index of the iteration - 1, the NaN ones of the iterations of none.
- **`Dxxwjz3JsonEmitter`** - the `Dxxwjz2JsonEmitter` of the network member of the JSON of the network cfg, which writes every layer object with the arrays of its `layer()` (the layers of none of no arrays) and the members after the network one by its `members()` before the header, of the array of the class names, of the trailing bytes (it's `trailing()`) and of the loss columns of the `Dxxwjz3Losses` (it's `losses()`).
- **`Dxxwjz3MetadataReader`** - the `Dxxwjz2MetadataReader` of the cfg text of the network tree, which it checks the way the rest of the members are checked, of the arrays of a layer object (it's `arrays()`, which moves them out of the tree into the layer of the document of the index of the object and the type of its section), of the class names array and of the training tree of no iteration column.
- **`Dxxwjz3NetworkBuilder`** - the `Dxxwjz2SubtreeBuilder` of the network value, which hands every layer object over to the `arrays()` of the `Dxxwjz3MetadataReader` once the object is complete (it's `closed()`), so the tree keeps the weights of a layer at a time.
- **`Dxxwjz3SaxHandler`** - the `Dxxwjz2SaxHandler` of the dxxwjz3 format name and version, which builds the network tree by the `Dxxwjz3NetworkBuilder` (it's `builder()`), reads it by the `Dxxwjz3MetadataReader` (it's `metadata()`) and the trailing bytes by the `Dxxwjz3Bytes` (it's `trailing()`) and skips the layers key of the document (it's `layers_member()`).
- **`Dxxwjz3Parser`**, **`Dxxwjz3Reader`**, **`Dxxwjz3Writer`**, **`Dxxwjz3Validator`**, **`Dxxwjz3FileFormat`** - the dxxwjz2 ones of the dxxwjz3 handler and emitter; the writer refuses the cfg text the JSON can't keep and the layers which aren't the ones of the cfg sections of their indexes (it's `keeps()`).

## The dxxwjz3p project file format

The dxxwjz3p file is the dxxwjz2p project (see above) of the dxxwjz3 document, so the whole training project is the JSON data alone: the network member and the trailing bytes are the dxxwjz3 ones, the annotations member is the dxxwjz2p one.

```
{
  "format": "dxxwjz3p",
  "version": 1,
  "generator": "darknetxx 0.11.0",
  "network": {"net": {"batch": 64, "width": 416, ...}, "layers": [...], "classes": ["cat", "dog"]},
  "training": {"seconds": 3725, "loss": [812.5,790.25,701], ...},
  "annotations": {"ImagesAnnotator": {"version": "0.12.0"}, "annotations": [
    {"db": {"path": "images"}, "annotations": [
      {"rel_path": "park.jpg", "width": 640, "height": 480, "annotations": [{"name": "dog", "rect": {"x": 200, "y": 130, "width": 48, "height": 52}}]}
    ]}
  ]},
  "header": {"major": 0, "minor": 2, "revision": 5, "seen": 192}
}
```

| Key | Value | Required |
| --- | --- | --- |
| `format` | the `"dxxwjz3p"` string | yes |
| `version` | the `1` integer, the readers refuse the other versions | yes |
| `network`, `training`, `trailing` | the dxxwjz3 ones | yes, no, no |
| `annotations` | the dxxwjz2p one | yes |

The file is the dxxwjz2p one for everything else: the `--train-cfg project.dxxwjz3p` training trains the images of it's annotations and updates the project, the training GUI opens it, the detection runs it's network, the converter converts it into the dxxwjz2p project and back of the same weights and annotations and into the dxxwjz3 weights file of no annotations. The classes of the [`dxxwjz/dxxwjz3p`](/src/NetworksLoaders/dxxwjz/dxxwjz3p) directory are the dxxwjz2p ones of the dxxwjz3 network member and trailing bytes (the `Dxxwjz3NetworkBuilder`, the `Dxxwjz3MetadataReader` and the `Dxxwjz3Bytes` of the dxxwjz3 version): the `Dxxwjz3pFormat`, the `Dxxwjz3pJsonEmitter` (the `Dxxwjz3JsonEmitter` of the annotations member of the `Dxxwjz2pAnnotationsJson`), the `Dxxwjz3pSaxHandler`, the `Dxxwjz3pParser`, the `Dxxwjz3pReader`, the `Dxxwjz3pWriter` (which refuses the network the dxxwjz3 file can't keep as well), the `Dxxwjz3pValidator` and the `Dxxwjz3pFileFormat`.

## The YOLO4 training folder

The YOLO4 training folder, the one the ImagesAnnotator exports, is the `yolo4` format of the conversion alone: of no file name extension, so neither the training nor the detection takes a folder for a weights file. The `Yolo4FolderReader` of the [`yolo4-folder`](/src/NetworksLoaders/yolo4-folder) directory reads the folder into the document of the dxxwjz2p project the way the training reads it, the `Yolo4FolderWriter` writes the document into a new folder, the `Yolo4FolderLayout` knows the files of the folder:

| Files | The reader | The writer |
| --- | --- | --- |
| `cfg/*.cfg` | the network: the first `.cfg` file by the name | `cfg/<name>.cfg` of the name of the folder up to its first dot: the cfg text of the train=, the valid=, the names= and the backup= keys of the `[net]` section of the files of the folder |
| `data/obj.names` | the class names over the ones of the weights, the ones of the `names=` file of the cfg for neither | the class names of the network |
| `data/obj.data` | not read, the training reads none | the one of the original darknet |
| the `train=` and the `valid=` lists of the cfg (the `data/train.txt` and the train list by default) | the images of the training and of the mAP calculation, relative to the folder or absolute | the `data/train.txt` and the `data/val.txt` of the validation images, the train list is the valid one for none |
| the images | the size of the images: of the header of the stb_image formats, of the OpenCV for the rest | the copies in the `data/obj/` folder of the names of their own: the `_<n>` suffix of the same stem, the case blind file systems included |
| the labels (the darknet `replace_image_to_label()` of the image paths) | the boxes, none of the image of no label file | the `data/obj/<stem>.txt` |
| the backup folder of the `backup=` key of the cfg | the latest weights the training resumes from, of the network of the folder cfg for the original weights file | the `backup/<name>_last.dxxwjz3` of the weights of the document and of it's training record |

The folder converted into the dxxwjz2p file and back is of the same lists, labels, class names and weights. The converter writes no folder into a folder of the files (the destination folder of the converter replaces the empty one alone), so no folder of the user is deleted.

## The format neutral model

- **`WeightsSchema`** ([`WeightsSchema.h`](/src/NetworksLoaders/weights-model/WeightsSchema.h)) - the arrays and the sub-layers of every layer type, their order and their names (the table above).
- **`WeightsLayout`** ([`WeightsLayout.h`](/src/NetworksLoaders/weights-model/WeightsLayout.h)) - the arrays of a network in the order of the files: the arrays the saves write (the EMA ones of the EMA save) and the ones the loads read with their transpositions, up to a layer.
- **`WeightsDocument`** ([`WeightsDocument.h`](/src/NetworksLoaders/weights-model/WeightsDocument.h)) - the content of a weights file: the header, the layers with their arrays and the bytes after the arrays. The arrays in their order are the payload of the original weights file, so the document of the file keeps every byte of it: the truncated arrays, the part of a float the truncated file ends in and the extra bytes included.
- **`NetworkDescription`** ([`NetworkDescription.h`](/src/NetworksLoaders/weights-model/NetworkDescription.h)) and **`TrainingRecord`** ([`TrainingRecord.h`](/src/NetworksLoaders/weights-model/TrainingRecord.h)) - the network and the training of the document the dxxwjz2 and the dxxwjz3 files keep: the cfg text and the class names, the seconds, the losses of the iterations and the mAPs of the mAP calculations.
- **`Yolo4Annotations`** ([`Yolo4Annotations.h`](/src/NetworksLoaders/weights-model/Yolo4Annotations.h)), **`Yolo4Image`** ([`Yolo4Image.h`](/src/NetworksLoaders/weights-model/Yolo4Image.h)) and **`Yolo4Box`** ([`Yolo4Box.h`](/src/NetworksLoaders/weights-model/Yolo4Box.h)) - the annotations of the document the dxxwjz2p and the dxxwjz3p files keep: the images of their absolute paths, sizes, boxes and sets, the boxes of the YOLO4 label files (the class index, the center and the size, the fractions of the size of the image) and the darknet label path of an image (the `Yolo4Image::label_of()`).
- **`NetworkFiles`** ([`NetworkFiles.h`](/src/NetworksLoaders/weights-model/NetworkFiles.h)) - reads the text of the cfg file and the names of the darknet class names file (a name on a line, the CR LF line ends included).
- **`WeightsDocumentValidator`** ([`WeightsDocumentValidator.h`](/src/NetworksLoaders/weights-model/WeightsDocumentValidator.h)) - validates a document alone and against a network, with the findings in the `WeightsReport`.

The validators report the errors, which keep a file from being loaded or converted, and the warnings about the files, which load, but not the way their authors might have expected:

| Validator | Errors | Warnings |
| --- | --- | --- |
| `OrigWeightsValidator` | the file unable to be read, shorter than the header or gzip compressed; for the document written as the original weights file of the network: the array missing in the middle, the shorter array followed by others, the array of no network layer | the truncated file, the part of a float at the end, the extra bytes, the file made for the load of the `dontload` and the `dontloadscales` layers |
| `Dxxwjz1Validator` | the file unable to be read, the corrupted or the truncated gzip data, the JSON syntax errors, the other format or version, the wrong structure, the documents the `WeightsDocumentValidator` rejects | the plain JSON file, the unknown keys, the ones of the `WeightsDocumentValidator` |
| `Dxxwjz2Validator` | the ones of the `Dxxwjz1Validator`, the document of no cfg text, the wrong network or training members | the ones of the `Dxxwjz1Validator` (but the network arrays of the file of no weights), the class names of the count other than the classes of the network |
| `Dxxwjz2pValidator` | the ones of the `Dxxwjz2Validator`, the wrong annotations, the boxes of the classes of no name and of the images of no size | the ones of the `Dxxwjz2Validator`, the unknown keys of the annotations |
| `Dxxwjz3Validator` | the ones of the `Dxxwjz2Validator` of the dxxwjz3 document, the wrong network cfg (the `net` and the `layers` members, the option keys and values), arrays of the layers (the names of the arrays and of the sub-layers, the floats, the layer types of the arrays) and trailing bytes | the ones of the `Dxxwjz2Validator`, the unknown keys of the network and of it's layers, the layers key of the document |
| `Dxxwjz3pValidator` | the ones of the `Dxxwjz3Validator` and of the `Dxxwjz2pValidator` | the ones of the `Dxxwjz3Validator` and of the `Dxxwjz2pValidator` |
| `WeightsDocumentValidator` | the 32 bits seen of the seen beyond the 32 bits, the repeated layers, the arrays the layers of their types have none; for the network: the layers beyond its layers or of the other types, the arrays it has none, the arrays longer than its ones | the arrays shorter than the network ones, the network arrays the document has none |

## The conversion

The **`WeightsConverter`** ([`WeightsConverter.h`](/src/NetworksLoaders/weights-converter/WeightsConverter.h)) performs the **`WeightsConversion`** ([`WeightsConversion.h`](/src/NetworksLoaders/weights-converter/WeightsConversion.h)) of a file of one format into another one (or into the same one): the network of the cfg file is parsed (the original weights file requires it, the rest is validated against it when it's given), the source is validated and read into the document, the document is written into the temporary file of the destination directory, which replaces the destination once complete. A failed conversion leaves the destination as it was and no temporary file behind, and the stop request of the application (a Ctrl+C) stops the conversion between its steps.

The original weights file is split into the arrays the `save_weights()` writes of the network, unless the file is of the size of the arrays the `load_weights()` reads. The original weights file converted to the dxxwjz1, the dxxwjz2 or the dxxwjz3 one and back is the same file byte to byte, whatever it holds: the header of any version, the truncated arrays, the extra bytes. The file converted into the original weights one with the cfg file is checked by the `OrigWeightsValidator` to hold the arrays of the network one after another (see above), while without the cfg file there is no network to check the document against, so its arrays are written as they are.

The dxxwjz2 and the dxxwjz3 destinations keep the network description: the cfg text of the cfg file of the conversion (a cfg, a dxxwjz2 or a dxxwjz3 file) over the one of the source, which one of them is required, and the class names of the class names file of the conversion over the classes of the source, over the `names=` file of the network cfg. The training record of the source goes on into the dxxwjz2 and the dxxwjz3 destinations, while the formats of the weights alone keep neither of them.

The dxxwjz2p and the dxxwjz3p destinations keep the annotations of the dxxwjz2p, the dxxwjz3p or the YOLO4 folder source, of none of the rest: the project converted into the dxxwjz2 or the dxxwjz3 file is the weights file of the project of no annotations, the one of the inference. The YOLO4 folder source is the network, the latest weights and the annotations of the folder, the YOLO4 folder destination is the new folder of the copies of the images of the document (see the [The YOLO4 training folder](#the-yolo4-training-folder) section).

The conversion of no source file makes the network to train of the cfg file: the dxxwjz2 or the dxxwjz3 file of the network description alone (the cfg text and the class names, the way above), of no layers. Such file converted into the formats of the weights alone would keep nothing but a header, so that conversion fails, while the weights files of the networks of no weights arrays convert back and forth as the rest.

## Training with the formats

The training saves its weights into the dxxwjz3 files by default (the `--save-format` command line parameter names the format of them, the `LibraryContext::save_format` of the library): the `<base>_<iteration>_<avg loss>` (the numbered ones, of the average loss of their iteration, e.g. the `net_1200_0.734521`), the `<base>_last`, the `<base>_final`, the `<base>_best` and the `<base>_ema` files of the backup directory, with the extension of the format. The C training code saves them by the `weights_saver` of its context, which the `DarknetContext` points to the **`TrainingJournal`** ([`TrainingJournal.h`](/src/darknet-adaptor/adaptors/TrainingJournal/TrainingJournal.h)) of the training:

- the journal starts with the network description of the training: the text of the network cfg file of the training (or of the dxxwjz2 one) and the class names of the `--names` file, of the dxxwjz2 weights file, of the dxxwjz2 network file or of the `names=` file of the cfg, the first of them given;
- it resumes the training record of the dxxwjz2 weights file the training starts from up to the iteration of the weights (the record of the iterations after it is dropped, the `--clear` drops the whole record with the iterations), the seconds of it's clock start at the seconds of the record;
- it records the losses of every iteration and the mAP of every mAP calculation once, in the ascending order of the iterations, the iteration saved before it's report included, and saves every weights file of the time of the training so far.

So the file of a training resumed of a dxxwjz2 one keeps the losses and the mAPs of every training iteration in their order and the whole time of all the trainings, and the average loss of the resumed training goes on of the last one of the record. The chart of every training start is the new `chart_<base>_<YYYY-MM-DD--HH-MM-SS>.png` file of the date and the time of the start, so no chart of the trainings resumed before is overwritten. The whole record of the resumed weights (the average losses and the mAP line) is redrawn on it and the chart is saved at once, then every 100 iterations (every iteration of the `--save-charts-every-iter`), at the end and at the stop of the training; the `--chart-path` image goes on for the weights of no record alone (the original and the dxxwjz1 ones). The chart of the record of a dxxwjz2 file is drawn at any moment as well (the `--draw-chart` command line parameter, the `TrainingChart` class and the `LibraryFacade::training_chart()` of the library, the chart the GUI shows as soon as the training folder is opened).

The mAP calculation of the training (the `--map`) takes the class names the same way, so the class names of the dxxwjz2 file need no names file; the names of a count other than the classes of the network fail the training.

The detection and the training read the weights files of every format, the format told by the file name extension, and the file of the network is a cfg file or a dxxwjz2 one (the dxxwjz3 files and the dxxwjz3p projects are read the way the dxxwjz2 and the dxxwjz2p ones are):

| Command line | The network | The weights |
| --- | --- | --- |
| `--train-cfg net.cfg --weights w` | of the `net.cfg` | of the `w` file of any format, the training record of the dxxwjz2 one resumed |
| `--train-cfg net.dxxwjz2` | of the cfg the file keeps | of the file, none for the file of no weights (the training from scratch) |
| `--detect-cfg net.cfg --weights w` | of the `net.cfg` | of the `w` file of any format, the dxxwjz2 one of no weights refused |
| `--detect-cfg net.dxxwjz2` | of the cfg the file keeps | of the file |
| `--weights net.dxxwjz2` alone | of the cfg the file keeps | the detection of the weights of the file, the training of the network of the file of no weights |
| `--train-cfg project.dxxwjz2p` | of the cfg the file keeps | of the file, the training of the images of the annotations of the file, which updates it (see below) |
| `--detect-cfg project.dxxwjz2p`, `--weights project.dxxwjz2p` alone | of the cfg the file keeps | the ones of the dxxwjz2 file |

The backups of a training are named after the file of it's network: the `<base>` of the `net.cfg` and of the `net.dxxwjz2` files is the `net`, and the one of the backup files of a training (the `net_last`, the `net_final`, the `net_best`, the `net_ema` and the `net_<iteration>_<avg loss>` dxxwjz2 files of the iteration of their weights, the `net_<iteration>` ones of the earlier versions too) is the `net` too, so the training resumed of the backup file alone goes on with the same files.

## Training the dxxwjz2p project

The dxxwjz3p project trains the same way, of the dxxwjz3p file of it's latest weights. The training of the dxxwjz2p file (the `--train-cfg project.dxxwjz2p` command line, the dxxwjz2p project the training GUI opens) trains the network of the file, resumed from it's weights, on the images of it's annotations: the image lists and the labels of the training are the ones of the annotations, over the `train=` and the `valid=` lists of the cfg and the label files beside the images. The `OrigDefaultLoader` names them the lists of the training networks and the `DarknetContext` of the training publishes them in the `TrainingDataRegistry` of the [`TrainingData`](/src/darknet-adaptor/adaptors/TrainingData) directory, which it withdraws at the end of the training. The darknet C code reads them by the `get_paths()` of the `dxxwjz2p:train` and the `dxxwjz2p:valid` list names and the `read_boxes()` of the label paths of the images, which ask the registry before the files (the `get_training_data_paths()`, the `has_training_data_paths()` and the `read_training_data_boxes()` C functions of the `TrainingDataCApi`). So the images stay where they are, the project keeps no copies of them and no list or label file is written.

Every save of the training updates the project as well: the dxxwjz2p file of the latest weights, of the training record and of the annotations is written into the `<project>.saving` file beside the project, which replaces the project once complete. So the training of the project resumes from it's latest weights, while the backup folder of the `backup=` key of it's cfg beside the project keeps the backups of the save format, and the charts are drawn beside the project.

## Using the formats

The C++ code loads and saves the weights of a network with any format through the interfaces:

```
#include "src/NetworksLoaders/dxxwjz/dxxwjz2/Dxxwjz2Reader.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz2/Dxxwjz2Validator.h"
#include "src/NetworksLoaders/weights-formats/WeightsFileFormats.h"

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
