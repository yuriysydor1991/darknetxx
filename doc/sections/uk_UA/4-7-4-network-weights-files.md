# Файли ваг нейромереж

`darknetxx` читає і записує ваги нейромереж у трьох форматах:

- **оригінальний файл ваг darknet** (`*.weights`) - бінарний файл, який читає і записує оригінальний Darknet;
- **файл dxxwjz1** (`*.dxxwjz1`, від англ. **d**arknet**xx** **w**eights **j**son **z**ipped - стиснений файл ваг darknetxx у форматі JSON, перша версія) - стиснений gzip документ JSON з тими самими вагами, який може прочитати як будь-який інструмент для роботи з JSON, так і людина;
- **файл dxxwjz2** (`*.dxxwjz2`, друга версія) - документ dxxwjz1 з усім, з чим нейромережу навчають і з чим вона розпізнає, окрім її ваг: текстом файлу cfg нейромережі, назвами класів з їхніми індексами, втратами (loss) кожної ітерації навчання у порядку навчання і часом, який зайняло навчання.

Навчання і розпізнавання `darknetxx` читають файли ваг кожного формату (параметр командного рядка `--weights`, формат файлу визначає розширення його назви), навчання за замовчуванням зберігає свої ваги у файли dxxwjz2 (див. секцію [Навчання і формати](#навчання-і-формати) нижче), а сам лише файл dxxwjz2 - це нейромережа, з якою працює розпізнавання.

Ваги форматів конвертуються один в одний без втрат: оригінальний файл ваг, конвертований у файл dxxwjz1 чи dxxwjz2 і назад, байт у байт збігається з початковим (див. секції [Конвертування файлів ваг нейромереж](/doc/sections/uk_UA/6-2-4-converting-the-network-weights-files.md) і [Бінарний файл darknetxxConverter](/doc/sections/uk_UA/5-35-the-darknetxxConverter-binary.md)), тоді як опис нейромережі і запис навчання зберігають лише файли dxxwjz2.

Компоненти файлів ваг розміщені у просторі імен `darknet_adaptor::loaders` у директорії [`src/lib/darknet-adaptor/adaptors/NetworksLoaders`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders):

| Директорія | Містить |
| --- | --- |
| [`NetworksLoaders`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders) | інтерфейси `IWeightsReader`, `IWeightsWriter` і `IWeightsFileFormat`, які реалізує кожен формат |
| [`weights-model`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model) | незалежну від формату модель ваг: масиви кожного виду шарів та їхній порядок (`WeightsSchema`), масиви нейромережі у цьому порядку (`WeightsLayout`), вміст файлу (`WeightsDocument` з `NetworkDescription` і `TrainingRecord`), читання файлів cfg і назв класів (`NetworkFiles`) і перевірку (`WeightsDocumentValidator`, `WeightsReport`) |
| [`original-weights`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/original-weights) | оригінальний файл ваг: `OrigWeightsReader`, `OrigWeightsWriter`, `OrigWeightsValidator` і `OrigWeightsFileFormat` |
| [`dxxwjz1/v1`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1) | файл dxxwjz1 першої версії: `Dxxwjz1Reader`, `Dxxwjz1Writer`, `Dxxwjz1Validator` і `Dxxwjz1FileFormat` з їхніми допоміжними класами (простір імен `dxxwjz1::v1`) |
| [`dxxwjz2/v2`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2) | файл dxxwjz2 другої версії: `Dxxwjz2Reader`, `Dxxwjz2Writer`, `Dxxwjz2Validator` і `Dxxwjz2FileFormat` з їхніми допоміжними класами (простір імен `dxxwjz2::v2`), які розширюють класи dxxwjz1 |
| [`weights-formats`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-formats) | формати за їхніми назвами і розширеннями назв файлів (`WeightsFileFormats`) і файли, з яких будуються нейромережі: файли cfg і dxxwjz2 (`NetworkSource`) |
| [`weights-converter`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-converter) | `WeightsConverter` - конвертер файлів між форматами - і конвертування `WeightsConversion`, яке він виконує |

Журнал навчання, `TrainingJournal` директорії [`TrainingJournal`](/src/lib/darknet-adaptor/adaptors/TrainingJournal), тримає опис нейромережі і запис навчання, які зберігають файли ваг навчання, а `TrainingChart` директорії [`TrainingChart`](/src/lib/darknet-adaptor/adaptors/TrainingChart) малює графік цього запису (див. секцію [Навчання і формати](#навчання-і-формати)).

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

## Читання і запис

Кожен формат реалізує три інтерфейси:

- **`IWeightsReader`** ([`IWeightsReader.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/IWeightsReader.h)) - метод `load(net, filename, cutoff)` завантажує ваги шарів до межі відсікання у масиви нейромережі (`load_weights_upto()` оригінального Darknet), а метод `load(net, filename, cutoff, rest)` ще й зберігає решту файлу: документ без масивів з описом нейромережі і записом навчання тих форматів, які їх зберігають. Метод `read(filename, net, document)` читає весь файл у незалежний від формату `WeightsDocument`, а метод `peek(filename, rest, weights)` читає решту файлу до першого масиву ваг і повідомляє, чи містить файл ваги (лише файли dxxwjz2: решта форматів не зберігає опису нейромережі, який можна було б прочитати).
- **`IWeightsWriter`** ([`IWeightsWriter.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/IWeightsWriter.h)) - метод `save(net, filename, cutoff, save_ema)` зберігає ваги шарів до межі відсікання (`save_weights_upto()` оригінального Darknet: поточну версію, значення seen поточної ітерації, масиви EMA згорткових шарів для `save_ema`), а метод `save(net, filename, cutoff, save_ema, rest)` зберігає поряд з ними опис нейромережі і запис навчання з решти, які формати лише ваг пропускають. Метод `write(document, filename)` записує документ.
- **`IWeightsFileFormat`** ([`IWeightsFileFormat.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/IWeightsFileFormat.h)) - сам формат: його назва (та, що в командному рядку), розширення назв його файлів і опис, чи зберігають його файли структуру нейромережі (шари і назви масивів, тож файли читаються без нейромережі) і опис нейромережі, класи читання і запису його файлів. `WeightsFileFormats` директорії [`weights-formats`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-formats) знаходить формати за їхніми назвами і розширеннями назв файлів (будь-яке розширення, окрім `.dxxwjz1` і `.dxxwjz2`, - це оригінальний файл ваг), а `NetworkSource` читає опис нейромережі з файлу, з якого вона будується: текст файлу cfg або опис, який зберігає файл dxxwjz2.

Класи читання і запису форматів однаково завантажують і зберігають ті самі масиви нейромережі, тож будь-який файл однієї нейромережі завантажує ту саму нейромережу. Саме читання документів методом `read()` і їхній запис методом `write()` конвертують файли між форматами.

## Оригінальний файл ваг darknet

Файл складається з трійки чисел версії int32 `major`, `minor` і `revision`, кількості зображень, які бачила нейромережа (uint64 `seen` для версій `(major * 10 + minor) >= 2` і uint32 для старіших версій), і масивів float шарів з вагами, що йдуть один за одним у порядку шарів і таблиці нижче. Власної структури файл не має: яке значення float належить якому масиву, визначає лише нейромережа.

- **`OrigWeightsReader`** ([`OrigWeightsReader.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsReader.h)) - перенесені на C++ оригінальні функції `load_weights()`, `load_weights_upto()` і процедури окремих шарів, що читають рівно ті самі байти, що й оригінал: пропускають шари з `dontload` і масиви нормалізації батчів шарів з `dontloadscales`, транспонують ваги згорткових шарів з `flipped` і ваги повнозв'язних шарів у файлах, старший чи молодший номер версії яких перевищує 1000, заповнюють масиви всіма повними значеннями float, які містить обрізаний файл, і зупиняються після шару, читання якого виходить за кінець файлу. Метод `read()` цього класу розбиває файл на масиви нейромережі, якій належить файл (нейромережа обов'язкова).
- **`OrigWeightsWriter`** ([`OrigWeightsWriter.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsWriter.h)) - перенесені на C++ оригінальні функції `save_weights()`, `save_weights_upto()` і процедури окремих шарів, байт у байт сумісні з оригінальними файлами. Метод `write()` цього класу записує заголовок, кожен масив документа і його хвостові байти, тобто відтворює файл, з якого документ було прочитано.
- **`OrigWeightsValidator`** ([`OrigWeightsValidator.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsValidator.h)) - порівнює розмір файлу з масивами нейромережі: з масивами, які записує `save_weights()` (файл, який зберігає darknet), і з масивами, які читає `load_weights()` і серед яких немає масивів шарів з `dontload` і `dontloadscales` (файл, створений для завантаження такої нейромережі). Файл, розмір якого не відповідає жодному з них, обрізаний або містить зайві байти, про що валідатор повідомляє попередженнями, тоді як стиснений gzip файл (файл dxxwjz1, переданий як оригінальний) і файл, коротший за заголовок, є помилками. Метод `validate(document, net)` цього класу перевіряє документ, який має бути записаний як оригінальний файл ваг нейромережі: такий файл не зберігає назв своїх масивів, тож масиви документа мають бути масивами нейромережі, починаючи з першого, і всі вони, крім останнього (обрізаний файл), мають бути повними. Документ, у якому бракує масиву посередині або за коротшим масивом ідуть інші (наприклад, відредагований вручну файл dxxwjz1), є помилкою, оскільки завантажувачі прочитали б масиви після пропуску не в ті масиви.

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

Допоміжні класи директорії [`dxxwjz1/v1`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1):

- **`Dxxwjz1Format`** - константи формату: назва, версія, розширення імені файлу і ключі JSON.
- **`Dxxwjz1FloatCodec`** - описаний вище текст JSON значень float (функції C++17 `std::to_chars()` і `std::from_chars()`, які не залежать від локалі).
- **`Dxxwjz1Base64`** - текст base64 хвостових байтів.
- **`Dxxwjz1JsonString`** - текст рядка JSON для будь-якого тексту: байти, які не є UTF-8, замінюються символами U+FFFD.
- **`Dxxwjz1JsonEmitter`** - записує текст JSON документа у потік пошарово, розміщуючи кожен шар і кожен масив в окремому рядку, безпосередньо з масивів нейромережі чи документа. Класи запису наступних версій записують свої члени через його метод `sections()`.
- **`Dxxwjz1SaxHandler`** - будує документ з подій SAX-аналізатора [nlohmann json](https://github.com/nlohmann/json) по одному значенню, тож дерево JSON з мільйонів значень float не будується, і попутно перевіряє структуру документа: синтаксис JSON, формат і версію, типи значень, назви типів шарів, підшарів і масивів, повторювані ключі. Обробники наступних версій задають свої назву формату, версію й обов'язкові ключі і приймають значення своїх ключів документа (див. формат dxxwjz2 нижче).
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

Класи читання перевіряють члени network і training так само, як перевіряються члени dxxwjz1 (типи значень, індекси класів, розміри стовпців, повторювані ключі), пропускають їхні невідомі ключі з попередженням і попереджають про кількість назв класів, відмінну від `classes=` нейромережі. Класи версії dxxwjz2 побудовані на класах dxxwjz1, а директорія [`dxxwjz2/v2`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2) містить те, що вони додають:

- **`Dxxwjz2Format`** - константи формату: назва, версія, розширення назв файлів і ключі JSON, які він додає.
- **`Dxxwjz2JsonEmitter`** - `Dxxwjz1JsonEmitter` з членами network і training, які він записує між generator і header (його метод `sections()`): клас у рядку і стовпець втрат у рядку.
- **`Dxxwjz2SaxHandler`** - `Dxxwjz1SaxHandler` з назвою, версією і обов'язковими ключами формату dxxwjz2, який передає значення ключів network і training класам `Dxxwjz2SubtreeBuilder` (їхні дерева JSON, малі порівняно з вагами, значення float - точні значення їхніх текстів) і `Dxxwjz2MetadataReader` (`NetworkDescription` і `TrainingRecord` з цих дерев). Його попереднє читання (peek) зупиняється на першій вазі, щойно прочитано network, тож опис нейромережі файлу читається без його ваг.
- **`Dxxwjz2Parser`** - розбір файлів і текстів класом `Dxxwjz1Parser` з обробником dxxwjz2, попереднє читання файлів.
- **`Dxxwjz2Reader`**, **`Dxxwjz2Writer`** - `Dxxwjz1Reader` і `Dxxwjz1Writer` з перевіркою і записом dxxwjz2: опис нейромережі і запис навчання потрапляють у решту завантаження і беруться з решти збереження та з документа, що записується. Клас запису відхиляє решту без тексту cfg, бо файл зберігає опис нейромережі.
- **`Dxxwjz2Validator`** - валідатор файлів: перевірки `Dxxwjz1Validator`, текст cfg, назви класів нейромережі.
- **`Dxxwjz2FileFormat`** - `IWeightsFileFormat` файлів dxxwjz2.

## Незалежна від формату модель

- **`WeightsSchema`** ([`WeightsSchema.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsSchema.h)) - масиви і підшари кожного типу шарів, їхній порядок і назви (таблиця вище).
- **`WeightsLayout`** ([`WeightsLayout.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsLayout.h)) - масиви нейромережі у порядку файлів: масиви, які записує збереження (масиви EMA для збереження EMA), і масиви, які читає завантаження, з їхніми транспонуваннями, до певного шару.
- **`WeightsDocument`** ([`WeightsDocument.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h)) - вміст файлу ваг: заголовок, шари з їхніми масивами і байти після масивів. Масиви у своєму порядку становлять корисні дані оригінального файлу ваг, тож документ файлу зберігає кожен його байт, включно з обрізаними масивами, частиною значення float, якою закінчується обрізаний файл, і зайвими байтами.
- **`NetworkDescription`** ([`NetworkDescription.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/NetworkDescription.h)) і **`TrainingRecord`** ([`TrainingRecord.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/TrainingRecord.h)) - нейромережа і навчання документа, які зберігають файли dxxwjz2: текст cfg і назви класів, секунди, втрати ітерацій і mAP обчислень mAP.
- **`NetworkFiles`** ([`NetworkFiles.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/NetworkFiles.h)) - читає текст файлу cfg і назви з файлу назв класів darknet (назва в рядку, включно з закінченнями рядків CR LF).
- **`WeightsDocumentValidator`** ([`WeightsDocumentValidator.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocumentValidator.h)) - перевіряє документ окремо і на відповідність нейромережі, записуючи результати у `WeightsReport`.

Валідатори повідомляють про помилки, через які файл неможливо завантажити чи конвертувати, і попереджають про файли, які завантажуються, але не так, як могли очікувати їхні автори:

| Валідатор | Помилки | Попередження |
| --- | --- | --- |
| `OrigWeightsValidator` | файл, який неможливо прочитати, коротший за заголовок або стиснений gzip; для документа, що записується як оригінальний файл ваг нейромережі: масив, якого бракує посередині, коротший масив, за яким ідуть інші, масив, який не належить жодному шару нейромережі | обрізаний файл, частина значення float у кінці, зайві байти, файл, створений для завантаження шарів з `dontload` і `dontloadscales` |
| `Dxxwjz1Validator` | файл, який неможливо прочитати, пошкоджені чи обрізані дані gzip, синтаксичні помилки JSON, інший формат чи версія, хибна структура, документи, які відхиляє `WeightsDocumentValidator` | нестиснений файл JSON, невідомі ключі, попередження `WeightsDocumentValidator` |
| `Dxxwjz2Validator` | помилки `Dxxwjz1Validator`, документ без тексту cfg, хибні члени network чи training | попередження `Dxxwjz1Validator` (окрім масивів нейромережі для файлу без ваг), кількість назв класів, відмінна від класів нейромережі |
| `WeightsDocumentValidator` | 32-бітне поле seen для значення seen понад 32 біти, повторювані шари, масиви, яких немає у шарів їхнього типу; для нейромережі: шари поза межами її шарів або інших типів, масиви, яких у неї немає, масиви, довші за її масиви | масиви, коротші за масиви нейромережі, масиви нейромережі, яких немає в документі |

## Конвертування

Клас **`WeightsConverter`** ([`WeightsConverter.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-converter/WeightsConverter.h)) виконує конвертування **`WeightsConversion`** ([`WeightsConversion.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-converter/WeightsConversion.h)) файлу одного формату в інший (або в той самий): спершу розбирається нейромережа з файлу cfg (оригінальний файл ваг її потребує, а решта, якщо файл cfg вказано, перевіряється на відповідність їй), потім файл-джерело перевіряється і читається у документ, а документ записується у тимчасовий файл у директорії призначення, який після завершення запису замінює файл призначення. Невдале конвертування залишає файл призначення таким, яким він був, і не залишає по собі тимчасового файлу, а запит зупинки програми (натискання Ctrl+C) зупиняє конвертування між його кроками.

Оригінальний файл ваг розбивається на масиви нейромережі, які записує `save_weights()`, якщо тільки розмір файлу не відповідає масивам, які читає `load_weights()`. Оригінальний файл ваг, конвертований у файл dxxwjz1 чи dxxwjz2 і назад, байт у байт збігається з початковим, хай що він містить: заголовок будь-якої версії, обрізані масиви, зайві байти. Під час конвертування файлу в оригінальний файл ваг з файлом cfg клас `OrigWeightsValidator` перевіряє, що файл містить масиви нейромережі один за одним (див. вище), а без файлу cfg немає нейромережі, з якою можна звірити документ, тож його масиви записуються як є.

Файл призначення dxxwjz2 зберігає опис нейромережі: текст cfg з файлу cfg конвертування (файлу cfg чи dxxwjz2) замість тексту файлу-джерела dxxwjz2 - один з них обов'язковий, - і назви класів з файлу назв класів конвертування замість класів файлу-джерела, а ті - замість файлу `names=` з cfg нейромережі. Запис навчання файлу-джерела dxxwjz2 переходить у файл призначення dxxwjz2, тоді як формати лише ваг не зберігають ні того, ні іншого.

Конвертування без файлу-джерела створює з файлу cfg нейромережу, яку треба навчити: файл dxxwjz2 лише з описом нейромережі (текстом cfg і назвами класів, як описано вище), без шарів. Такий файл, конвертований у формати лише ваг, не зберігав би нічого, крім заголовка, тож таке конвертування не вдається, тоді як файли ваг нейромереж без масивів ваг конвертуються в обидва боки, як і решта.

## Навчання і формати

Навчання за замовчуванням зберігає свої ваги у файли dxxwjz2 (параметр командного рядка `--save-format` задає їхній формат, у бібліотеці - `LibraryContext::save_format`): файли `<base>_<ітерація>`, `<base>_last`, `<base>_final`, `<base>_best` і `<base>_ema` директорії резервних копій з розширенням формату. Код навчання мовою C зберігає їх через `weights_saver` свого контексту, який `DarknetContext` спрямовує до журналу навчання **`TrainingJournal`** ([`TrainingJournal.h`](/src/lib/darknet-adaptor/adaptors/TrainingJournal/TrainingJournal.h)):

- журнал починається з опису нейромережі навчання: тексту файлу cfg нейромережі навчання (або файлу dxxwjz2) і назв класів з файлу `--names`, з файлу ваг dxxwjz2, з файлу нейромережі dxxwjz2 чи з файлу `names=` з cfg - з першого з них, що вказаний;
- він продовжує запис навчання файлу ваг dxxwjz2, з якого починається навчання, до ітерації цих ваг (запис ітерацій після неї відкидається, а `--clear` відкидає весь запис разом з ітераціями), а секунди його годинника починаються з секунд запису;
- він записує втрати кожної ітерації і mAP кожного обчислення mAP один раз, у зростаючому порядку ітерацій, включно з ітерацією, збереженою до звіту про неї, і зберігає кожен файл ваг з часом навчання на цей момент.

Тож файл навчання, продовженого з файлу dxxwjz2, зберігає втрати і mAP кожної ітерації навчання у їхньому порядку і весь час усіх навчань, а середні втрати продовженого навчання продовжують останні середні втрати запису. Графік кожного запуску навчання - це новий файл `chart_<base>_<РРРР-ММ-ДД--ГГ-ХХ-СС>.png` з датою і часом запуску, тож жоден графік продовжених раніше навчань не перезаписується. На ньому заново малюється весь запис продовжених ваг (середні втрати і лінія mAP), і графік одразу зберігається, далі - кожні 100 ітерацій (кожну ітерацію з `--save-charts-every-iter`), наприкінці і при зупинці навчання; зображення `--chart-path` продовжується лише для ваг без запису (оригінальних і dxxwjz1). Графік запису файлу dxxwjz2 можна також намалювати будь-коли (параметр командного рядка `--draw-chart`, клас `TrainingChart` і `LibraryFacade::training_chart()` бібліотеки - графік, який GUI показує одразу, щойно відкрито директорію навчання).

Обчислення mAP навчання (`--map`) бере назви класів так само, тож назвам класів файлу dxxwjz2 не потрібен файл назв; назви, кількість яких відрізняється від кількості класів нейромережі, зупиняють навчання з помилкою.

Розпізнавання і навчання читають файли ваг кожного формату, формат визначає розширення назви файлу, а файл нейромережі - це файл cfg або dxxwjz2:

| Командний рядок | Нейромережа | Ваги |
| --- | --- | --- |
| `--train-cfg net.cfg --weights w` | з `net.cfg` | з файлу `w` будь-якого формату, запис навчання файлу dxxwjz2 продовжується |
| `--train-cfg net.dxxwjz2` | з cfg, який зберігає файл | з файлу, жодних для файлу без ваг (навчання з нуля) |
| `--detect-cfg net.cfg --weights w` | з `net.cfg` | з файлу `w` будь-якого формату, файл dxxwjz2 без ваг відхиляється |
| `--detect-cfg net.dxxwjz2` | з cfg, який зберігає файл | з файлу |
| лише `--weights net.dxxwjz2` | з cfg, який зберігає файл | розпізнавання з вагами файлу, навчання нейромережі файлу без ваг |

Резервні копії навчання названі за файлом його нейромережі: `<base>` файлів `net.cfg` і `net.dxxwjz2` - це `net`, і `<base>` резервних файлів навчання (файлів dxxwjz2 `net_last`, `net_final`, `net_best`, `net_ema` і `net_<ітерація>` з ітерацією їхніх ваг) - теж `net`, тож навчання, продовжене з самого лише резервного файлу, продовжує ті самі файли.

## Використання форматів

Код C++ завантажує і зберігає ваги нейромережі у будь-якому форматі через інтерфейси:

```
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2Reader.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2Validator.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-formats/WeightsFileFormats.h"

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
