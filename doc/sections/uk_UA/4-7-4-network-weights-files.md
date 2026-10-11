# Файли ваг нейромереж

`darknetxx` читає і записує ваги нейромереж у шести форматах:

- **оригінальний файл ваг darknet** (`*.weights`) - бінарний файл, який читає і записує оригінальний Darknet;
- **файл dxxwjz1** (`*.dxxwjz1`, від англ. **d**arknet**xx** **w**eights **j**son **z**ipped - стиснений файл ваг darknetxx у форматі JSON, перша версія) - стиснений gzip документ JSON з тими самими вагами, який може прочитати як будь-який інструмент для роботи з JSON, так і людина;
- **файл dxxwjz2** (`*.dxxwjz2`, друга версія) - документ dxxwjz1 з усім, з чим нейромережу навчають і з чим вона розпізнає, окрім її ваг: текстом файлу cfg нейромережі, назвами класів з їхніми індексами, втратами (loss) кожної ітерації навчання у порядку навчання і часом, який зайняло навчання;
- **файл dxxwjz2p** (`*.dxxwjz2p`, від англ. dxxwjz2 **p**roject - файл проекту dxxwjz2) - документ dxxwjz2 проекту навчання YOLO4: нейромережа, ваги і запис навчання поряд з анотаціями зображень YOLO4, тобто все, що потрібно навчанню, окрім самих зображень (див. секцію [Формат файлу проекту dxxwjz2p](#формат-файлу-проекту-dxxwjz2p));
- **файл dxxwjz3** (`*.dxxwjz3`, третя версія) - документ dxxwjz2, який складається лише з даних JSON: cfg нейромережі - це структура JSON опцій її секцій, об'єкт `net` опцій секції `[net]`, а не текст файлу cfg, а секції шарів зберігають масиви своїх шарів (див. секцію [Формат файлу dxxwjz3 третьої версії](#формат-файлу-dxxwjz3-третьої-версії));
- **файл dxxwjz3p** (`*.dxxwjz3p`, від англ. dxxwjz3 **p**roject - файл проекту dxxwjz3) - проект dxxwjz2p з документом dxxwjz3, тобто весь проект навчання лише з даних JSON (див. секцію [Формат файлу проекту dxxwjz3p](#формат-файлу-проекту-dxxwjz3p)).

**Тека навчання YOLO4**, та, яку експортує ImagesAnnotator, конвертується у проект dxxwjz2p і назад: це формат `yolo4`, який використовує лише конвертування (див. секцію [Тека навчання YOLO4](#тека-навчання-yolo4)).

Навчання і розпізнавання `darknetxx` читають файли ваг кожного формату (параметр командного рядка `--weights`, формат файлу визначає розширення його назви), навчання за замовчуванням зберігає свої ваги у файли dxxwjz3 (див. секцію [Навчання і формати](#навчання-і-формати) нижче), а самі лише файли dxxwjz2, dxxwjz2p, dxxwjz3 і dxxwjz3p - це нейромережі, з якими працює розпізнавання.

Ваги форматів конвертуються один в одний без втрат: оригінальний файл ваг, конвертований у файл dxxwjz1, dxxwjz2 чи dxxwjz3 і назад, байт у байт збігається з початковим (див. секції [Конвертування файлів ваг нейромереж](/doc/sections/uk_UA/6-2-4-converting-the-network-weights-files.md) і [Бінарний файл darknetxxConverter](/doc/sections/uk_UA/5-35-the-darknetxxConverter-binary.md)), тоді як опис нейромережі і запис навчання зберігають лише файли dxxwjz2 і dxxwjz3 та їхні проекти, а анотації зображень - лише проекти dxxwjz2p і dxxwjz3p.

Компоненти файлів ваг розміщені у просторі імен `darknet_adaptor::loaders` у директорії [`src/NetworksLoaders`](/src/NetworksLoaders):

| Директорія | Містить |
| --- | --- |
| [`NetworksLoaders`](/src/NetworksLoaders) | інтерфейси `IWeightsReader`, `IWeightsWriter` і `IWeightsFileFormat`, які реалізує кожен формат |
| [`weights-model`](/src/NetworksLoaders/weights-model) | незалежну від формату модель ваг: масиви кожного виду шарів та їхній порядок (`WeightsSchema`), масиви нейромережі у цьому порядку (`WeightsLayout`), вміст файлу (`WeightsDocument` з `NetworkDescription`, `TrainingRecord` і `Yolo4Annotations` з `Yolo4Image` і `Yolo4Box`), читання файлів cfg і назв класів (`NetworkFiles`) і перевірку (`WeightsDocumentValidator`, `WeightsReport`) |
| [`original-weights`](/src/NetworksLoaders/original-weights) | оригінальний файл ваг: `OrigWeightsReader`, `OrigWeightsWriter`, `OrigWeightsValidator` і `OrigWeightsFileFormat` |
| [`dxxwjz/dxxwjz1`](/src/NetworksLoaders/dxxwjz/dxxwjz1) | файл dxxwjz1 першої версії: `Dxxwjz1Reader`, `Dxxwjz1Writer`, `Dxxwjz1Validator` і `Dxxwjz1FileFormat` з їхніми допоміжними класами (простір імен `dxxwjz1::v1`) |
| [`dxxwjz/dxxwjz2`](/src/NetworksLoaders/dxxwjz/dxxwjz2) | файл dxxwjz2 другої версії: `Dxxwjz2Reader`, `Dxxwjz2Writer`, `Dxxwjz2Validator` і `Dxxwjz2FileFormat` з їхніми допоміжними класами (простір імен `dxxwjz2::v2`), які розширюють класи dxxwjz1 |
| [`dxxwjz/dxxwjz2p`](/src/NetworksLoaders/dxxwjz/dxxwjz2p) | файл проекту dxxwjz2p першої версії: `Dxxwjz2pReader`, `Dxxwjz2pWriter`, `Dxxwjz2pValidator` і `Dxxwjz2pFileFormat` з їхніми допоміжними класами (простір імен `dxxwjz2p::v1`), які розширюють класи dxxwjz2 |
| [`dxxwjz/dxxwjz3`](/src/NetworksLoaders/dxxwjz/dxxwjz3) | файл dxxwjz3 третьої версії: `Dxxwjz3Reader`, `Dxxwjz3Writer`, `Dxxwjz3Validator` і `Dxxwjz3FileFormat` з їхніми допоміжними класами (простір імен `dxxwjz3::v3`), які розширюють класи dxxwjz2 |
| [`dxxwjz/dxxwjz3p`](/src/NetworksLoaders/dxxwjz/dxxwjz3p) | файл проекту dxxwjz3p першої версії: `Dxxwjz3pReader`, `Dxxwjz3pWriter`, `Dxxwjz3pValidator` і `Dxxwjz3pFileFormat` з їхніми допоміжними класами (простір імен `dxxwjz3p::v1`), які розширюють класи dxxwjz2p членами dxxwjz3 |
| [`yolo4-folder`](/src/NetworksLoaders/yolo4-folder) | теку навчання YOLO4 для конвертування: `Yolo4FolderReader`, `Yolo4FolderWriter` і `Yolo4FolderFormat` з `Yolo4FolderLayout` файлів теки (простір імен `yolo4`) |
| [`weights-formats`](/src/NetworksLoaders/weights-formats) | формати за їхніми назвами і розширеннями назв файлів, а також формати конвертування (`WeightsFileFormats`) і файли, з яких будуються нейромережі: файли cfg, dxxwjz2, dxxwjz2p, dxxwjz3 і dxxwjz3p (`NetworkSource`) |
| [`weights-converter`](/src/NetworksLoaders/weights-converter) | `WeightsConverter` - конвертер файлів між форматами - і конвертування `WeightsConversion`, яке він виконує |

Журнал навчання, `TrainingJournal` директорії [`TrainingJournal`](/src/darknet-adaptor/adaptors/TrainingJournal), тримає опис нейромережі і запис навчання, які зберігають файли ваг навчання, `TrainingChart` директорії [`TrainingChart`](/src/darknet-adaptor/adaptors/TrainingChart) малює графік цього запису (див. секцію [Навчання і формати](#навчання-і-формати)), а `TrainingDataRegistry` директорії [`TrainingData`](/src/darknet-adaptor/adaptors/TrainingData) передає списки зображень і мітки проектів dxxwjz2p і dxxwjz3p коду навчання darknet мовою C (див. секцію [Навчання проекту dxxwjz2p](#навчання-проекту-dxxwjz2p)).

Окремий вихідний файл діаграми нижче -
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

## Читання і запис

Кожен формат реалізує три інтерфейси:

- **`IWeightsReader`** ([`IWeightsReader.h`](/src/NetworksLoaders/IWeightsReader.h)) - метод `load(net, filename, cutoff)` завантажує ваги шарів до межі відсікання у масиви нейромережі (`load_weights_upto()` оригінального Darknet), а метод `load(net, filename, cutoff, rest)` ще й зберігає решту файлу: документ без масивів з описом нейромережі і записом навчання тих форматів, які їх зберігають. Метод `read(filename, net, document)` читає весь файл у незалежний від формату `WeightsDocument`, а метод `peek(filename, rest, weights)` читає решту файлу до першого масиву ваг (усю нейромережу файлів dxxwjz3 та їхніх проектів, яка зберігає ваги) і повідомляє, чи містить файл ваги (лише файли dxxwjz2 і dxxwjz3 та їхні проекти: решта форматів не зберігає опису нейромережі, який можна було б прочитати).
- **`IWeightsWriter`** ([`IWeightsWriter.h`](/src/NetworksLoaders/IWeightsWriter.h)) - метод `save(net, filename, cutoff, save_ema)` зберігає ваги шарів до межі відсікання (`save_weights_upto()` оригінального Darknet: поточну версію, значення seen поточної ітерації, масиви EMA згорткових шарів для `save_ema`), а метод `save(net, filename, cutoff, save_ema, rest)` зберігає поряд з ними опис нейромережі і запис навчання з решти, які формати лише ваг пропускають. Метод `write(document, filename)` записує документ.
- **`IWeightsFileFormat`** ([`IWeightsFileFormat.h`](/src/NetworksLoaders/IWeightsFileFormat.h)) - сам формат: його назва (та, що в командному рядку), розширення назв його файлів і опис, чи зберігають його файли структуру нейромережі (шари і назви масивів, тож файли читаються без нейромережі) і опис нейромережі, класи читання і запису його файлів. `WeightsFileFormats` директорії [`weights-formats`](/src/NetworksLoaders/weights-formats) знаходить формати за їхніми назвами і розширеннями назв файлів (будь-яке розширення, окрім `.dxxwjz1`, `.dxxwjz2`, `.dxxwjz2p`, `.dxxwjz3` і `.dxxwjz3p`, - це оригінальний файл ваг) і формати конвертування: усі формати і теку YOLO4 (його метод `convertible()`), а `NetworkSource` читає опис нейромережі з файлу, з якого вона будується (текст файлу cfg або опис, який зберігають файли dxxwjz2 і dxxwjz3 та їхні проекти), і ключі секції `[net]` цього опису.

Класи читання і запису форматів однаково завантажують і зберігають ті самі масиви нейромережі, тож будь-який файл однієї нейромережі завантажує ту саму нейромережу. Саме читання документів методом `read()` і їхній запис методом `write()` конвертують файли між форматами.

## Оригінальний файл ваг darknet

Файл складається з трійки чисел версії int32 `major`, `minor` і `revision`, кількості зображень, які бачила нейромережа (uint64 `seen` для версій `(major * 10 + minor) >= 2` і uint32 для старіших версій), і масивів float шарів з вагами, що йдуть один за одним у порядку шарів і таблиці нижче. Власної структури файл не має: яке значення float належить якому масиву, визначає лише нейромережа.

- **`OrigWeightsReader`** ([`OrigWeightsReader.h`](/src/NetworksLoaders/original-weights/OrigWeightsReader.h)) - перенесені на C++ оригінальні функції `load_weights()`, `load_weights_upto()` і процедури окремих шарів, що читають рівно ті самі байти, що й оригінал: пропускають шари з `dontload` і масиви нормалізації батчів шарів з `dontloadscales`, транспонують ваги згорткових шарів з `flipped` і ваги повнозв'язних шарів у файлах, старший чи молодший номер версії яких перевищує 1000, заповнюють масиви всіма повними значеннями float, які містить обрізаний файл, і зупиняються після шару, читання якого виходить за кінець файлу. Метод `read()` цього класу розбиває файл на масиви нейромережі, якій належить файл (нейромережа обов'язкова).
- **`OrigWeightsWriter`** ([`OrigWeightsWriter.h`](/src/NetworksLoaders/original-weights/OrigWeightsWriter.h)) - перенесені на C++ оригінальні функції `save_weights()`, `save_weights_upto()` і процедури окремих шарів, байт у байт сумісні з оригінальними файлами. Метод `write()` цього класу записує заголовок, кожен масив документа і його хвостові байти, тобто відтворює файл, з якого документ було прочитано.
- **`OrigWeightsValidator`** ([`OrigWeightsValidator.h`](/src/NetworksLoaders/original-weights/OrigWeightsValidator.h)) - порівнює розмір файлу з масивами нейромережі: з масивами, які записує `save_weights()` (файл, який зберігає darknet), і з масивами, які читає `load_weights()` і серед яких немає масивів шарів з `dontload` і `dontloadscales` (файл, створений для завантаження такої нейромережі). Файл, розмір якого не відповідає жодному з них, обрізаний або містить зайві байти, про що валідатор повідомляє попередженнями, тоді як стиснений gzip файл (файл dxxwjz1, переданий як оригінальний) і файл, коротший за заголовок, є помилками. Метод `validate(document, net)` цього класу перевіряє документ, який має бути записаний як оригінальний файл ваг нейромережі: такий файл не зберігає назв своїх масивів, тож масиви документа мають бути масивами нейромережі, починаючи з першого, і всі вони, крім останнього (обрізаний файл), мають бути повними. Документ, у якому бракує масиву посередині або за коротшим масивом ідуть інші (наприклад, відредагований вручну файл dxxwjz1), є помилкою, оскільки завантажувачі прочитали б масиви після пропуску не в ті масиви.

## Формат файлу dxxwjz1 першої версії

Файл dxxwjz1 - це стиснений gzip ([RFC 1952](https://www.rfc-editor.org/rfc/rfc1952)) документ [JSON](https://www.rfc-editor.org/rfc/rfc8259) у кодуванні UTF-8 з вагами нейромережі. Його розпаковують стандартні інструменти gzip (`gzip -dc model.dxxwjz1`), а файл з нестисненим текстом JSON теж читається, але з попередженням. Документ нейромережі зі згортковим шаром, шаром maxpool і рекурентним шаром:

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

Ключі документа:

| Ключ | Значення | Обов'язковий |
| --- | --- | --- |
| `format` | рядок `"dxxwjz1"` | так |
| `version` | ціле число `1`, інші версії під час читання відхиляються | так |
| `generator` | програма, яка записала файл, лише для інформації | ні |
| `header` | заголовок оригінального файлу ваг: числа int32 `major`, `minor` і `revision` та число uint64 `seen`, 32-бітне для версій `(major * 10 + minor) < 2` | так |
| `layers` | об'єкти шарів у порядку зростання їхніх індексів | так |
| `trailing` | текст base64 ([RFC 4648](https://www.rfc-editor.org/rfc/rfc4648)) байтів, які оригінальний файл ваг містить після масивів і які завантажувачі ігнорують | ні |

Ключі об'єкта шару: `index` (унікальний індекс шару в нейромережі), `type` (назва секції cfg шару без квадратних дужок: `convolutional`, `maxpool`, `yolo` та інші; `reorg3d` і `reorg` позначають два типи шарів reorg, а `activation` - шар `[activation]`) і, для шарів з вагами, об'єкт `arrays` з масивами шару або, для рекурентних шарів, об'єкт `sublayers` з їхніми підшарами, кожен з яких має власний об'єкт `arrays`. Масиви і підшари названо за полями шарів darknet:

| Тип шару | Підшари | Масиви |
| --- | --- | --- |
| `convolutional` | | `biases`, `scales`, `rolling_mean`, `rolling_variance`, `weights` |
| `connected` | | `biases`, `weights`, `scales`, `rolling_mean`, `rolling_variance` |
| `batchnorm` | | `biases`, `scales`, `rolling_mean`, `rolling_variance` |
| `shortcut`, `implicit` | | `weights` |
| `local` | | `biases`, `weights` |
| `rnn`, `crnn` | `input_layer`, `self_layer`, `output_layer` | масиви шару `connected` (`rnn`) або `convolutional` (`crnn`) |
| `gru` | `input_z_layer`, `input_r_layer`, `input_h_layer`, `state_z_layer`, `state_r_layer`, `state_h_layer` | масиви шару `connected` |
| `lstm` | `wf`, `wi`, `wg`, `wo`, `uf`, `ui`, `ug`, `uo` | масиви шару `connected` |
| `conv_lstm` | `vf`, `vi`, `vo` (лише для шарів з peephole), `wf`, `wi`, `wg`, `wo` (без `wi`, `wg` і `wo` у шарах з bottleneck), `uf`, `ui`, `ug`, `uo` | масиви шару `convolutional` |

Порядок у таблиці відповідає порядку масивів в оригінальному файлі ваг. Масиви нормалізації батчів (`scales`, `rolling_mean` і `rolling_variance`) мають лише шари з параметром `batch_normalize=1`, згорткові шари, які спільно використовують ваги іншого шару, масивів не мають, а шари shortcut мають масив `weights` лише з параметром `weights_type`. Масиви записуються саме в цьому порядку, а під час читання ключі приймаються в будь-якому порядку.

Значеннями масиву є:

- числа JSON для скінченних значень float: найкоротший десятковий запис, з якого значення float відновлюється без змін (`0.1`, `-2.5e-05`, `1e-45` для найменшого денормалізованого числа), і `-0.0` для від'ємного нуля (`-0` у JSON є цілим нулем). Під час читання приймається будь-яке число JSON з діапазону float, яке читається як найближче до нього значення float;
- рядки JSON з `"0x"` і 8 шістнадцяткових цифр бітового подання значення float для нескінченностей і NaN (`"0x7f800000"`, `"0xffc12345"`), що зберігає знак і корисне навантаження (payload) значень NaN. Під час читання приймається рядок бітового подання будь-якого значення float.

Тож кожне значення float файлу зберігається біт у біт.

Файл завантажується так само, як оригінальний файл ваг, з урахуванням версії заголовка: встановлюються `seen` і поточна ітерація нейромережі, ваги повнозв'язних шарів транспонуються для старшого чи молодшого номера версії понад 1000, шари з `dontload` і масиви нормалізації батчів шарів з `dontloadscales` не завантажуються, ваги згорткових шарів з `flipped` транспонуються. На відміну від позицій в оригінальному файлі, масиви знаходяться за їхніми шарами і назвами, тож пропущені масиви одного шару не зсувають масивів інших шарів. Масив, коротший за масив нейромережі, заповнює його початок, а відсутній масив залишає значення нейромережі без змін (документ обрізаного файлу чи файлу, збереженого до певного шару), про що валідатор повідомляє попередженнями. Невідомі ключі під час читання пропускаються з попередженням, тож наступні версії можуть додавати ключі, які перша версія ігнорує.

Допоміжні класи директорії [`dxxwjz/dxxwjz1`](/src/NetworksLoaders/dxxwjz/dxxwjz1):

- **`Dxxwjz1Format`** - константи формату: назва, версія, розширення імені файлу і ключі JSON.
- **`Dxxwjz1FloatCodec`** - описаний вище текст JSON значень float (функції C++17 `std::to_chars()` і `std::from_chars()`, які не залежать від локалі).
- **`Dxxwjz1Base64`** - текст base64 хвостових байтів.
- **`Dxxwjz1JsonString`** - текст рядка JSON для будь-якого тексту: байти, які не є UTF-8, замінюються символами U+FFFD.
- **`Dxxwjz1JsonEmitter`** - записує текст JSON документа у потік пошарово, розміщуючи кожен шар і кожен масив в окремому рядку, безпосередньо з масивів нейромережі чи документа. Класи запису наступних версій записують свої члени через його метод `sections()`, а свої хвостові байти - через його метод `trailing()`; класи dxxwjz3 записують свій документ через його методи `begin()`, `layer()` і `end()` з масивами шарів його методу `arrays()`.
- **`Dxxwjz1SaxHandler`** - будує документ з подій SAX-аналізатора [nlohmann json](https://github.com/nlohmann/json) по одному значенню, тож дерево JSON з мільйонів значень float не будується, і попутно перевіряє структуру документа: синтаксис JSON, формат і версію, типи значень, назви типів шарів, підшарів і масивів, повторювані ключі. Обробники наступних версій задають свої назву формату, версію й обов'язкові ключі і приймають значення своїх ключів документа, включно з ключем trailing версії dxxwjz3, тоді як обробники dxxwjz3 пропускають ключ layers як невідомий (їхній метод `layers_member()`, див. формати dxxwjz2 і dxxwjz3 нижче).
- **`Dxxwjz1Parser`** - читає текст JSON у документ обробником. Розбір виконується в числовій локалі "C" свого потоку, тож документ читається однаково, хай у якій локалі працює програма (включно з локалями, де десятковим роздільником є кома чи багатобайтовий символ), а локаль програми та її інших потоків залишається незмінною.
- **`Dxxwjz1Reader`**, **`Dxxwjz1Writer`** - класи читання і запису (див. вище). Клас запису стискає текст у файл у міру запису з найшвидшим рівнем стиснення zlib, якщо його конструктору не передано інший рівень: типовий рівень zlib потребує приблизно вшестеро більше часу і дає файли, менші приблизно на 12%, а найкращий - приблизно втридцятеро більше часу для файлів, менших приблизно на 14%, тоді як під час читання приймається файл будь-якого рівня стиснення.
- **`Dxxwjz1Validator`** - валідатор файлів (див. нижче).
- **`Dxxwjz1FileFormat`** - `IWeightsFileFormat` файлів dxxwjz1.

Файли стискаються і розпаковуються контролером `zlibi::ZlibController` (див. секцію [Допоміжні компоненти](/doc/sections/uk_UA/4-7-3-supporting-components.md)) частинами, тож у пам'яті не тримається ані весь файл, ані весь текст JSON. Текст займає близько 12 байтів на кожне значення float, а стиснений файл приблизно в 1,35 раза більший за оригінальний файл ваг (приблизно в 1,2 раза після повторного стиснення з найкращим рівнем, `gzip -dc model.dxxwjz1 | gzip -9 > smaller.dxxwjz1`).

## Формат файлу dxxwjz2 другої версії

Файл dxxwjz2 - це документ dxxwjz1 (див. вище) з усім, з чим нейромережу навчають і з чим вона розпізнає: описом нейромережі і записом навчання поряд з вагами. Документ нейромережі двох класів, навченої за три ітерації:

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

Ключі документа, окрім ключів dxxwjz1:

| Ключ | Значення | Обов'язковий |
| --- | --- | --- |
| `format` | рядок `"dxxwjz2"` | так |
| `version` | ціле число `2`, інші версії відхиляються | так |
| `network` | опис нейромережі: текст `cfg` файлу cfg нейромережі (обов'язковий, непорожній) і масив `classes` об'єктів класів з цілим числом `index` і рядком `name`, кожен індекс від нуля - один раз, у будь-якому порядку | так |
| `training` | запис навчання: кількість секунд `seconds`, які зайняло навчання, і цілі числа `iteration`, значення float `loss` і `avg_loss` кожної ітерації навчання у порядку навчання, три масиви однакового розміру; цілі числа `map_iteration` і значення float `map` кожного обчислення mAP (їх записують лише навчання з `--map`), обидва масиви разом, однакового розміру і зі зростаючими ітераціями | ні |
| `header`, `layers`, `trailing` | ваги, ключі dxxwjz1 (див. вище); відсутній `header` - це заголовок поточної версії з нульовим seen | ні |

Тож файл cfg нейромережі можна відновити з файлу будь-яким інструментом для роботи з JSON (`gzip -dc net.dxxwjz2 | jq -r .network.cfg > net.cfg`). Текст JSON - це UTF-8, тож байти файлу cfg чи назв класів, які не є UTF-8 (наприклад, коментар файлу cfg у Latin-1), зберігаються як символи заміни U+FFFD. Втрати - це значення float масивів dxxwjz1: найкоротший десятковий текст, з якого відновлюється значення float, або рядок бітового шаблону `"0x"` для нескінченних значень і NaN (втрата NaN навчання, що розійшлося).

**Файл без ваг - це нейромережа, яку треба навчити.** Файл dxxwjz2 без ваг (без жодного значення float у його масивах) не завантажує жодних ваг, нейромережа зберігає свою ініціалізацію і немає жодного попередження про масиви нейромережі, яких немає у файлі: такий файл навчає навчання, а розпізнавання його відхиляє (див. секцію [Навчання і формати](#навчання-і-формати)). Файл з самим лише `network` (без шарів: той, який конвертер створює з файлу cfg, див. [конвертування](#конвертування), або експортує інструмент) - це нейромережа, яку треба навчити з нуля: не продовжуються ні значення seen з його заголовка, ні його запис навчання, а класи запису записують його без членів `header` і `layers`.

Класи читання перевіряють члени network і training так само, як перевіряються члени dxxwjz1 (типи значень, індекси класів, розміри стовпців, повторювані ключі), пропускають їхні невідомі ключі з попередженням і попереджають про кількість назв класів, відмінну від `classes=` нейромережі. Класи версії dxxwjz2 побудовані на класах dxxwjz1, а директорія [`dxxwjz/dxxwjz2`](/src/NetworksLoaders/dxxwjz/dxxwjz2) містить те, що вони додають:

- **`Dxxwjz2Format`** - константи формату: назва, версія, розширення назв файлів і ключі JSON, які він додає.
- **`Dxxwjz2JsonEmitter`** - `Dxxwjz1JsonEmitter` з членами network і training, які він записує між generator і header (його метод `sections()`): клас у рядку і стовпець втрат у рядку, член cfg нейромережі - через свій метод `cfg()`, а стовпці втрат - через свій метод `losses()`.
- **`Dxxwjz2SaxHandler`** - `Dxxwjz1SaxHandler` з назвою, версією і обов'язковими ключами формату dxxwjz2, який передає значення ключів network і training класам `Dxxwjz2SubtreeBuilder` (їхні дерева JSON, малі порівняно з вагами, значення float - точні значення їхніх текстів) і `Dxxwjz2MetadataReader` (`NetworkDescription` і `TrainingRecord` з цих дерев). Його попереднє читання (peek) зупиняється на першій вазі, щойно прочитано network, тож опис нейромережі файлу читається без його ваг. Обробники наступних версій будують дерево network класом побудови свого методу `builder()`, читають його класом читання свого методу `metadata()`, а хвостові байти - обробником свого методу `trailing()`.
- **`Dxxwjz2Parser`** - розбір файлів і текстів класом `Dxxwjz1Parser` з обробником dxxwjz2, попереднє читання файлів.
- **`Dxxwjz2Reader`**, **`Dxxwjz2Writer`** - `Dxxwjz1Reader` і `Dxxwjz1Writer` з перевіркою і записом dxxwjz2: опис нейромережі і запис навчання потрапляють у решту завантаження і беруться з решти збереження та з документа, що записується. Клас запису відхиляє решту без тексту cfg, бо файл зберігає опис нейромережі.
- **`Dxxwjz2Validator`** - валідатор файлів: перевірки `Dxxwjz1Validator`, текст cfg, назви класів нейромережі.
- **`Dxxwjz2FileFormat`** - `IWeightsFileFormat` файлів dxxwjz2.

## Формат файлу проекту dxxwjz2p

Файл dxxwjz2p - це документ dxxwjz2 (див. вище) проекту навчання YOLO4: опис нейромережі, ваги і запис навчання поряд з анотаціями зображень YOLO4, тобто все, що потрібно навчанню, окрім самих зображень. Тож проект - це або єдиний файл dxxwjz2p з абсолютними шляхами до зображень, або файл dxxwjz2p зі шляхами до зображень, відносними до нього, поряд з теками зображень з будь-якими назвами. Член annotations - це документ проекту [ImagesAnnotator](https://github.com/yuriysydor1991/ImagesAnnotator) (його структури 0.12.0) з ознакою перевірки (validation) зображень:

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

Ключі документа, окрім ключів dxxwjz2:

| Ключ | Значення | Обов'язковий |
| --- | --- | --- |
| `format` | рядок `"dxxwjz2p"` | так |
| `version` | ціле число `1`, інші версії під час читання відхиляються | так |
| `annotations` | анотації зображень: об'єкт `ImagesAnnotator` з версією структури `version` і масив `annotations` тек зображень | так |

Ключі анотацій:

| Об'єкт | Ключ | Значення | Обов'язковий |
| --- | --- | --- | --- |
| тека | `db` | об'єкт зі шляхом `path` до теки: відносним до теки файлу dxxwjz2p або абсолютним | так |
| | `annotations` | об'єкти зображень теки | так |
| зображення | `rel_path` | шлях до зображення відносно теки, непорожній | так |
| | `width`, `height` | розмір зображення в пікселях, цілі числа від нуля, за замовчуванням `0` (невідомий); зображення з рамками потребує обох | ні |
| | `validation` | `true` для зображення лише для обчислення mAP (список `valid=` darknet), за замовчуванням `false` для зображення навчання (список `train=`) | ні |
| | `annotations` | об'єкти рамок зображення, порожній масив для зображення без жодного об'єкта | так |
| | `scale_factor` | масштаб ImagesAnnotator, пропускається | ні |
| рамка | `name` | назва класу рамки, один з класів `network` | так |
| | `rect` | `x` і `y` верхнього лівого кута та ширина `width` і висота `height` рамки в пікселях зображення, числа JSON | так |
| | `scale_factor` | пропускається | ні |

У пам'яті рамки зберігаються так, як у YOLO4 (індекс класу, центр і розмір рамки в частках розміру зображення), а прямокутники (rect) файлу - це ті самі рамки в пікселях: найкоротший десятковий запис значення float пікселів, тож цілочислові прямокутники ImagesAnnotator читаються і записуються назад без змін. Класи запису групують зображення за теками їхніх шляхів, по зображенню в рядку: теки, вкладені в теку файлу, записуються відносно неї, а решта - абсолютними шляхами, тож теку проекту разом з її зображеннями можна переміщувати цілком. Якщо зображень для перевірки немає, mAP обчислюється на зображеннях навчання, як це робить darknet.

В усьому іншому файл dxxwjz2p - це файл dxxwjz2: розпізнавання і розпізнавач зображень працюють з нейромережею самого лише файлу dxxwjz2p, конвертер створює з нього файл ваг dxxwjz2 без анотацій (див. [конвертування](#конвертування)), а файл dxxwjz2p без ваг - це нейромережа, яку треба навчити. Класи читання перевіряють анотації так само, як перевіряються члени dxxwjz2: типи значень, обов'язкові ключі, назви класів нейромережі (анотації, прочитані раніше за нейромережу, чекають на неї), розмір зображень з рамками і довжину шляхів до зображень, яку допускають шляхи до міток darknet; невідомі ключі пропускаються з попередженням. Попереднє читання (peek) теж читає анотації, включно з тими, що йдуть після ваг (класи запису записують їх перед заголовком).

Класи версії dxxwjz2p побудовані на класах dxxwjz2, а директорія [`dxxwjz/dxxwjz2p`](/src/NetworksLoaders/dxxwjz/dxxwjz2p) містить те, що вони додають:

- **`Dxxwjz2pFormat`** - константи формату: назва, версія, розширення назв файлів, версія структури ImagesAnnotator і ключі JSON, які він додає.
- **`Dxxwjz2pJsonEmitter`** - `Dxxwjz2JsonEmitter` з членом annotations, який він записує після членів network і training класом **`Dxxwjz2pAnnotationsJson`** (ним же користуються проекти dxxwjz3p): теку зображень для кожного шляху db, зображення в рядку.
- **`Dxxwjz2pAnnotationsReader`** - `Dxxwjz2MetadataReader` для `Yolo4Annotations` з дерева annotations.
- **`Dxxwjz2pSaxHandler`** - `Dxxwjz2SaxHandler` з назвою, версією і обов'язковими ключами формату dxxwjz2p, який читає дерево annotations, щойно прочитано назви класів нейромережі. Його попереднє читання зупиняється на першій вазі, щойно прочитано і network, і annotations.
- **`Dxxwjz2pParser`** - розбір і попереднє читання файлів обробником dxxwjz2p з текою файлу.
- **`Dxxwjz2pReader`**, **`Dxxwjz2pWriter`** - `Dxxwjz2Reader` і `Dxxwjz2Writer` з анотаціями, які потрапляють у решту завантаження і беруться з решти збереження та з документа, що записується. Клас запису відхиляє рамки, яких файл не може зберегти: рамки класів без назви, рамки зображень без розміру і рамки з нескінченними значеннями чи NaN.
- **`Dxxwjz2pValidator`** - валідатор файлів: перевірки `Dxxwjz2Validator` для документа dxxwjz2p.
- **`Dxxwjz2pFileFormat`** - `IWeightsFileFormat` файлів dxxwjz2p.

## Формат файлу dxxwjz3 третьої версії

Файл dxxwjz3 - це документ dxxwjz2 (див. вище), який складається лише з даних JSON: член network зберігає структуру JSON cfg нейромережі замість тексту файлу cfg, кожен його шар зберігає масиви шару, а хвостові байти - це масив JSON їхніх значень замість тексту base64, тож будь-який інструмент для роботи з JSON читає і редагує кожну опцію нейромережі (`gzip -dc net.dxxwjz3 | jq .network.net.batch`). Документ нейромережі з двома класами:

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

Ключі документа, окрім ключів dxxwjz2 (header - це ключ dxxwjz2, файл без header не містить ваг; ключа layers немає, масиви зберігають шари нейромережі):

| Ключ | Значення | Обов'язковий |
| --- | --- | --- |
| `format` | рядок `"dxxwjz3"` | так |
| `version` | ціле число `3`, інші версії відхиляються | так |
| `network` | опис нейромережі: об'єкт `net` опцій секції `[net]` cfg (секції `[network]`), масив `layers` секцій шарів у порядку cfg з масивами їхніх шарів, обидва обов'язкові, і масив `classes` рядків назв класів, назва з індексом - це назва класу з цим індексом | так |
| `training` | запис навчання dxxwjz2 без стовпця `iteration`: втрати індексу - це втрати ітерації індекс + 1, тож `loss` 1000-ї ітерації - це `loss[999]` (втрати NaN ітерацій без запису) | ні |
| `trailing` | байти, які оригінальний файл ваг містить після масивів: масив цілих чисел від 0 до 255 | ні |

Шар нейромережі - це об'єкт з рядком `type`, назвою секції cfg без дужок, як її називає cfg (`conv` секції `[conv]`, тоді як шари ваг зберігають назви типів шарів darknet), і з об'єктом `options` опцій секції, якого немає для секції без опцій. Шар з вагами зберігає їх в об'єкті `arrays` шарів dxxwjz1 (в об'єкті `sublayers` для рекурентних шарів, див. секцію [Формат файлу dxxwjz1 першої версії](#формат-файлу-dxxwjz1-першої-версії)): масиви шару з індексом об'єкта в масиві `layers` і з типом шару, на який darknet перетворює секцію `type` (секції `[conv]` і `[convolutional]` - це згорткові шари, див. `WeightsSchema::section_type()`). Об'єкт шару без масивів не містить ваг: це шари без ваг (maxpool, route, yolo), кожен шар нейромережі, яку треба навчити, і шари поза межею відсікання збереження. Тож під час читання нейромережа і її ваги читаються з тих самих об'єктів, без індексів і типів шарів, які треба зіставляти, а класи запису відхиляють шар, який не є шаром секції cfg свого індексу (якої немає або яка має інший тип). Ключ документа `layers` (ключ документів dxxwjz1 і dxxwjz2) - це невідомий ключ, який під час читання пропускається з попередженням. Опції - це рядки `key=value` секції у їхньому порядку, а значення - це значення JSON значень darknet:

| Опція cfg | Значення JSON |
| --- | --- |
| `batch=64`, `layers=-1` | ціле число: `64`, `-1` (у межах 64 бітів) |
| `hue=.1`, `decay=0.0005`, `exposure=1.50` | дробове число найкоротшого десяткового запису з фіксованою комою його значення float (darknet зберігає значення опцій як float): `0.1`, `0.0005`, `1.5` |
| `activation=leaky`, `train=data/train.txt` | рядок: `"leaky"`, `"data/train.txt"` |
| `mask=0,1,2`, `scales=.1,.1` | масив значень, розділених комами: `[0,1,2]`, `[0.1,0.1]` |
| `random` (рядок без `=`) | `null`, значення NULL darknet, для якого береться значення опції за замовчуванням |

Число - це число того тексту, який darknet читає однаково: `atoi()` і `atof()` тексту і числа збігаються, тож `1e-3` залишається рядком `"1e-3"` (його `atoi()` дорівнює 1, а `atoi()` числа `0.001` - 0), так само як рядками залишаються `0x10` і `nan`. Під час читання приймаються числа JSON будь-якого запису (`"learning_rate": 1e-3`, відредагований вручну, - це `0.001`) і будь-яке значення, яке записують класи запису, а помилками є порожні значення, рядки з пробільними символами (darknet видаляє їх з рядків cfg), булеві значення, об'єкти і масиви чогось, окрім чисел і рядків, а також ключі опцій з пробільними символами, з `=` чи з `#`, `;` або `[` на початку і назви секцій з пробільними символами чи дужками.

Під час читання член network перетворюється на текст cfg нейромережі: секцію `[net]` з опцій `net`, секцію кожного шару, рядок `key=value` кожної опції (сам ключ для `null`), а секції розділяються порожнім рядком. Тож решта darknetxx працює з текстом cfg файлу dxxwjz3 так само, як з текстом файлу dxxwjz2, а файл cfg відновлюється з файлу конвертером: прапорець `--dump-cfg` друкує його текст cfg (див. секцію [Друк cfg нейромережі файлу](/doc/sections/uk_UA/6-2-4-converting-the-network-weights-files.md#друк-cfg-нейромережі-файлу)), а файл dxxwjz3, конвертований у файл dxxwjz2, зберігає його. JSON не зберігає ні коментарів, ні пробільних символів файлу cfg, ні повторюваних ключів секції, з яких darknet читає лише перший: текст cfg, конвертований у файл dxxwjz3 і назад, - це та сама нейромережа з нормалізованим текстом cfg, яка знову конвертується у той самий файл dxxwjz3. Класи запису відхиляють текст cfg, якого JSON не може зберегти: без секції `[net]` першою, з опцією перед нею, з заголовком секції без закривальної дужки (такий cfg відхиляє і darknet).

Під час читання дерево члена network будується пошарово: масиви об'єкта шару переходять у шари документа, щойно об'єкт завершено, тож дерево JSON з вагами всієї нейромережі не будується. Член network зберігає і опис нейромережі, і ваги, тож попереднє читання (peek) файлу читає увесь член: воно триває приблизно стільки, скільки читання всього файлу (нейромережа з 9 мільйонів значень float - близько 2 секунд у збірці release), тоді як попереднє читання файлу dxxwjz2 зупиняється на його першій вазі.

В усьому іншому файл dxxwjz3 - це файл dxxwjz2: навчання і розпізнавання його читають, навчання його зберігає (за замовчуванням, `--save-format dxxwjz3`), конвертер конвертує його в інші формати і назад з вагами тих самих байтів, а файл без ваг - це нейромережа, яку треба навчити. Класи версії dxxwjz3 побудовані на класах dxxwjz2, а директорія [`dxxwjz/dxxwjz3`](/src/NetworksLoaders/dxxwjz/dxxwjz3) містить те, що вони додають:

- **`Dxxwjz3Format`** - константи формату: назва, версія, розширення назв файлів і ключі JSON, які він додає.
- **`Dxxwjz3ValueCodec`** - значення JSON опцій cfg з таблиці вище в обидва боки: текст JSON тексту опції і текст cfg значення JSON.
- **`Dxxwjz3CfgJson`** - текст JSON тексту cfg, який він читає так, як darknet (методом `CfgSection::read_cfg()` класу `OrigDefaultLoader`): члени опцій `[net]`, члени `type` і `options` кожної секції шару і типи шарів секцій.
- **`Dxxwjz3Bytes`** - масив JSON хвостових байтів: текст байтів і байти з подій SAX масиву по одному значенню (дерево JSON з мегабайтів байтів оригінального файлу ваг іншої нейромережі не будується).
- **`Dxxwjz3Losses`** - стовпці втрат з втратами ітерації за індексом ітерація - 1, втратами NaN ітерацій без них.
- **`Dxxwjz3JsonEmitter`** - `Dxxwjz2JsonEmitter` з членом network з JSON cfg нейромережі, який записує кожен об'єкт шару з масивами свого методу `layer()` (шари без нього - без масивів), а члени після network - своїм методом `members()` перед header, з масивом назв класів, з хвостовими байтами (його метод `trailing()`) і стовпцями втрат `Dxxwjz3Losses` (його метод `losses()`).
- **`Dxxwjz3MetadataReader`** - `Dxxwjz2MetadataReader` тексту cfg з дерева network, яке він перевіряє так само, як перевіряються решта членів, масивів об'єкта шару (його метод `arrays()`, який переносить їх з дерева у шар документа з індексом об'єкта і типом його секції), масиву назв класів і дерева training без стовпця ітерацій.
- **`Dxxwjz3NetworkBuilder`** - `Dxxwjz2SubtreeBuilder` значення network, який передає кожен об'єкт шару методу `arrays()` класу `Dxxwjz3MetadataReader`, щойно об'єкт завершено (його метод `closed()`), тож дерево зберігає ваги лише одного шару.
- **`Dxxwjz3SaxHandler`** - `Dxxwjz2SaxHandler` з назвою і версією формату dxxwjz3, який будує дерево network класом `Dxxwjz3NetworkBuilder` (його метод `builder()`), читає його класом `Dxxwjz3MetadataReader` (його метод `metadata()`), хвостові байти - класом `Dxxwjz3Bytes` (його метод `trailing()`) і пропускає ключ документа layers (його метод `layers_member()`).
- **`Dxxwjz3Parser`**, **`Dxxwjz3Reader`**, **`Dxxwjz3Writer`**, **`Dxxwjz3Validator`**, **`Dxxwjz3FileFormat`** - відповідні класи dxxwjz2 з обробником і класом запису JSON dxxwjz3; клас запису відхиляє текст cfg, якого JSON не може зберегти, і шари, які не є шарами секцій cfg своїх індексів (його метод `keeps()`).

## Формат файлу проекту dxxwjz3p

Файл dxxwjz3p - це проект dxxwjz2p (див. вище) з документом dxxwjz3, тож увесь проект навчання складається лише з даних JSON: член network і хвостові байти - це члени dxxwjz3, а член annotations - член dxxwjz2p.

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

| Ключ | Значення | Обов'язковий |
| --- | --- | --- |
| `format` | рядок `"dxxwjz3p"` | так |
| `version` | ціле число `1`, інші версії відхиляються | так |
| `network`, `training`, `trailing` | члени dxxwjz3 | так, ні, ні |
| `annotations` | член dxxwjz2p | так |

В усьому іншому файл dxxwjz3p - це файл dxxwjz2p: навчання `--train-cfg project.dxxwjz3p` навчає зображення його анотацій і оновлює проект, графічний інтерфейс навчання його відкриває, розпізнавання працює з його нейромережею, конвертер конвертує його у проект dxxwjz2p і назад з тими самими вагами й анотаціями та у файл ваг dxxwjz3 без анотацій. Класи директорії [`dxxwjz/dxxwjz3p`](/src/NetworksLoaders/dxxwjz/dxxwjz3p) - це класи dxxwjz2p з членом network і хвостовими байтами dxxwjz3 (класами `Dxxwjz3NetworkBuilder`, `Dxxwjz3MetadataReader` і `Dxxwjz3Bytes` версії dxxwjz3): `Dxxwjz3pFormat`, `Dxxwjz3pJsonEmitter` (`Dxxwjz3JsonEmitter` з членом annotations класу `Dxxwjz2pAnnotationsJson`), `Dxxwjz3pSaxHandler`, `Dxxwjz3pParser`, `Dxxwjz3pReader`, `Dxxwjz3pWriter` (який теж відхиляє нейромережу, якої файл dxxwjz3 не може зберегти), `Dxxwjz3pValidator` і `Dxxwjz3pFileFormat`.

## Тека навчання YOLO4

Тека навчання YOLO4, та, яку експортує ImagesAnnotator, - це формат `yolo4`, який використовує лише конвертування: формат без розширення назви файлу, тож ні навчання, ні розпізнавання не приймають теку як файл ваг. `Yolo4FolderReader` директорії [`yolo4-folder`](/src/NetworksLoaders/yolo4-folder) читає теку в документ проекту dxxwjz2p так, як її читає навчання, `Yolo4FolderWriter` записує документ у нову теку, а `Yolo4FolderLayout` знає файли теки:

| Файли | Клас читання | Клас запису |
| --- | --- | --- |
| `cfg/*.cfg` | нейромережа: перший за назвою файл `.cfg` | `cfg/<name>.cfg` з назвою теки до її першої крапки: текст cfg з ключами train=, valid=, names= і backup= секції `[net]`, які вказують на файли теки |
| `data/obj.names` | назви класів, які мають перевагу над назвами класів ваг, а якщо немає ні тих, ні інших - назви з файлу `names=` з cfg | назви класів нейромережі |
| `data/obj.data` | не читається, навчання такого файлу не читає | такий, як в оригінальному darknet |
| списки `train=` і `valid=` з cfg (за замовчуванням `data/train.txt` і список навчання) | зображення навчання і обчислення mAP, відносні до теки або абсолютні | `data/train.txt` і `data/val.txt` зі зображеннями для перевірки; якщо таких немає, списком перевірки є список навчання |
| зображення | розмір зображень: із заголовка для форматів stb_image, за допомогою OpenCV для решти | копії в теці `data/obj/` з власними назвами: суфікс `_<n>` для однакових основ назв, включно з файловими системами, нечутливими до регістру |
| мітки (шляхи до зображень, перетворені функцією darknet `replace_image_to_label()`) | рамки, жодної для зображення без файлу міток | `data/obj/<stem>.txt` |
| тека резервних копій, яку називає ключ `backup=` з cfg | найновіші ваги, з яких продовжується навчання (оригінальний файл ваг читається для нейромережі з cfg теки) | `backup/<name>_last.dxxwjz3` з вагами документа і його записом навчання |

Тека, конвертована у файл dxxwjz2p і назад, має ті самі списки, мітки, назви класів і ваги. Конвертер не записує теку в теку з файлами (тека призначення конвертера замінює лише порожню теку), тож жодна тека користувача не видаляється.

## Незалежна від формату модель

- **`WeightsSchema`** ([`WeightsSchema.h`](/src/NetworksLoaders/weights-model/WeightsSchema.h)) - масиви і підшари кожного типу шарів, їхній порядок і назви (таблиця вище).
- **`WeightsLayout`** ([`WeightsLayout.h`](/src/NetworksLoaders/weights-model/WeightsLayout.h)) - масиви нейромережі у порядку файлів: масиви, які записує збереження (масиви EMA для збереження EMA), і масиви, які читає завантаження, з їхніми транспонуваннями, до певного шару.
- **`WeightsDocument`** ([`WeightsDocument.h`](/src/NetworksLoaders/weights-model/WeightsDocument.h)) - вміст файлу ваг: заголовок, шари з їхніми масивами і байти після масивів. Масиви у своєму порядку становлять корисні дані оригінального файлу ваг, тож документ файлу зберігає кожен його байт, включно з обрізаними масивами, частиною значення float, якою закінчується обрізаний файл, і зайвими байтами.
- **`NetworkDescription`** ([`NetworkDescription.h`](/src/NetworksLoaders/weights-model/NetworkDescription.h)) і **`TrainingRecord`** ([`TrainingRecord.h`](/src/NetworksLoaders/weights-model/TrainingRecord.h)) - нейромережа і навчання документа, які зберігають файли dxxwjz2 і dxxwjz3: текст cfg і назви класів, секунди, втрати ітерацій і mAP обчислень mAP.
- **`Yolo4Annotations`** ([`Yolo4Annotations.h`](/src/NetworksLoaders/weights-model/Yolo4Annotations.h)), **`Yolo4Image`** ([`Yolo4Image.h`](/src/NetworksLoaders/weights-model/Yolo4Image.h)) і **`Yolo4Box`** ([`Yolo4Box.h`](/src/NetworksLoaders/weights-model/Yolo4Box.h)) - анотації документа, які зберігають файли dxxwjz2p і dxxwjz3p: зображення з їхніми абсолютними шляхами, розмірами, рамками і наборами (навчання чи перевірки), рамки файлів міток YOLO4 (індекс класу, центр і розмір у частках розміру зображення) і шлях darknet до файлу міток зображення (`Yolo4Image::label_of()`).
- **`NetworkFiles`** ([`NetworkFiles.h`](/src/NetworksLoaders/weights-model/NetworkFiles.h)) - читає текст файлу cfg і назви з файлу назв класів darknet (назва в рядку, включно з закінченнями рядків CR LF).
- **`WeightsDocumentValidator`** ([`WeightsDocumentValidator.h`](/src/NetworksLoaders/weights-model/WeightsDocumentValidator.h)) - перевіряє документ окремо і на відповідність нейромережі, записуючи результати у `WeightsReport`.

Валідатори повідомляють про помилки, через які файл неможливо завантажити чи конвертувати, і попереджають про файли, які завантажуються, але не так, як могли очікувати їхні автори:

| Валідатор | Помилки | Попередження |
| --- | --- | --- |
| `OrigWeightsValidator` | файл, який неможливо прочитати, коротший за заголовок або стиснений gzip; для документа, що записується як оригінальний файл ваг нейромережі: масив, якого бракує посередині, коротший масив, за яким ідуть інші, масив, який не належить жодному шару нейромережі | обрізаний файл, частина значення float у кінці, зайві байти, файл, створений для завантаження шарів з `dontload` і `dontloadscales` |
| `Dxxwjz1Validator` | файл, який неможливо прочитати, пошкоджені чи обрізані дані gzip, синтаксичні помилки JSON, інший формат чи версія, хибна структура, документи, які відхиляє `WeightsDocumentValidator` | нестиснений файл JSON, невідомі ключі, попередження `WeightsDocumentValidator` |
| `Dxxwjz2Validator` | помилки `Dxxwjz1Validator`, документ без тексту cfg, хибні члени network чи training | попередження `Dxxwjz1Validator` (окрім масивів нейромережі для файлу без ваг), кількість назв класів, відмінна від класів нейромережі |
| `Dxxwjz2pValidator` | помилки `Dxxwjz2Validator`, хибні анотації, рамки класів без назви і рамки зображень без розміру | попередження `Dxxwjz2Validator`, невідомі ключі анотацій |
| `Dxxwjz3Validator` | помилки `Dxxwjz2Validator` для документа dxxwjz3, хибні cfg нейромережі (члени `net` і `layers`, ключі і значення опцій), масиви шарів (назви масивів і підшарів, значення float, типи шарів масивів) і хвостові байти | попередження `Dxxwjz2Validator`, невідомі ключі нейромережі та її шарів, ключ документа layers |
| `Dxxwjz3pValidator` | помилки `Dxxwjz3Validator` і `Dxxwjz2pValidator` | попередження `Dxxwjz3Validator` і `Dxxwjz2pValidator` |
| `WeightsDocumentValidator` | 32-бітне поле seen для значення seen понад 32 біти, повторювані шари, масиви, яких немає у шарів їхнього типу; для нейромережі: шари поза межами її шарів або інших типів, масиви, яких у неї немає, масиви, довші за її масиви | масиви, коротші за масиви нейромережі, масиви нейромережі, яких немає в документі |

## Конвертування

Клас **`WeightsConverter`** ([`WeightsConverter.h`](/src/NetworksLoaders/weights-converter/WeightsConverter.h)) виконує конвертування **`WeightsConversion`** ([`WeightsConversion.h`](/src/NetworksLoaders/weights-converter/WeightsConversion.h)) файлу одного формату в інший (або в той самий): спершу розбирається нейромережа з файлу cfg (оригінальний файл ваг її потребує, а решта, якщо файл cfg вказано, перевіряється на відповідність їй), потім файл-джерело перевіряється і читається у документ, а документ записується у тимчасовий файл у директорії призначення, який після завершення запису замінює файл призначення. Невдале конвертування залишає файл призначення таким, яким він був, і не залишає по собі тимчасового файлу, а запит зупинки програми (натискання Ctrl+C) зупиняє конвертування між його кроками.

Оригінальний файл ваг розбивається на масиви нейромережі, які записує `save_weights()`, якщо тільки розмір файлу не відповідає масивам, які читає `load_weights()`. Оригінальний файл ваг, конвертований у файл dxxwjz1, dxxwjz2 чи dxxwjz3 і назад, байт у байт збігається з початковим, хай що він містить: заголовок будь-якої версії, обрізані масиви, зайві байти. Під час конвертування файлу в оригінальний файл ваг з файлом cfg клас `OrigWeightsValidator` перевіряє, що файл містить масиви нейромережі один за одним (див. вище), а без файлу cfg немає нейромережі, з якою можна звірити документ, тож його масиви записуються як є.

Файли призначення dxxwjz2 і dxxwjz3 зберігають опис нейромережі: текст cfg з файлу cfg конвертування (файлу cfg, dxxwjz2 чи dxxwjz3) замість тексту файлу-джерела - один з них обов'язковий, - і назви класів з файлу назв класів конвертування замість класів файлу-джерела, а ті - замість файлу `names=` з cfg нейромережі. Запис навчання файлу-джерела переходить у файли призначення dxxwjz2 і dxxwjz3, тоді як формати лише ваг не зберігають ні того, ні іншого.

Файли призначення dxxwjz2p і dxxwjz3p зберігають анотації джерела dxxwjz2p, dxxwjz3p чи теки-джерела YOLO4 і жодного іншого джерела: проект, конвертований у файл dxxwjz2 чи dxxwjz3, - це файл ваг проекту без анотацій, файл для розпізнавання. Тека-джерело YOLO4 - це нейромережа, найновіші ваги й анотації теки, а тека призначення YOLO4 - це нова тека з копіями зображень документа (див. секцію [Тека навчання YOLO4](#тека-навчання-yolo4)).

Конвертування без файлу-джерела створює з файлу cfg нейромережу, яку треба навчити: файл dxxwjz2 чи dxxwjz3 лише з описом нейромережі (текстом cfg і назвами класів, як описано вище), без шарів. Такий файл, конвертований у формати лише ваг, не зберігав би нічого, крім заголовка, тож таке конвертування не вдається, тоді як файли ваг нейромереж без масивів ваг конвертуються в обидва боки, як і решта.

## Навчання і формати

Навчання за замовчуванням зберігає свої ваги у файли dxxwjz3 (параметр командного рядка `--save-format` задає їхній формат, у бібліотеці - `LibraryContext::save_format`): файли `<base>_<ітерація>_<середня втрата>` (нумеровані, із середньою втратою (avg loss) їхньої ітерації, наприклад `net_1200_0.734521`), `<base>_last`, `<base>_final`, `<base>_best` і `<base>_ema` директорії резервних копій з розширенням формату. Код навчання мовою C зберігає їх через `weights_saver` свого контексту, який `DarknetContext` спрямовує до журналу навчання **`TrainingJournal`** ([`TrainingJournal.h`](/src/darknet-adaptor/adaptors/TrainingJournal/TrainingJournal.h)):

- журнал починається з опису нейромережі навчання: тексту файлу cfg нейромережі навчання (або файлу dxxwjz2) і назв класів з файлу `--names`, з файлу ваг dxxwjz2, з файлу нейромережі dxxwjz2 чи з файлу `names=` з cfg - з першого з них, що вказаний;
- він продовжує запис навчання файлу ваг dxxwjz2, з якого починається навчання, до ітерації цих ваг (запис ітерацій після неї відкидається, а `--clear` відкидає весь запис разом з ітераціями), а секунди його годинника починаються з секунд запису;
- він записує втрати кожної ітерації і mAP кожного обчислення mAP один раз, у зростаючому порядку ітерацій, включно з ітерацією, збереженою до звіту про неї, і зберігає кожен файл ваг з часом навчання на цей момент.

Тож файл навчання, продовженого з файлу dxxwjz2, зберігає втрати і mAP кожної ітерації навчання у їхньому порядку і весь час усіх навчань, а середні втрати продовженого навчання продовжують останні середні втрати запису. Графік кожного запуску навчання - це новий файл `chart_<base>_<РРРР-ММ-ДД--ГГ-ХХ-СС>.png` з датою і часом запуску, тож жоден графік продовжених раніше навчань не перезаписується. На ньому заново малюється весь запис продовжених ваг (середні втрати і лінія mAP), і графік одразу зберігається, далі - кожні 100 ітерацій (кожну ітерацію з `--save-charts-every-iter`), наприкінці і при зупинці навчання; зображення `--chart-path` продовжується лише для ваг без запису (оригінальних і dxxwjz1). Графік запису файлу dxxwjz2 можна також намалювати будь-коли (параметр командного рядка `--draw-chart`, клас `TrainingChart` і `LibraryFacade::training_chart()` бібліотеки - графік, який GUI показує одразу, щойно відкрито директорію навчання).

Обчислення mAP навчання (`--map`) бере назви класів так само, тож назвам класів файлу dxxwjz2 не потрібен файл назв; назви, кількість яких відрізняється від кількості класів нейромережі, зупиняють навчання з помилкою.

Розпізнавання і навчання читають файли ваг кожного формату, формат визначає розширення назви файлу, а файл нейромережі - це файл cfg або dxxwjz2 (файли dxxwjz3 і проекти dxxwjz3p читаються так само, як файли dxxwjz2 і проекти dxxwjz2p):

| Командний рядок | Нейромережа | Ваги |
| --- | --- | --- |
| `--train-cfg net.cfg --weights w` | з `net.cfg` | з файлу `w` будь-якого формату, запис навчання файлу dxxwjz2 продовжується |
| `--train-cfg net.dxxwjz2` | з cfg, який зберігає файл | з файлу, жодних для файлу без ваг (навчання з нуля) |
| `--detect-cfg net.cfg --weights w` | з `net.cfg` | з файлу `w` будь-якого формату, файл dxxwjz2 без ваг відхиляється |
| `--detect-cfg net.dxxwjz2` | з cfg, який зберігає файл | з файлу |
| лише `--weights net.dxxwjz2` | з cfg, який зберігає файл | розпізнавання з вагами файлу, навчання нейромережі файлу без ваг |
| `--train-cfg project.dxxwjz2p` | з cfg, який зберігає файл | з файлу, навчання на зображеннях анотацій файлу, яке оновлює цей файл (див. нижче) |
| `--detect-cfg project.dxxwjz2p`, лише `--weights project.dxxwjz2p` | з cfg, який зберігає файл | такі самі, як для файлу dxxwjz2 |

Резервні копії навчання названі за файлом його нейромережі: `<base>` файлів `net.cfg` і `net.dxxwjz2` - це `net`, і `<base>` резервних файлів навчання (файлів dxxwjz2 `net_last`, `net_final`, `net_best`, `net_ema` і `net_<ітерація>_<середня втрата>` з ітерацією їхніх ваг, а також `net_<ітерація>` попередніх версій) - теж `net`, тож навчання, продовжене з самого лише резервного файлу, продовжує ті самі файли.

## Навчання проекту dxxwjz2p

Проект dxxwjz3p навчається так само, з файлом dxxwjz3p його найновіших ваг. Навчання файлу dxxwjz2p (командний рядок `--train-cfg project.dxxwjz2p`, проект dxxwjz2p, який відкриває GUI навчання) навчає нейромережу файлу на зображеннях його анотацій, продовжуючи від його ваг: списки зображень і мітки навчання беруться з анотацій замість списків `train=` і `valid=` з cfg і файлів міток поряд із зображеннями. `OrigDefaultLoader` вказує їх нейромережам навчання як списки, а `DarknetContext` навчання публікує їх у `TrainingDataRegistry` директорії [`TrainingData`](/src/darknet-adaptor/adaptors/TrainingData) і відкликає наприкінці навчання. Код darknet мовою C читає їх функціями `get_paths()` (за назвами списків `dxxwjz2p:train` і `dxxwjz2p:valid`) і `read_boxes()` (за шляхами до міток зображень), які звертаються до реєстру раніше, ніж до файлів (функції C `get_training_data_paths()`, `has_training_data_paths()` і `read_training_data_boxes()` з `TrainingDataCApi`). Тож зображення залишаються там, де вони є, проект не зберігає їхніх копій і жоден файл списку чи міток не записується.

Кожне збереження навчання також оновлює проект: файл dxxwjz2p з найновішими вагами, записом навчання і анотаціями записується у файл `<project>.saving` поряд з проектом, який після завершення запису замінює проект. Тож навчання проекту продовжується від його найновіших ваг, тоді як тека резервних копій поряд з проектом, яку називає ключ `backup=` його cfg, зберігає резервні копії формату збереження, а графіки малюються поряд з проектом.

## Використання форматів

Код C++ завантажує і зберігає ваги нейромережі у будь-якому форматі через інтерфейси:

```
#include "src/NetworksLoaders/dxxwjz/dxxwjz2/Dxxwjz2Reader.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz2/Dxxwjz2Validator.h"
#include "src/NetworksLoaders/weights-formats/WeightsFileFormats.h"

using namespace darknet_adaptor::loaders;

// net - подання нейромережі ANetworkAdaptor (networkptr, який повертає його
// метод get()), решта (rest) тримає опис нейромережі і запис навчання
WeightsDocument rest;
rest.network = NetworkDescription{cfg_text, {"cat", "dog"}};

const IWeightsFileFormatPtr format = WeightsFileFormats::named("dxxwjz2");

format->create_writer()->save(*net, "backup.dxxwjz2", net->n, false, rest);
WeightsFileFormats::of_file("backup.dxxwjz2")
    ->create_reader()
    ->load(*net, "backup.dxxwjz2", net->n, rest);

// перевірка файлу на відповідність нейромережі перед завантаженням
const WeightsReport report =
    dxxwjz2::v2::Dxxwjz2Validator::validate("backup.dxxwjz2", net.get());
```
