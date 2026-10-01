# Файли ваг нейромереж

`darknetxx` читає і записує ваги нейромереж у двох форматах:

- **оригінальний файл ваг darknet** (`*.weights`) - бінарний файл, який читає і записує оригінальний Darknet; саме його приймають навчання і розпізнавання `darknetxx` (параметр командного рядка `--weights`) і в нього навчання зберігає свої резервні копії;
- **файл dxxwjz1** (`*.dxxwjz1`, від англ. **d**arknet**xx** **w**eights **j**son **z**ipped - стиснений файл ваг darknetxx у форматі JSON, перша версія) - стиснений gzip документ JSON з тими самими вагами, який може прочитати як будь-який інструмент для роботи з JSON, так і людина.

Обидва формати конвертуються один в одний без втрат: оригінальний файл ваг, конвертований у файл dxxwjz1 і назад, байт у байт збігається з початковим (див. секції [Конвертування файлів ваг нейромереж](/doc/sections/uk_UA/6-2-4-converting-the-network-weights-files.md) і [Бінарний файл darknetxxConverter](/doc/sections/uk_UA/5-35-the-darknetxxConverter-binary.md)).

Компоненти файлів ваг розміщені у просторі імен `darknet_adaptor::loaders` у директорії [`src/darknet-adaptor/adaptors/NetworksLoaders`](/src/darknet-adaptor/adaptors/NetworksLoaders):

| Директорія | Містить |
| --- | --- |
| [`NetworksLoaders`](/src/darknet-adaptor/adaptors/NetworksLoaders) | інтерфейси `IWeightsReader` і `IWeightsWriter`, які реалізує кожен формат |
| [`weights-model`](/src/darknet-adaptor/adaptors/NetworksLoaders/weights-model) | незалежну від формату модель ваг: масиви кожного виду шарів та їхній порядок (`WeightsSchema`), масиви нейромережі у цьому порядку (`WeightsLayout`), вміст файлу (`WeightsDocument`) і його перевірку (`WeightsDocumentValidator`, `WeightsReport`) |
| [`original-weights`](/src/darknet-adaptor/adaptors/NetworksLoaders/original-weights) | оригінальний файл ваг: `OrigWeightsReader`, `OrigWeightsWriter` і `OrigWeightsValidator` |
| [`dxxwjz1/v1`](/src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1) | файл dxxwjz1 першої версії: `Dxxwjz1Reader`, `Dxxwjz1Writer` і `Dxxwjz1Validator` з їхніми допоміжними класами (простір імен `dxxwjz1::v1`) |
| [`weights-converter`](/src/darknet-adaptor/adaptors/NetworksLoaders/weights-converter) | `WeightsConverter` - конвертер файлів між форматами |

Окремий вихідний файл діаграми нижче -
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

## Читання і запис

Кожен формат реалізує два інтерфейси:

- **`IWeightsReader`** ([`IWeightsReader.h`](/src/darknet-adaptor/adaptors/NetworksLoaders/IWeightsReader.h)) - метод `load(net, filename, cutoff)` завантажує ваги шарів до межі відсікання у масиви нейромережі (`load_weights_upto()` оригінального Darknet), а метод `read(filename, net, document)` читає весь файл у незалежний від формату `WeightsDocument`.
- **`IWeightsWriter`** ([`IWeightsWriter.h`](/src/darknet-adaptor/adaptors/NetworksLoaders/IWeightsWriter.h)) - метод `save(net, filename, cutoff, save_ema)` зберігає ваги шарів до межі відсікання (`save_weights_upto()` оригінального Darknet: поточну версію, значення seen поточної ітерації, масиви EMA згорткових шарів для `save_ema`), а метод `write(document, filename)` записує документ.

Класи читання і запису обох форматів однаково завантажують і зберігають ті самі масиви нейромережі, тож будь-який з двох файлів однієї нейромережі завантажує ту саму нейромережу. Саме читання документів методом `read()` і їхній запис методом `write()` конвертують файли між форматами.

## Оригінальний файл ваг darknet

Файл складається з трійки чисел версії int32 `major`, `minor` і `revision`, кількості зображень, які бачила нейромережа (uint64 `seen` для версій `(major * 10 + minor) >= 2` і uint32 для старіших версій), і масивів float шарів з вагами, що йдуть один за одним у порядку шарів і таблиці нижче. Власної структури файл не має: яке значення float належить якому масиву, визначає лише нейромережа.

- **`OrigWeightsReader`** ([`OrigWeightsReader.h`](/src/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsReader.h)) - перенесені на C++ оригінальні функції `load_weights()`, `load_weights_upto()` і процедури окремих шарів, що читають рівно ті самі байти, що й оригінал: пропускають шари з `dontload` і масиви нормалізації батчів шарів з `dontloadscales`, транспонують ваги згорткових шарів з `flipped` і ваги повнозв'язних шарів у файлах, старший чи молодший номер версії яких перевищує 1000, заповнюють масиви всіма повними значеннями float, які містить обрізаний файл, і зупиняються після шару, читання якого виходить за кінець файлу. Метод `read()` цього класу розбиває файл на масиви нейромережі, якій належить файл (нейромережа обов'язкова).
- **`OrigWeightsWriter`** ([`OrigWeightsWriter.h`](/src/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsWriter.h)) - перенесені на C++ оригінальні функції `save_weights()`, `save_weights_upto()` і процедури окремих шарів, байт у байт сумісні з оригінальними файлами. Метод `write()` цього класу записує заголовок, кожен масив документа і його хвостові байти, тобто відтворює файл, з якого документ було прочитано.
- **`OrigWeightsValidator`** ([`OrigWeightsValidator.h`](/src/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsValidator.h)) - порівнює розмір файлу з масивами нейромережі: з масивами, які записує `save_weights()` (файл, який зберігає darknet), і з масивами, які читає `load_weights()` і серед яких немає масивів шарів з `dontload` і `dontloadscales` (файл, створений для завантаження такої нейромережі). Файл, розмір якого не відповідає жодному з них, обрізаний або містить зайві байти, про що валідатор повідомляє попередженнями, тоді як стиснений gzip файл (файл dxxwjz1, переданий як оригінальний) і файл, коротший за заголовок, є помилками. Метод `validate(document, net)` цього класу перевіряє документ, який має бути записаний як оригінальний файл ваг нейромережі: такий файл не зберігає назв своїх масивів, тож масиви документа мають бути масивами нейромережі, починаючи з першого, і всі вони, крім останнього (обрізаний файл), мають бути повними. Документ, у якому бракує масиву посередині або за коротшим масивом ідуть інші (наприклад, відредагований вручну файл dxxwjz1), є помилкою, оскільки завантажувачі прочитали б масиви після пропуску не в ті масиви.

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

Допоміжні класи директорії [`dxxwjz1/v1`](/src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1):

- **`Dxxwjz1Format`** - константи формату: назва, версія, розширення імені файлу і ключі JSON.
- **`Dxxwjz1FloatCodec`** - описаний вище текст JSON значень float (функції C++17 `std::to_chars()` і `std::from_chars()`, які не залежать від локалі).
- **`Dxxwjz1Base64`** - текст base64 хвостових байтів.
- **`Dxxwjz1JsonEmitter`** - записує текст JSON документа у потік пошарово, розміщуючи кожен шар і кожен масив в окремому рядку, безпосередньо з масивів нейромережі чи документа.
- **`Dxxwjz1Parser`** - читає текст JSON у документ SAX-аналізатором [nlohmann json](https://github.com/nlohmann/json) по одному значенню, тож дерево JSON з мільйонів значень float не будується, і попутно перевіряє структуру документа: синтаксис JSON, формат і версію, типи значень, назви типів шарів, підшарів і масивів, повторювані ключі. Розбір виконується в числовій локалі "C" свого потоку, тож документ читається однаково, хай у якій локалі працює програма (включно з локалями, де десятковим роздільником є кома чи багатобайтовий символ), а локаль програми та її інших потоків залишається незмінною.
- **`Dxxwjz1Reader`**, **`Dxxwjz1Writer`** - класи читання і запису (див. вище). Клас запису стискає текст у файл у міру запису з найшвидшим рівнем стиснення zlib, якщо його конструктору не передано інший рівень: типовий рівень zlib потребує приблизно вшестеро більше часу і дає файли, менші приблизно на 12%, а найкращий - приблизно втридцятеро більше часу для файлів, менших приблизно на 14%, тоді як під час читання приймається файл будь-якого рівня стиснення.
- **`Dxxwjz1Validator`** - валідатор файлів (див. нижче).

Файли стискаються і розпаковуються контролером `zlibi::ZlibController` (див. секцію [Допоміжні компоненти](/doc/sections/uk_UA/4-7-3-supporting-components.md)) частинами, тож у пам'яті не тримається ані весь файл, ані весь текст JSON. Текст займає близько 12 байтів на кожне значення float, а стиснений файл приблизно в 1,35 раза більший за оригінальний файл ваг (приблизно в 1,2 раза після повторного стиснення з найкращим рівнем, `gzip -dc model.dxxwjz1 | gzip -9 > smaller.dxxwjz1`).

## Незалежна від формату модель

- **`WeightsSchema`** ([`WeightsSchema.h`](/src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsSchema.h)) - масиви і підшари кожного типу шарів, їхній порядок і назви (таблиця вище).
- **`WeightsLayout`** ([`WeightsLayout.h`](/src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsLayout.h)) - масиви нейромережі у порядку файлів: масиви, які записує збереження (масиви EMA для збереження EMA), і масиви, які читає завантаження, з їхніми транспонуваннями, до певного шару.
- **`WeightsDocument`** ([`WeightsDocument.h`](/src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h)) - вміст файлу ваг: заголовок, шари з їхніми масивами і байти після масивів. Масиви у своєму порядку становлять корисні дані оригінального файлу ваг, тож документ файлу зберігає кожен його байт, включно з обрізаними масивами, частиною значення float, якою закінчується обрізаний файл, і зайвими байтами.
- **`WeightsDocumentValidator`** ([`WeightsDocumentValidator.h`](/src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocumentValidator.h)) - перевіряє документ окремо і на відповідність нейромережі, записуючи результати у `WeightsReport`.

Валідатори повідомляють про помилки, через які файл неможливо завантажити чи конвертувати, і попереджають про файли, які завантажуються, але не так, як могли очікувати їхні автори:

| Валідатор | Помилки | Попередження |
| --- | --- | --- |
| `OrigWeightsValidator` | файл, який неможливо прочитати, коротший за заголовок або стиснений gzip; для документа, що записується як оригінальний файл ваг нейромережі: масив, якого бракує посередині, коротший масив, за яким ідуть інші, масив, який не належить жодному шару нейромережі | обрізаний файл, частина значення float у кінці, зайві байти, файл, створений для завантаження шарів з `dontload` і `dontloadscales` |
| `Dxxwjz1Validator` | файл, який неможливо прочитати, пошкоджені чи обрізані дані gzip, синтаксичні помилки JSON, інший формат чи версія, хибна структура, документи, які відхиляє `WeightsDocumentValidator` | нестиснений файл JSON, невідомі ключі, попередження `WeightsDocumentValidator` |
| `WeightsDocumentValidator` | 32-бітне поле seen для значення seen понад 32 біти, повторювані шари, масиви, яких немає у шарів їхнього типу; для нейромережі: шари поза межами її шарів або інших типів, масиви, яких у неї немає, масиви, довші за її масиви | масиви, коротші за масиви нейромережі, масиви нейромережі, яких немає в документі |

## Конвертування

Клас **`WeightsConverter`** ([`WeightsConverter.h`](/src/darknet-adaptor/adaptors/NetworksLoaders/weights-converter/WeightsConverter.h)) конвертує файл одного формату в інший (або в той самий): спершу розбирається нейромережа з файлу cfg (оригінальний файл ваг її потребує, а файл dxxwjz1, якщо файл cfg вказано, перевіряється на відповідність їй), потім файл-джерело перевіряється і читається у документ, а документ записується у тимчасовий файл у директорії призначення, який після завершення запису замінює файл призначення. Невдале конвертування залишає файл призначення таким, яким він був, і не залишає по собі тимчасового файлу, а запит зупинки програми (натискання Ctrl+C) зупиняє конвертування між його кроками.

Оригінальний файл ваг розбивається на масиви нейромережі, які записує `save_weights()`, якщо тільки розмір файлу не відповідає масивам, які читає `load_weights()`. Оригінальний файл ваг, конвертований у файл dxxwjz1 і назад, байт у байт збігається з початковим, хай що він містить: заголовок будь-якої версії, обрізані масиви, зайві байти. Під час конвертування файлу dxxwjz1 в оригінальний файл ваг з файлом cfg клас `OrigWeightsValidator` перевіряє, що файл містить масиви нейромережі один за одним (див. вище), а без файлу cfg немає нейромережі, з якою можна звірити документ, тож його масиви записуються як є.

## Використання форматів

Навчання і розпізнавання `darknetxx` читають оригінальні файли ваг (параметр командного рядка `--weights`), а файли dxxwjz1 конвертуються в них конвертером. Код C++ завантажує і зберігає ваги нейромережі у будь-якому з двох форматів через інтерфейси:

```
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Reader.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Validator.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Writer.h"

using namespace darknet_adaptor::loaders;

// net - подання нейромережі ANetworkAdaptor (networkptr, який повертає його
// метод get())
dxxwjz1::v1::Dxxwjz1Writer{}.save(*net, "backup.dxxwjz1");
dxxwjz1::v1::Dxxwjz1Reader{}.load(*net, "backup.dxxwjz1");

// перевірка файлу на відповідність нейромережі перед завантаженням
const WeightsReport report =
    dxxwjz1::v1::Dxxwjz1Validator::validate("backup.dxxwjz1", net.get());
```
