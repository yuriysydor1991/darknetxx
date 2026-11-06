# Файли ваг нейромереж

`darknetxx` читає і записує ваги нейромереж у восьми форматах:

- **оригінальний файл ваг darknet** (`*.weights`) - бінарний файл, який читає і записує оригінальний Darknet;
- **файл dxxwjz1** (`*.dxxwjz1`, від англ. **d**arknet**xx** **w**eights **j**son **z**ipped - стиснений файл ваг darknetxx у форматі JSON, перша версія) - стиснений gzip документ JSON з тими самими вагами, який може прочитати як будь-який інструмент для роботи з JSON, так і людина;
- **файл dxxwjz2** (`*.dxxwjz2`, друга версія) - документ dxxwjz1 з усім, з чим нейромережу навчають і з чим вона розпізнає, окрім її ваг: текстом файлу cfg нейромережі, назвами класів з їхніми індексами, втратами (loss) кожної ітерації навчання у порядку навчання і часом, який зайняло навчання;
- **файл dxxwjz2p** (`*.dxxwjz2p`, від англ. dxxwjz2 **p**roject - файл проекту dxxwjz2) - документ dxxwjz2 проекту навчання YOLO4: нейромережа, ваги і запис навчання поряд з анотаціями зображень YOLO4, тобто все, що потрібно навчанню, окрім самих зображень (див. секцію [Формат файлу проекту dxxwjz2p](#формат-файлу-проекту-dxxwjz2p));
- **файл dxxwjz3** (`*.dxxwjz3`, третя версія) - документ dxxwjz2, який складається лише з даних JSON: cfg нейромережі - це структура JSON опцій її секцій, об'єкт `net` опцій секції `[net]`, а не текст файлу cfg, а секції шарів зберігають масиви своїх шарів (див. секцію [Формат файлу dxxwjz3 третьої версії](#формат-файлу-dxxwjz3-третьої-версії));
- **файл dxxwjz3p** (`*.dxxwjz3p`, від англ. dxxwjz3 **p**roject - файл проекту dxxwjz3) - проект dxxwjz2p з документом dxxwjz3, тобто весь проект навчання лише з даних JSON (див. секцію [Формат файлу проекту dxxwjz3p](#формат-файлу-проекту-dxxwjz3p));
- **файл dxxwjz4** (`*.dxxwjz4`, четверта версія) - документ dxxwjz3 з текстом опису: коментарями й описом нейромережі та її даних будь-якої довжини (див. секцію [Формат файлу dxxwjz4 четвертої версії](#формат-файлу-dxxwjz4-четвертої-версії));
- **файл dxxwjz4p** (`*.dxxwjz4p`, від англ. dxxwjz4 **p**roject - файл проекту dxxwjz4) - весь проект навчання в одному файлі, включно із зображеннями анотацій: архів ZIP з єдиним документом JSON з усім, що потрібно навчанню, і зі зображеннями в його теці `data/`, який розпаковує будь-який інструмент zip (див. секцію [Формат файлу проекту dxxwjz4p](#формат-файлу-проекту-dxxwjz4p)).

**Тека навчання YOLO4**, та, яку експортує ImagesAnnotator, конвертується у проект dxxwjz2p і назад: це формат `yolo4`, який використовує лише конвертування (див. секцію [Тека навчання YOLO4](#тека-навчання-yolo4)).

[ImagesAnnotator](https://github.com/yuriysydor1991/ImagesAnnotator) сам генерує файли найновіших форматів: проект dxxwjz3p своїх анотацій і нейромережу dxxwjz3, яку треба навчити, своєї теки навчання YOLO4 (а також файли dxxwjz2p і dxxwjz2), і читає їх назад (див. секцію [Файли ImagesAnnotator](#файли-imagesannotator)).

Навчання і розпізнавання `darknetxx` читають файли ваг кожного формату (параметр командного рядка `--weights`, формат файлу визначає розширення його назви), навчання за замовчуванням зберігає свої ваги у файли dxxwjz3 (див. секцію [Навчання і формати](#навчання-і-формати) нижче), а самі лише файли dxxwjz2, dxxwjz2p, dxxwjz3, dxxwjz3p, dxxwjz4 і dxxwjz4p - це нейромережі, з якими працює розпізнавання. Навчання проекту dxxwjz4p розпаковує його у тимчасову директорію і зберігає нейромережу, яку воно навчає, у файл dxxwjz4 поряд з проектом (див. секцію [Навчання проекту dxxwjz4p](#навчання-проекту-dxxwjz4p)).

Ваги форматів конвертуються один в одний без втрат: оригінальний файл ваг, конвертований у файл dxxwjz1, dxxwjz2 чи dxxwjz3 і назад, байт у байт збігається з початковим (див. секції [Конвертування файлів ваг нейромереж](/doc/sections/uk_UA/6-2-4-converting-the-network-weights-files.md) і [Бінарний файл darknetxxConverter](/doc/sections/uk_UA/5-35-the-darknetxxConverter-binary.md)), тоді як опис нейромережі і запис навчання зберігають лише файли dxxwjz2, dxxwjz3 і dxxwjz4 та їхні проекти, анотації зображень - лише проекти dxxwjz2p, dxxwjz3p і dxxwjz4p, а самі зображення і текст опису - лише проект dxxwjz4p (опис - також файл dxxwjz4).

Формати - їхні документи, класи читання, запису і перевірки та конвертер між ними - належать окремій бібліотеці [darknetxx-datadrivers](https://github.com/yuriysydor1991/darknetxx-datadrivers), з якою будується проект і яка описує їх повністю: ключі документів JSON, текст значень float, архіви ZIP проектів і перевірки кожного валідатора. Секції нижче коротко розповідають, що darknetxx бере з бібліотеки, як він передає їй нейромережі darknet і що зберігає кожен формат.

## Бібліотека darknetxx-datadrivers

Формати файлів ваг нейромереж і проектів навчання були розміщені в директорії [`src/NetworksLoaders`](/src/NetworksLoaders) цього проекту, а їхнє стиснення zlib - у директорії `src/zlib`. Їх винесено в бібліотеку darknetxx-datadrivers, створену з гілки `applib` проекту [cpp-app-template](https://github.com/yuriysydor1991/cpp-app-template), яку проект використовує встановленою (див. секцію [Вмикання бібліотеки darknetxx-datadrivers](/doc/sections/uk_UA/5-39-enabling-the-darknetxx-datadrivers-library.md)), тож darknetxx, її бінарний файл `darknetxxConverter` і будь-який інший інструмент спільно використовують одну й ту саму реалізацію форматів:

| Колишні вихідні файли darknetxx | Містили | Тепер у бібліотеці |
| --- | --- | --- |
| `src/NetworksLoaders/IWeightsReader.h`, `IWeightsWriter.h` і `IWeightsFileFormat.h` | інтерфейси, які реалізує кожен формат | `IWeightsReader`, `IWeightsWriter` і `IWeightsFileFormat` публічних заголовних файлів, які завантажують файли в нейромережу `INetworkWeights` і зберігають їх з неї |
| `src/NetworksLoaders/weights-model` | незалежну від формату модель ваг та її перевірку | `WeightsSchema`, `WeightsLayout`, `WeightsDocument` (з його `NetworkDescription`, `TrainingRecord`, `Yolo4Annotations`, `Yolo4Image` і `Yolo4Box`), `NetworkFiles` і `WeightsReport` публічних заголовних файлів, валідатор документів - усередині |
| `src/NetworksLoaders/original-weights` | оригінальний файл ваг darknet | усередині бібліотеки |
| `src/NetworksLoaders/dxxwjz/dxxwjz1` ... `dxxwjz4p` | формати dxxwjz1, dxxwjz2, dxxwjz2p, dxxwjz3, dxxwjz3p, dxxwjz4 і dxxwjz4p та архіви ZIP проектів dxxwjz4p | усередині бібліотеки |
| `src/NetworksLoaders/yolo4-folder` | теку навчання YOLO4 для конвертування | усередині бібліотеки, а `yolo4::Yolo4FolderLayout` файлів теки - серед публічних заголовних файлів |
| `src/NetworksLoaders/weights-formats` | формати за їхніми назвами і розширеннями назв файлів, файли, з яких будуються нейромережі | `WeightsFileFormats` і `NetworkSource` публічних заголовних файлів |
| `src/NetworksLoaders/weights-converter` | конвертер файлів між форматами | `WeightsConverter` і `WeightsConversion` публічних заголовних файлів (`NetworkRewriter` цієї директорії залишився тут, див. нижче) |
| `src/NetworksLoaders/OrigDefaultLoader/CfgSection.h` | секції файлу cfg | `CfgSection` публічних заголовних файлів, яка кидає `CfgError` |
| `src/zlib` | `zlibi::ZlibController` і файлові потоки gzip | усередині бібліотеки |
| `src/darknetxxConverter-target.cmake` | бінарний файл `darknetxxConverter` | бінарний файл бібліотеки (див. секцію [Бінарний файл darknetxxConverter](/doc/sections/uk_UA/5-35-the-darknetxxConverter-binary.md)) |
| `src/detector-cmake-adaptor/darknet/src/structs`, `enums` і структура `layer` файлу `layers/layer.h` | C типи нейромережі та шарів darknet | заголовкові файли `<DarknetxxDataDrivers-0.12/darknet/*.h>`, які включають джерельні коди darknet |
| перевірки типів шарів і `type_of()` класу `NetworkWeights` | `LayerType` значення `LAYER_TYPE` darknet і назад | `DarknetLayerType` публічних заголовкових файлів |

Публічні заголовні файли директорії `include/DarknetxxDataDrivers-0.12`, простір імен `DarknetxxDataDrivers012`, - це весь інтерфейс бібліотеки. Класи форматів - класи читання, запису і перевірки, аналізатори і кодеки кожного з них - будуються з прихованою видимістю символів і залишаються всередині спільного об'єкта, тож darknetxx звертається до форматів лише через реєстр `WeightsFileFormats`: за їхніми назвами (тими, що й у командному рядку) і за розширеннями назв файлів, а будь-який інший шлях - це оригінальний файл ваг. Бібліотека не містить коду darknet: типи шарів - це її `LayerType` (`LAYER_TYPE` darknet з тими самими назвами і значеннями), а нейромережі, у які завантажуються і з яких зберігаються файли, - це подання користувача бібліотеки над його власними нейромережами (`INetworkWeights`).

## Нейромережі darknet для форматів

Нейромережі darknet для форматів залишаються в цьому проекті, у просторі імен `darknet_adaptor::loaders`:

| Вихідні файли | Містять |
| --- | --- |
| [`src/NetworksLoaders/DataDriversAliases.h`](/src/NetworksLoaders/DataDriversAliases.h) | єдине підключення бібліотеки і псевдонім `dxxdd` її простору імен |
| [`src/NetworksLoaders/OrigDefaultLoader`](/src/NetworksLoaders/OrigDefaultLoader) | `OrigDefaultLoader` нейромереж darknet із секцій cfg бібліотеки, який завантажує в них файли ваг кожного формату (див. секцію [Компоненти адаптера Darknet](/doc/sections/uk_UA/4-7-2-darknet-adaptor-components.md)) |
| [`src/NetworksLoaders/network-weights`](/src/NetworksLoaders/network-weights) | подання `NetworkWeights` бібліотеки над нейромережею darknet і `NetworkWeightsConverter` файлів за нейромережею darknet з cfg |
| [`src/NetworksLoaders/network-rewriter`](/src/NetworksLoaders/network-rewriter) | `NetworkRewriter` зміненої нейромережі, перевіреної нейромережею darknet зі зміненого cfg (див. секцію [Збереження зміненої нейромережі](#збереження-зміненої-нейромережі)) |

Журнал навчання, `TrainingJournal` директорії [`TrainingJournal`](/src/darknet-adaptor/adaptors/TrainingJournal), тримає опис нейромережі і запис навчання, які зберігають файли ваг навчання, `TrainingChart` директорії [`TrainingChart`](/src/darknet-adaptor/adaptors/TrainingChart) малює графік цього запису (див. секцію [Навчання і формати](#навчання-і-формати)), а `TrainingDataRegistry` директорії [`TrainingData`](/src/darknet-adaptor/adaptors/TrainingData) передає списки зображень і мітки проектів dxxwjz2p і dxxwjz3p коду навчання darknet мовою C (див. секцію [Навчання проекту dxxwjz2p](#навчання-проекту-dxxwjz2p)) - усі вони працюють з документами бібліотеки.

Окремий вихідний файл діаграми нижче -
[`doc/diagrams/network-weights-files-classes.puml`](/doc/diagrams/network-weights-files-classes.puml).

@startuml
hide empty members

package "darknetxx-datadrivers (the DarknetxxDataDrivers012 namespace, the dxxdd alias)" {
  interface INetworkWeights {
    +layers() : int
    +type(layer) : LayerType
    +layout(options) : WeightsLayoutEntries
    +classes() : int
    +names() : string
    +seen() / saved_seen() : uint64
    +loaded_seen(seen)
  }

  class CfgNetworkStructure {
    +{static} of_text(cfg, name) : CfgNetworkStructurePtr
    +{static} of_file(path) : CfgNetworkStructurePtr
    +structure() : Layer[]
  }

  interface IWeightsReader {
    +load(net, filename, cutoff) : bool
    +load(net, filename, cutoff, rest) : bool
    +read(filename, net, document) : bool
    +peek(filename, rest, weights) : bool
  }

  interface IWeightsWriter {
    +save(net, filename, cutoff, save_ema) : bool
    +save(net, filename, cutoff, save_ema, rest) : bool
    +write(document, filename) : bool
  }

  interface IWeightsFileFormat {
    +name() / extension() / description() : string
    +keeps_structure() / keeps_network() : bool
    +keeps_annotations() / keeps_images() : bool
    +keeps_description() : bool
    +create_reader() : IWeightsReaderPtr
    +create_writer() : IWeightsWriterPtr
  }

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
    +{static} trained_of(path) / resumed_of(path) : string
    +{static} keeps_images(path) : bool
  }

  class WeightsDocument {
    +header : WeightsHeader
    +layers : WeightsLayer[]
    +trailing : bytes
    +network : NetworkDescription
    +training : TrainingRecord
    +annotations : Yolo4Annotations
    +description : string
  }

  class WeightsConversion {
    +from / to : IWeightsFileFormatPtr
    +src / dst / cfg / names / description : string
    +stop_requested : function
  }

  class WeightsConverter {
    +convert(conversion) : bool
    +{static} write_destination(conversion, document) : bool
    #create_network(cfg) : INetworkWeightsPtr
  }

  class CfgSection {
    +{static} read_cfg(filename) : CfgSections
    +{static} read_cfg(text, name) : CfgSections
  }

  class "LayerType, DarknetLayerType, WeightsSchema,\nWeightsLayout, WeightsReport" as Schema

  class "the formats: the original weights, the dxxwjz1 ... dxxwjz4p,\nthe YOLO4 folder, their validators, the ZlibController" as Formats <<inside the shared object>>

  class "darknetxxConverter" as DxxConverter <<binary>>
}

package "darknet_adaptor::loaders" {
  class NetworkWeights {
    +NetworkWeights(net)
    +NetworkWeights(adaptor)
    +{static} create(adaptor) : NetworkWeightsPtr
    +{static} layout_of(net, options) : WeightsLayoutEntries
  }

  class NetworkWeightsConverter {
    +{static} create() : NetworkWeightsConverterPtr
    #create_network(cfg) : INetworkWeightsPtr
  }

  class NetworkRewriter {
    +{static} rewrite(src, dst, network, message) : bool
  }

  class OrigDefaultLoader {
    +load(dctx) : bool
    +parse_network_cfg_custom(cfg, batch, time_steps) : ANetworkAdaptorPtr
    +parse_network_text(cfg, name, batch, time_steps) : ANetworkAdaptorPtr
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
}

class TrainingChart {
  +{static} render(rest, png) : bool
  +{static} render_file(weights, png) : bool
}

class "NetworksCApi (extern C)" as NetworksCApi {
  +load_weights_upto(net, filename, cutoff)
  +save_weights_upto(net, filename, cutoff, save_ema)
}

INetworkWeights <|.. CfgNetworkStructure
INetworkWeights <|.. NetworkWeights
WeightsConverter <|-- NetworkWeightsConverter
IWeightsFileFormat <|.. Formats
WeightsFileFormats o-- IWeightsFileFormat
IWeightsFileFormat ..> IWeightsReader : creates
IWeightsFileFormat ..> IWeightsWriter : creates
IWeightsReader ..> INetworkWeights : loads the files into
IWeightsWriter ..> INetworkWeights : saves the files of
IWeightsReader ..> WeightsDocument : reads
IWeightsWriter ..> WeightsDocument : writes
Formats ..> Schema : the arrays of the layers
CfgNetworkStructure ..> CfgSection : walks the cfg sections of
WeightsConverter ..> WeightsConversion
WeightsConverter ..> WeightsFileFormats : reads the source, writes the destination by
WeightsConverter ..> CfgNetworkStructure : the default network of the cfg
DxxConverter ..> WeightsConverter : converts by
NetworkSource ..> WeightsFileFormats : peeks the files by

NetworkWeights ..> Schema : walks the darknet layers into
NetworkWeightsConverter ..> OrigDefaultLoader : builds the darknet network of the cfg
NetworkWeightsConverter ..> NetworkWeights : hands the network over as
NetworkRewriter ..> OrigDefaultLoader : builds the edited network
NetworkRewriter ..> NetworkWeights : the arrays of the edited network
NetworkRewriter ..> CfgSection : compares the sections by
NetworkRewriter ..> WeightsConverter : writes the destination by
OrigDefaultLoader ..> CfgSection : parses the cfg by
OrigDefaultLoader ..> NetworkSource : parses the network of
OrigDefaultLoader ..> NetworkWeights : loads the weights into
OrigDefaultLoader ..> TrainingJournal : starts the journal of the training
OrigDefaultLoader ..> TrainingDataRegistry : the lists of the project training
NetworksCApi ..> NetworkWeights : loads and saves the weights through
NetworksCApi ..> WeightsFileFormats : the format of the file
TrainingJournal ..> NetworkWeights : saves the training weights through
TrainingJournal ..> IWeightsFileFormat : saves by
TrainingChart ..> WeightsFileFormats : peeks the files by
TrainingDataRegistry ..> WeightsDocument : the annotations of
"darknetxx_impl::WeightsConversionLib" ..> NetworkWeightsConverter : converts the library context by
"darknetxx012::LibraryFacade" ..> NetworkRewriter : saves the edited network by
@enduml

### Заголовний файл псевдоніма

Заголовний файл [`DataDriversAliases.h`](/src/NetworksLoaders/DataDriversAliases.h) - єдине місце в усьому проекті, яке називає бібліотеку: він підключає заголовний файл `<DarknetxxDataDrivers-0.12/DxxDataDriversFacade.h>`, який підтягує всі інші публічні заголовні файли бібліотеки (документи, реєстр форматів, джерела нейромереж, секції і структуру cfg, розкладку теки YOLO4 і конвертер), і оголошує псевдонім `dxxdd` простору імен `DarknetxxDataDrivers012`, назва якого містить мажорну і мінорну версію бібліотеки. Усі інші вихідні файли проекту звертаються до бібліотеки через псевдонім (`dxxdd::WeightsDocument`, `dxxdd::WeightsFileFormats` та інші), тож перехід на простір імен наступної версії бібліотеки залишається зміною одного рядка в цьому файлі. Заголовний файл підключається через версіоновану директорію `<DarknetxxDataDrivers-0.12/...>`, а не через простий `<DxxDataDriversFacade.h>`, який теж знаходиться у корені підключень без префікса: префікс фіксує версію бібліотеки, з якою працює кожна одиниця трансляції, а загальні назви цього кореня (`CfgSection.h`, `WeightsDocument.h`) - саме ті, що конфліктують у переповненому шляху підключень.

### Подання NetworkWeights

Бібліотека не знає нейромереж darknet: її класи читання завантажують ваги файлів у масиви подання `INetworkWeights` користувача бібліотеки, класи запису зберігають масиви подання у файли, файли без власної структури читаються за масивами подання, а документи решти перевіряються на відповідність їм. **`NetworkWeights`** ([`NetworkWeights.h`](/src/NetworksLoaders/network-weights/NetworkWeights.h)) - це таке подання над структурою нейромережі darknet:

- `layers()`, `type(layer)`, `classes()` і `names()` - шари нейромережі, їхні типи бібліотеки, класи останнього шару і ключ `names=` з cfg подання, створеного з адаптера нейромережі;
- `layout(options)` - масиви float нейромережі у порядку файлів ваг за параметрами (збереження, збереження EMA, завантаження з їхніми транспонуваннями, до обмеження шарів), ті самі масиви структури нейромережі, у які завантажують класи читання і з яких зберігають класи запису. Обхід шарів нейромережі darknet, який виконували оригінальні `save_weights_upto()` і `load_weights_upto()`, - `WeightsLayout::of()` колишньої моделі ваг - переїхав у цей проект як `NetworkWeights::layout_of()`, бо він обходить шари darknet;
- `seen()`, `loaded_seen(seen)` і `saved_seen()` - кількість зображень, які бачила нейромережа, значення seen завантаженого файлу, за яким іде поточна ітерація нейромережі (`get_current_batch()`), і значення seen збереженого файлу поточної ітерації (`get_current_iteration()`, помножене на batch і subdivisions), так, як це робили оригінальні функції.

Подання створюється зі структури нейромережі лише на час виклику (`NetworkWeights{net}`, яке `OrigDefaultLoader`, `load_weights_upto()` і `save_weights_upto()` C API darknet і `TrainingJournal` передають класам читання і запису), з константної структури нейромережі, за якою читаються і перевіряються документи, або з адаптера нейромережі, який подання тримає живим (`NetworkWeights::create()` у `NetworkWeightsConverter`). Типи шарів перетворюються `dxxdd::DarknetLayerType::type_of()` в обидва боки, що бібліотека перевіряє під час власної компіляції. [`NetworkWeights.cpp`](/src/NetworksLoaders/network-weights/NetworkWeights.cpp) під час компіляції перевіряє, що версія файлів ваг, яку записує бібліотека (її `WeightsHeader::CURRENT_MAJOR`, `CURRENT_MINOR` і `CURRENT_REVISION`), - це `MAJOR_VERSION`, `MINOR_VERSION` і `PATCH_VERSION` з `version.h` вихідних файлів darknet: бібліотека іншої версії зупиняє побудову проекту, а не читання файлів.

### Дві нейромережі cfg

Оригінальний файл ваг darknet не має власної структури, тож він читається за нейромережею свого файлу cfg, яку створює користувач бібліотеки (`WeightsConverter::create_network()`), і два бінарні файли конвертування створюють її по-різному:

- **`NetworkWeightsConverter`** ([`NetworkWeightsConverter.h`](/src/NetworksLoaders/network-weights/NetworkWeightsConverter.h)) цього проекту - це `dxxdd::WeightsConverter` нейромереж darknet: його `create_network()` - це подання `NetworkWeights` справжньої нейромережі darknet, яку `OrigDefaultLoader::parse_network_cfg_custom()` будує з cfg (нейромережа батчу одного зображення з найменшою пам'яттю, бо масиви ваг не залежать від батчу), тієї самої нейромережі, яку darknetxx навчає і якою розпізнає. Ним конвертує `darknetxx_impl::WeightsConversionLib` бібліотеки darknetxx, тобто прапорці конвертування бінарного файлу `darknetxx`.
- **`CfgNetworkStructure`** бібліотеки - це структура нейромережі, яку бібліотека сама створює з тексту cfg, без коду darknet: вона обходить секції cfg так, як це робить `parse_network_cfg_custom()` darknet, - вхід `[net]`, потім кожен шар із входом, який видає попередній шар, або шарами, які називають його ключі `from=` і `layers=`, - і створює шари, розміри їхніх входів і виходів і масиви float, які виділяє кожен шар darknet, без пам'яті для них (дані `nullptr`). Це типова нейромережа `create_network()` конвертера бібліотеки, тож за нею оригінальні файли ваг читає бінарний файл `darknetxxConverter` бібліотеки.

Обидві мають давати розкладку тих самих масивів, щоб обидва бінарні файли читали і записували файли однаково. Компонентний тест **`CTEST_NetworkWeights`** ([`CTEST_NetworkWeights.cpp`](/src/NetworksLoaders/network-weights/tests/component/NetworkWeights/CTEST_NetworkWeights.cpp)) перевіряє це для кожного файлу cfg директорії [`misc/examples/cfg`](/misc/examples/cfg) і тестів завантажувача нейромереж: кількість шарів, тип кожного шару, класи, ключ `names=` і розкладку збереження, збереження EMA, завантаження, транспонованого завантаження і обмеження шарів, запис за записом. Файли cfg, з яких darknet звідти не будує нейромережі (файли, поряд з якими darknet читає ще інші файли, - дерева і мапи), пропускаються, але порівняти треба більше половини файлів. Тест також перевіряє типи шарів в обидва боки і те, що масиви подання - це масиви нейромережі, і передає масиви нейромережі через оригінальний файл ваг і файл dxxwjz1 подання в обидва боки без змін.

## Читання і запис

Кожен формат реалізує три інтерфейси бібліотеки:

- **`IWeightsReader`** - метод `load(net, filename, cutoff)` завантажує ваги шарів до обмеження у масиви подання нейромережі (`load_weights_upto()` оригінального Darknet), а метод `load(net, filename, cutoff, rest)` зберігає ще й решту файлу: документ без масивів, з описом нейромережі і записом навчання форматів, які їх зберігають. Метод `read(filename, net, document)` читає весь файл у незалежний від формату `WeightsDocument`, а метод `peek(filename, rest, weights)` читає решту файлу до його першого масиву ваг і повідомляє, чи містить файл ваги.
- **`IWeightsWriter`** - метод `save(net, filename, cutoff, save_ema)` зберігає ваги шарів до обмеження (`save_weights_upto()` оригінального Darknet: поточна версія, seen поточної ітерації, масиви EMA згорткових шарів для `save_ema`), а метод `save(net, filename, cutoff, save_ema, rest)` зберігає поряд з ними опис нейромережі і запис навчання з решти, які формати лише ваг пропускають. Метод `write(document, filename)` записує документ.
- **`IWeightsFileFormat`** - сам формат: його назва (та, що й у командному рядку), розширення назви файлу та опис, чи зберігають його файли структуру нейромережі (шари і назви масивів, тож вони читаються без нейромережі), опис нейромережі, анотації, зображення і текст опису, а також класи читання і запису його файлів.

`WeightsFileFormats` знаходить формати за їхніми назвами і розширеннями назв файлів і перелічує формати конвертування: кожен формат і теку YOLO4 (його `convertible()`), а `NetworkSource` читає опис нейромережі файлу, з якого будується нейромережа, - текст файлу cfg або опис, який зберігають файли dxxwjz2, dxxwjz3 і dxxwjz4 та їхні проекти, - і ключі його секції `[net]`. Класи читання і запису форматів однаково завантажують і зберігають ті самі масиви нейромережі, тож будь-який файл нейромережі завантажує ту саму нейромережу, а `read()` і `write()` документів - це те, що конвертує файли між форматами.

## Оригінальний файл ваг darknet

Бінарний файл, який читає і записує оригінальний Darknet: трійка версії int32 `major`, `minor` і `revision`, кількість зображень, які бачила нейромережа (uint64 `seen` для версій `(major * 10 + minor) >= 2` і uint32 для старіших), і масиви float шарів з вагами один за одним у порядку шарів. Файл не має власної структури: якому масиву належить яке значення float, визначає лише нейромережа (див. секцію [Дві нейромережі cfg](#дві-нейромережі-cfg)). Класи читання і запису бібліотеки - це перенесені на C++ оригінальні `load_weights_upto()`, `save_weights_upto()` і процедури окремих шарів, які читають і записують саме ті байти, що й оригінал: шари `dontload` і масиви нормалізації батчів шарів `dontloadscales` пропускаються, ваги згорткових шарів `flipped` і ваги повнозв'язних шарів файлів з мажорною чи мінорною версією понад 1000 транспонуються, обрізані файли і зайві байти після масивів зберігаються. Валідатор порівнює розмір файлу з масивами нейромережі: файл, розмір якого не відповідає ні збереженим, ні завантажуваним масивам, обрізаний або містить зайві байти (попередження), а стиснений gzip файл і файл, коротший за заголовок, - це помилки.

## Формат файлу dxxwjz1 першої версії

Файл dxxwjz1 - це стиснений gzip документ JSON у кодуванні UTF-8 з вагами нейромережі, який розпаковують стандартні інструменти gzip (`gzip -dc model.dxxwjz1`) і читає будь-який інструмент для роботи з JSON:

```
{
  "format": "dxxwjz1",
  "version": 1,
  "generator": "DarknetxxDataDrivers-0.12 0.12.0",
  "header": {"major": 0, "minor": 2, "revision": 5, "seen": 64},
  "layers": [
    {"index": 0, "type": "convolutional", "arrays": {
      "biases": [0.5,-0.25],
      "weights": [0.125,"0x7fc00000",-0.0,1e-45]
    }},
    {"index": 1, "type": "maxpool"}
  ],
  "trailing": "AAE="
}
```

`header` - це заголовок оригінального файлу ваг, `layers` - об'єкти шарів з вагами з їхніми `index` і `type` (назва секції cfg шару), з масивами `arrays`, названими за полями шарів darknet (`biases`, `scales`, `rolling_mean`, `rolling_variance`, `weights`), або з `sublayers` рекурентних шарів, а `trailing` - текст base64 байтів, які оригінальний файл зберігає після масивів. Скінченні значення float - це найкоротший десятковий текст, з якого значення відновлюється точно, а нескінченності та NaN - рядки `"0x"` з їхніми бітовими шаблонами, тож кожне значення float файлу зберігається біт у біт, а класи читання знаходять масиви за їхніми шарами і назвами, тож файл завантажує ту саму нейромережу, що й оригінальний файл ваг. `generator` називає бібліотеку, яка записала файл, і її версію - `DarknetxxDataDrivers-0.12 0.12.0` і для darknetxx, і для бінарного файлу `darknetxxConverter`, - і має лише інформаційне значення. Стиснений файл займає приблизно в 1,35 раза більше місця, ніж оригінальний файл ваг.

## Формат файлу dxxwjz2 другої версії

Файл dxxwjz2 - це документ dxxwjz1 з усім, з чим нейромережу навчають і з чим вона розпізнає: членом `network` з текстом файлу cfg нейромережі і назвами класів (об'єктами з їхніми `index` і `name`) і членом `training` із записом навчання (`seconds`, які зайняло навчання, стовпці `iteration`, `loss` і `avg_loss` кожної ітерації навчання і стовпці `map_iteration` і `map` кожного обчислення mAP) поряд з вагами. Тож файл cfg нейромережі можна відновити з файлу будь-яким інструментом для роботи з JSON (`gzip -dc net.dxxwjz2 | jq -r .network.cfg > net.cfg`).

**Файл без ваг - це нейромережа, яку треба навчити.** Файл dxxwjz2 без ваг (без жодного значення float у його масивах) не завантажує жодних ваг, і нейромережа зберігає свою ініціалізацію: навчання навчає такий файл, тоді як розпізнавання його відхиляє (див. секцію [Навчання і формати](#навчання-і-формати)). Файл лише з `network` (без шарів: той, який конвертер створює з файлу cfg, див. [конвертування](#конвертування), або який експортує інструмент) - це нейромережа, яку треба навчити з нуля: ані seen його заголовка, ані його запис навчання не продовжуються.

## Формат файлу проекту dxxwjz2p

Файл dxxwjz2p - це документ dxxwjz2 проекту навчання YOLO4: опис нейромережі, ваги і запис навчання поряд з членом `annotations` з анотаціями зображень YOLO4 - документом проекту [ImagesAnnotator](https://github.com/yuriysydor1991/ImagesAnnotator) (його структури 0.12.0) з теками зображень (їхніми шляхами `db`, відносними до теки файлу проекту або абсолютними), зображеннями з їхніми `rel_path`, `width`, `height` і позначкою `validation` і рамками з їхньою назвою класу `name` і прямокутником `rect` у пікселях. Тож проект - це все, що потрібно навчанню, окрім самих зображень: єдиний файл dxxwjz2p з абсолютними шляхами до зображень або файл dxxwjz2p з відносними до нього шляхами поряд з теками зображень, який переміщується разом з ними як ціле. В усьому іншому файл - це файл dxxwjz2: розпізнавання виконує нейромережа самого проекту, конвертер створює з нього файл ваг dxxwjz2 без анотацій, файл dxxwjz2p без ваг - це нейромережа, яку треба навчити.

## Формат файлу dxxwjz3 третьої версії

Файл dxxwjz3 - це документ dxxwjz2, який складається лише з даних JSON: член `network` зберігає структуру JSON cfg нейромережі замість тексту файлу cfg - об'єкт `net` з опціями секції `[net]`, масив `layers` секцій шарів з їхніми `type` і `options` разом з масивами `arrays` їхніх шарів, масив `classes` з назвами класів, - а хвостові байти - це масив JSON їхніх значень, тож будь-який інструмент для роботи з JSON читає і змінює кожну опцію нейромережі (`gzip -dc net.dxxwjz3 | jq .network.net.batch`). Опції - це значення JSON опцій darknet: цілі числа, дроби значень float, рядки, масиви значень, розділених комами, і `null` для рядка опції без `=`.

Класи читання перетворюють член network у текст cfg нейромережі, тож решта darknetxx працює з текстом cfg файлу dxxwjz3 так само, як з текстом файлу dxxwjz2, а прапорець `--dump-cfg` його друкує (див. секцію [Друк cfg нейромережі файлу](/doc/sections/uk_UA/6-2-4-converting-the-network-weights-files.md#друк-cfg-нейромережі-файлу)). JSON не зберігає ні коментарів і пробільних символів файлу cfg, ні повторених ключів секції: текст cfg, конвертований у файл dxxwjz3 і назад, - це та сама нейромережа нормалізованого тексту cfg, який знову конвертується у той самий файл dxxwjz3. В усьому іншому файл dxxwjz3 - це файл dxxwjz2: навчання зберігає його за замовчуванням (`--save-format dxxwjz3`), конвертер конвертує його в інші формати і назад з вагами тих самих байтів, файл без ваг - це нейромережа, яку треба навчити, та, яку ImagesAnnotator записує поряд з cfg теки YOLO4, яку він експортує (див. секцію [Файли ImagesAnnotator](#файли-imagesannotator)).

## Формат файлу проекту dxxwjz3p

Файл dxxwjz3p - це проект dxxwjz2p з документом dxxwjz3, тож увесь проект навчання складається лише з даних JSON: член network і хвостові байти - це члени dxxwjz3, а член annotations - член dxxwjz2p. ImagesAnnotator експортує свої анотації у проект dxxwjz3p з нейромережею, яку треба навчити (див. секцію [Файли ImagesAnnotator](#файли-imagesannotator)). В усьому іншому файл - це файл dxxwjz2p: навчання `--train-cfg project.dxxwjz3p` навчає на зображеннях його анотацій і оновлює проект, GUI навчання його відкриває, розпізнавання виконує його нейромережа, конвертер конвертує його у проект dxxwjz2p і назад з тими самими вагами і анотаціями та у файл ваг dxxwjz3 без анотацій.

## Формат файлу dxxwjz4 четвертої версії

Файл dxxwjz4 - це документ dxxwjz3 з членом `description`: коментарями й описом нейромережі та її даних, які пишуть користувачі, будь-якої довжини (включно з мегабайтами тексту), рядком JSON з текстом UTF-8 з будь-якими рядками, лапками і мовами, який класи запису записують перед нейромережею, тож його видно, щойно глянути на початок файлу (`gzip -dc net.dxxwjz4 | head -c 2000`). Опис конвертування - це параметр командного рядка `--convert-description <TEXT>`, який має перевагу над описом джерела (див. секцію [Конвертування файлів ваг нейромереж](/doc/sections/uk_UA/6-2-4-converting-the-network-weights-files.md)); файли попередніх форматів не зберігають опису, тож конвертування в них відкидає його з попередженням. Файл dxxwjz4 - це нейромережа, яку навчання проекту dxxwjz4p зберігає поряд з проектом (див. секцію [Навчання проекту dxxwjz4p](#навчання-проекту-dxxwjz4p)), а навчання і розпізнавання читають його так само, як файл dxxwjz3.

## Формат файлу проекту dxxwjz4p

Файл dxxwjz4p - це **весь проект навчання в одному файлі**: нейромережа, ваги, запис навчання, текст опису, анотації зображень **і самі зображення**, тож проект копіюють, пересилають і архівують як один файл, і він навчається на будь-якій машині без тек зображень поряд. Файл - це стандартний архів ZIP (з записами Zip64 для членів і архівів розміром 4 ГіБ і більше) з єдиним документом JSON члена `project.json`, першого в архіві, і копіями зображень його анотацій у теці `data/` (однакові назви зображень з різних тек отримують суфікс `_<n>`), тож будь-який інструмент zip перелічує і розпаковує його зовсім без darknetxx:

```
$ unzip -l cars.dxxwjz4p
Archive:  cars.dxxwjz4p
  Length      Date    Time    Name
---------  ---------- -----   ----
  9410833  2026-11-02 10:00   project.json
        0  2026-11-02 10:00   data/
   184220  2026-11-02 10:00   data/park.jpg
   201311  2026-11-02 10:00   data/park_1.jpg
    96112  2026-11-02 10:00   data/street.png
---------                     -------
  9892476                     5 files
```

Документ JSON - це документ dxxwjz3p з членом опису файлу dxxwjz4, а шляхи db анотацій відносні до кореня архіву, тож читається і проект, запакований вручну з документа JSON і зображень (`cd cars && zip -r ../cars.dxxwjz4p project.json data`). Класи читання відхиляють усе, що не є повним проектом з його зображеннями, безпечно розпакованим: члени зі шляхами поза проектом, члени інших типів, ніж звичайні файли і теки, повторювані, зашифровані члени і члени з методами стиснення, іншими, ніж stored і deflated, пошкоджені й обрізані архіви, зображення анотацій, для яких в архіві немає члена.

### Розпаковані проекти

Проект читається з теки, у яку його розпаковано: теки `darknetxx-dxxwjz4p-<name>-<random>` тимчасової директорії (тієї, що в змінній середовища `TMPDIR`, а якщо її немає - `/tmp`) з доступом лише для власника. Проект розпаковується, коли процес читає його вперше і коли файл змінився, тож читання того самого проекту (перегляд нейромережі, завантаження ваг, навчання) спільно використовують теку, яку видаляють наприкінці процесу:

```
The /srv/cars.dxxwjz4p project is unpacked into /tmp/darknetxx-dxxwjz4p-cars-5f0c1e93a7d2b4c6
```

Зображення прочитаних анотацій - це зображення теки, тож проект dxxwjz4p, конвертований у проект dxxwjz3p чи dxxwjz2p (лише зі шляхами до зображень), спершу отримує копії зображень поряд з файлом призначення: `data/park.jpg` проекту - це `data/park.jpg` поряд з файлом `out/cars.dxxwjz3p`, та сама розкладка розпакованого архіву. Наявне зображення з іншим вмістом не замінюється, натомість конвертування не вдається.

## Тека навчання YOLO4

Тека навчання YOLO4, та, яку експортує ImagesAnnotator, - це формат `yolo4`, який використовує лише конвертування: формат без розширення назви файлу, тож ні навчання, ні розпізнавання не приймають теку як файл ваг. Клас читання бібліотеки читає теку в документ проекту dxxwjz2p так, як її читає навчання: першу мережу `cfg/*.cfg`, класи `data/obj.names`, зображення списків `train=` і `valid=` з cfg з їхніми розмірами і рамками з їхніх файлів міток, найновіші ваги теки резервних копій (оригінальний файл цих ваг читається за структурою cfg теки, `CfgNetworkStructure`, якщо конвертування не задає власного файлу cfg). Клас запису записує документ у нову теку з нейромережею `cfg/<name>.cfg`, файлами `data/obj.names`, `data/obj.data`, `data/train.txt` і `data/val.txt`, копіями зображень у теці `data/obj/` з їхніми мітками і вагами `backup/<name>_last.dxxwjz3`; наявна тека з файлами не може бути місцем призначення, тож жодна тека користувача не видаляється. Тека, конвертована у файл dxxwjz2p і назад, має ті самі списки, мітки, назви класів і ваги.

## Файли ImagesAnnotator

[ImagesAnnotator](https://github.com/yuriysydor1991/ImagesAnnotator), анотатор зображень, який рекомендує darknetxx, сам генерує файли найновіших форматів: підменю `DarknetXX` його меню експорту (а також формати параметра `--convert-to` його командного рядка) записує анотації його проекту в чотири розкладки бібліотеки [ImagesAnnotator-DataExporters](https://github.com/yuriysydor1991/ImagesAnnotator-DataExporters), які darknetxx навчає без жодного конвертування:

| Пункт меню (формат) | Розкладка | Навчання всередині неї |
| --- | --- | --- |
| `dxxwjz3p project` (`dxxwjz3p`) | проект `yolov4-obj.dxxwjz3p` поряд з текою `images/` з копіями зображень і порожньою текою `backup/` | `darknetxx --train-cfg yolov4-obj.dxxwjz3p` |
| `dxxwjz2p project` (`dxxwjz2p`) | те саме з проектом `yolov4-obj.dxxwjz2p` | `darknetxx --train-cfg yolov4-obj.dxxwjz2p` |
| `dxxwjz3 with YOLO4 folder` (`dxxwjz3`) | тека навчання YOLO4 з нейромережею `cfg/yolov4-obj.dxxwjz3`, яку треба навчити, поряд з її `cfg/yolov4-obj.cfg` | `darknetxx --train-cfg cfg/yolov4-obj.dxxwjz3` |
| `dxxwjz2 with YOLO4 folder` (`dxxwjz2`) | те саме з нейромережею `cfg/yolov4-obj.dxxwjz2`, яку треба навчити | `darknetxx --train-cfg cfg/yolov4-obj.dxxwjz2` |

Графічний інтерфейс навчання теж відкриває ці проекти і теки. Нейромережа - це нейромережа YOLO v4 теки YOLO4, яку експортує ImagesAnnotator, записана для класів проекту (відсортованих назв його анотацій), без ваг, тож навчання починається з нуля. Файли - це ті, які записує darknetxx, байт у байт, окрім ключа `generator`: нейромережі тек - це ті, які конвертер створює з файлів cfg і назв класів теки (`--convert-to dxxwjz3 --convert-dst <file> --convert-cfg cfg/yolov4-obj.cfg --convert-names data/obj.names`), а проекти - ті, які конвертер записує з них назад. Анотації проекту - це цілочислові прямокутники в пікселях зображень теки `images`, відносної до файлу проекту, тож теку проекту можна переміщувати цілком, і жодне зображення не є зображенням для перевірки: mAP обчислюється на зображеннях навчання.

Підменю `DarknetXX` меню імпорту (а також формати параметра `--convert-from`) читає чотири розкладки назад за допомогою бібліотеки [ImagesAnnotator-DataImporters](https://github.com/yuriysydor1991/ImagesAnnotator-DataImporters): анотації проектів dxxwjz2p і dxxwjz3p, включно з тими, в які навчання зберегло ваги, і теки YOLO4 з назвами класів їхньої нейромережі, якщо файлу `data/obj.names` немає. Імпорт відхиляє документи іншого формату чи версії і пропускає ваги частинами, тож проект із сотнями мегабайтів ваг читається в обсязі пам'яті його анотацій. Файли dxxwjz1 і оригінальні файли ваг містять лише ваги, тож розкладок для них немає.

```
# проект анотацій ImagesAnnotator у проект dxxwjz3p
ImagesAnnotatorConverter \
  --convert-from imagesannotator \
  --convert-src ./cars.json \
  --convert-to dxxwjz3p \
  --convert-dst ./cars

# навчання всередині теки проекту зберігає ваги у проект
(cd ./cars && darknetxx --train-cfg yolov4-obj.dxxwjz3p)

# анотації навченого проекту назад у проект анотацій
ImagesAnnotatorConverter \
  --convert-from dxxwjz3p \
  --convert-src ./cars/yolov4-obj.dxxwjz3p \
  --convert-to imagesannotator \
  --convert-dst ./cars-trained.json
```

Перевірено 2026-10-11 з ImagesAnnotator `bc4b605`, ImagesAnnotator-DataExporters `d730625` (0.12.0) і ImagesAnnotator-DataImporters `814d224` (0.14.0).

## Незалежна від формату модель

Формати читають файли в незалежну від формату модель бібліотеки і записують їх з неї, а darknetxx працює з її публічними заголовними файлами:

- **`WeightsDocument`** - вміст файлу ваг: `WeightsHeader`, шари з їхніми масивами і байти після масивів, які зберігають кожен байт оригінального файлу ваг (включно з обрізаними масивами, частиною значення float, якою закінчується обрізаний файл, і зайвими байтами), поряд з `NetworkDescription` (текст cfg і назви класів), `TrainingRecord` (секунди, втрати ітерацій і mAP обчислень mAP), `Yolo4Annotations` з `Yolo4Image` і `Yolo4Box` (анотації проектів) і текстом `description`, який зберігають файли dxxwjz4 і dxxwjz4p.
- **`WeightsSchema`** і **`WeightsLayout`** - масиви і підшари кожного типу шарів, їхній порядок і назви, `WeightsLayoutOptions` і `WeightsLayoutEntry` масивів нейромережі у порядку файлів, які нейромережа створює сама (`NetworkWeights::layout_of()` для нейромереж darknet, обхід секцій cfg у `CfgNetworkStructure`).
- **`NetworkFiles`** - читання тексту файлу cfg і назв з файлу назв класів darknet.
- **`WeightsReport`** - результати валідаторів бібліотеки: помилки, через які файл неможливо завантажити чи конвертувати, і попередження про файли, які завантажуються, але не так, як могли очікувати їхні автори (обрізаний файл, зайві байти, невідомі ключі документа JSON).

## Конвертування

**`WeightsConverter`** бібліотеки виконує конвертування **`WeightsConversion`** файлу одного формату в інший (або в той самий): спершу створюється нейромережа з файлу cfg (оригінальний файл ваг її потребує, а решта, якщо файл cfg вказано, перевіряється на відповідність їй), потім файл-джерело перевіряється і читається у документ, а документ записується у тимчасовий файл у директорії призначення, який після завершення запису замінює файл призначення. Невдале конвертування залишає файл призначення таким, яким він був, і не залишає по собі тимчасового файлу, а `stop_requested` конвертування (запит зупинки програми, натискання Ctrl+C) зупиняє конвертування між його кроками.

Нейромережа з файлу cfg - це нейромережа `create_network()` конвертера: darknetxx конвертує за допомогою `NetworkWeightsConverter` нейромережею darknet з cfg (його створює `darknetxx_impl::WeightsConversionLib` бібліотеки darknetxx з параметрів конвертування контексту бібліотеки), а бінарний файл `darknetxxConverter` бібліотеки - за допомогою `CfgNetworkStructure` секцій cfg, і тест `CTEST_NetworkWeights` перевіряє, що їхні розкладки однакові (див. секцію [Дві нейромережі cfg](#дві-нейромережі-cfg)). Тека-джерело YOLO4 без файлу cfg конвертування в обох бінарних файлах читає оригінальний файл ваг своєї теки резервних копій за `CfgNetworkStructure` cfg теки.

Решта конвертування - це конвертування бібліотеки: оригінальний файл ваг, конвертований у файл dxxwjz1, dxxwjz2 чи dxxwjz3 і назад, байт у байт збігається з початковим, хай що він містить; файли призначення dxxwjz2, dxxwjz3 і dxxwjz4 зберігають опис нейромережі (текст cfg з файлу cfg конвертування замість тексту джерела, назви класів з файлу назв класів конвертування замість класів джерела, а ті - замість файлу `names=` з cfg) і запис навчання джерела; файли dxxwjz4 і dxxwjz4p - текст опису з `--convert-description` замість опису джерела; файли dxxwjz2p, dxxwjz3p і dxxwjz4p - анотації джерел-проектів і тек-джерел YOLO4 (файл призначення dxxwjz4p пакує їхні зображення, а файли призначення dxxwjz2p і dxxwjz3p джерела dxxwjz4p отримують їхні копії поряд, див. секцію [Розпаковані проекти](#розпаковані-проекти)), тоді як проект, конвертований у файл dxxwjz2 чи dxxwjz3, - це файл ваг проекту для розпізнавання, без анотацій. Конвертування без файлу-джерела створює з файлу cfg нейромережу, яку треба навчити: файл лише з описом нейромережі, без шарів, який конвертується лише у формати, що зберігають опис нейромережі. Командний рядок конвертування описано у секції [Конвертування файлів ваг нейромереж](/doc/sections/uk_UA/6-2-4-converting-the-network-weights-files.md).

## Збереження зміненої нейромережі

**`NetworkRewriter`** ([`NetworkRewriter.h`](/src/NetworksLoaders/network-rewriter/NetworkRewriter.h)) цього проекту записує змінений опис нейромережі (текст cfg і назви класів, які редагують редактори нейромереж, див. секцію [Бінарні файли редактора нейромереж](/doc/sections/uk_UA/5-38-the-network-editor-binaries.md)) у файл формату, що його зберігає (наприклад, проект dxxwjz3p, за розширенням назви файлу призначення), через тимчасовий файл так само, як це робить конвертування (`WeightsConverter::write_destination()` бібліотеки darknetxx-datadrivers), - функція `LibraryFacade::save_network()` бібліотеки darknetxx. Він залишається в цьому проекті, бо перевіряє зміну нейромережею darknet зі зміненого cfg, якої бібліотека darknetxx-datadrivers не має. Вихідний файл (відкритий проект, файл, з якого зберігають як інший, чи жоден для нової нейромережі) переходить у файл призначення: анотації, запис навчання, заголовок і ваги; вихідний файл і файл призначення можуть бути одним файлом.

Спершу darknet будує нейромережу cfg (нейромережу виявлення одного зображення, `OrigDefaultLoader::parse_network_text()`), тож cfg, яку darknet відкидає, не зберігається, як і нейромережа, якої навчання не виконає: шар `[route]` вхідних шарів різних розмірів (про який розбір попереджає, а перша зміна розміру під час навчання відкидає) і шар без виходів. Ваги шару лишаються, доки кожен шар до нього зберігає свою секцію cfg (тип і параметри вихідної cfg), а його масиви мають розміри масивів побудованої нейромережі (її `NetworkWeights::layout_of()`, розміри якого задають і `channels`, `width` та `height` секції `[net]`): тож зміна лише параметрів `[net]` зберігає всі ваги, запис навчання і побачені зображення, а зміна шару відкидає ваги цього шару і всіх наступних. Документ з відкинутими вагами - це нейромережа для навчання зі збереженими вагами, для перенавчання (як `partial` і `-clear` darknet): без побачених зображень, запису навчання і хвостових байтів, а без жодних ваг - нейромережа для навчання з нуля. Збереження повідомляє про відкинуті ваги:

```
The weights of the layer 11 and of the ones after it were dropped, the network changed them
```

Зображення анотацій у пам'яті - це абсолютні шляхи вихідного файлу, тож проект, збережений в іншу теку, і далі посилається на зображення вихідної теки (теки під текою файлу призначення записуються відносно неї).

## Навчання і формати

Навчання за замовчуванням зберігає свої ваги у файли dxxwjz3 (параметр командного рядка `--save-format` задає їхній формат, у бібліотеці - `LibraryContext::save_format`): файли `<base>_<ітерація>_<середня втрата>` (нумеровані, із середньою втратою (avg loss) їхньої ітерації, наприклад `net_1200_0.734521`), `<base>_last`, `<base>_final`, `<base>_best` і `<base>_ema` директорії резервних копій з розширенням формату. Код навчання мовою C зберігає їх через `weights_saver` свого контексту, який `DarknetContext` спрямовує до журналу навчання **`TrainingJournal`** ([`TrainingJournal.h`](/src/darknet-adaptor/adaptors/TrainingJournal/TrainingJournal.h)):

- журнал починається з опису нейромережі навчання: тексту файлу cfg нейромережі навчання (або файлу dxxwjz2) і назв класів з файлу `--names`, з файлу ваг dxxwjz2, з файлу нейромережі dxxwjz2 чи з файлу `names=` з cfg - з першого з них, що вказаний;
- він продовжує запис навчання файлу ваг dxxwjz2, з якого починається навчання, до ітерації цих ваг (запис ітерацій після неї відкидається, а `--clear` відкидає весь запис разом з ітераціями), а секунди його годинника починаються з секунд запису;
- він записує втрати кожної ітерації і mAP кожного обчислення mAP один раз, у зростаючому порядку ітерацій, включно з ітерацією, збереженою до звіту про неї, і зберігає кожен файл ваг з часом навчання на цей момент.

Тож файл навчання, продовженого з файлу dxxwjz2, зберігає втрати і mAP кожної ітерації навчання у їхньому порядку і весь час усіх навчань, а середні втрати продовженого навчання продовжують останні середні втрати запису. Графік кожного запуску навчання - це новий файл `chart_<base>_<РРРР-ММ-ДД--ГГ-ХХ-СС>.png` з датою і часом запуску, тож жоден графік продовжених раніше навчань не перезаписується. На ньому заново малюється весь запис продовжених ваг (середні втрати і лінія mAP), і графік одразу зберігається, далі - кожні 100 ітерацій (кожну ітерацію з `--save-charts-every-iter`), наприкінці і при зупинці навчання; зображення `--chart-path` продовжується лише для ваг без запису (оригінальних і dxxwjz1). Графік запису файлу dxxwjz2 можна також намалювати будь-коли (параметр командного рядка `--draw-chart`, клас `TrainingChart` і `LibraryFacade::training_chart()` бібліотеки - графік, який GUI показує одразу, щойно відкрито директорію навчання).

Обчислення mAP навчання (`--map`) бере назви класів так само, тож назвам класів файлу dxxwjz2 не потрібен файл назв; назви, кількість яких відрізняється від кількості класів нейромережі, зупиняють навчання з помилкою.

Розпізнавання і навчання читають файли ваг кожного формату, формат визначає розширення назви файлу, а файл нейромережі - це файл cfg або dxxwjz2 (файли dxxwjz3 і dxxwjz4 та проекти dxxwjz3p і dxxwjz4p читаються так само, як файли dxxwjz2 і проекти dxxwjz2p):

| Командний рядок | Нейромережа | Ваги |
| --- | --- | --- |
| `--train-cfg net.cfg --weights w` | з `net.cfg` | з файлу `w` будь-якого формату, запис навчання файлу dxxwjz2 продовжується |
| `--train-cfg net.dxxwjz2` | з cfg, який зберігає файл | з файлу, жодних для файлу без ваг (навчання з нуля) |
| `--detect-cfg net.cfg --weights w` | з `net.cfg` | з файлу `w` будь-якого формату, файл dxxwjz2 без ваг відхиляється |
| `--detect-cfg net.dxxwjz2` | з cfg, який зберігає файл | з файлу |
| лише `--weights net.dxxwjz2` | з cfg, який зберігає файл | розпізнавання з вагами файлу, навчання нейромережі файлу без ваг |
| `--train-cfg project.dxxwjz2p` | з cfg, який зберігає файл | з файлу, навчання на зображеннях анотацій файлу, яке оновлює цей файл (див. нижче) |
| `--detect-cfg project.dxxwjz2p`, лише `--weights project.dxxwjz2p` | з cfg, який зберігає файл | такі самі, як для файлу dxxwjz2 |
| `--train-cfg project.dxxwjz4p` | з cfg, який зберігає проект | з файлу `project.dxxwjz4` поряд з проектом, якщо він є, інакше з проекту; навчання на зображеннях проекту, розпакованого у тимчасову директорію, яке зберігає файл `project.dxxwjz4` (див. секцію [Навчання проекту dxxwjz4p](#навчання-проекту-dxxwjz4p)) |

Резервні копії навчання названі за файлом його нейромережі: `<base>` файлів `net.cfg` і `net.dxxwjz2` - це `net`, і `<base>` резервних файлів навчання (файлів dxxwjz2 `net_last`, `net_final`, `net_best`, `net_ema` і `net_<ітерація>_<середня втрата>` з ітерацією їхніх ваг, а також `net_<ітерація>` попередніх версій) - теж `net`, тож навчання, продовжене з самого лише резервного файлу, продовжує ті самі файли.

## Навчання проекту dxxwjz2p

Проект dxxwjz3p навчається так само, з файлом dxxwjz3p його найновіших ваг. Навчання файлу dxxwjz2p (командний рядок `--train-cfg project.dxxwjz2p`, проект dxxwjz2p, який відкриває GUI навчання) навчає нейромережу файлу на зображеннях його анотацій, продовжуючи від його ваг: списки зображень і мітки навчання беруться з анотацій замість списків `train=` і `valid=` з cfg і файлів міток поряд із зображеннями. `OrigDefaultLoader` вказує їх нейромережам навчання як списки, а `DarknetContext` навчання публікує їх у `TrainingDataRegistry` директорії [`TrainingData`](/src/darknet-adaptor/adaptors/TrainingData) і відкликає наприкінці навчання. Код darknet мовою C читає їх функціями `get_paths()` (за назвами списків `dxxwjz2p:train` і `dxxwjz2p:valid`) і `read_boxes()` (за шляхами до міток зображень), які звертаються до реєстру раніше, ніж до файлів (функції C `get_training_data_paths()`, `has_training_data_paths()` і `read_training_data_boxes()` з `TrainingDataCApi`). Тож зображення залишаються там, де вони є, проект не зберігає їхніх копій і жоден файл списку чи міток не записується.

Кожне збереження навчання також оновлює проект: файл dxxwjz2p з найновішими вагами, записом навчання і анотаціями записується у файл `<project>.saving` поряд з проектом, який після завершення запису замінює проект. Тож навчання проекту продовжується від його найновіших ваг, тоді як тека резервних копій поряд з проектом, яку називає ключ `backup=` його cfg, зберігає резервні копії формату збереження, а графіки малюються поряд з проектом.

## Навчання проекту dxxwjz4p

Навчання проекту dxxwjz4p (командний рядок `--train-cfg cars.dxxwjz4p`, проект dxxwjz4p, який відкриває GUI навчання) навчає так само, як навчання проекту dxxwjz3p (див. вище), з проекту, розпакованого у тимчасову директорію: darknetxx розпаковує проект у теку `darknetxx-dxxwjz4p-cars-<random>` директорії `/tmp` (або `TMPDIR`) на початку навчання і читає документ JSON і зображення звідти, а наприкінці процесу навчання тека видаляється. Проект зберігає зображення, тож кожне збереження навчання його не перезаписує (архів з гігабайтами зображень), натомість файли навчання розміщуються поряд з ним:

| Файл | Містить |
| --- | --- |
| `cars.dxxwjz4p` | проект, який читає навчання і якого навчання не змінює |
| `cars.dxxwjz4` | нейромережу, яку навчає навчання: найновіші ваги, запис навчання і опис проекту, які записує кожне збереження навчання (через файл `cars.dxxwjz4.saving`); навчання проекту продовжується з нього, якщо він є |
| `chart_cars_<РРРР-ММ-ДД--ГГ-ХХ-СС>.png` | графік кожного запуску навчання, поряд з проектом, хай якою є робоча директорія навчання |
| тека резервних копій, яку називає ключ `backup=` з cfg | резервні копії формату збереження (`--save-format`), відносна тека - відносно робочої директорії |

```
$ darknetxx --train-cfg /srv/projects/cars.dxxwjz4p --dont-show
The /srv/projects/cars.dxxwjz4p project is unpacked into /tmp/darknetxx-dxxwjz4p-cars-5f0c1e93a7d2b4c6
The network of the /srv/projects/cars.dxxwjz4p project is saved into /srv/projects/cars.dxxwjz4, the charts beside it
...
$ ls /srv/projects
cars.dxxwjz4  cars.dxxwjz4p  chart_cars_2026-11-02--10-00-00.png
```

Файл `cars.dxxwjz4` - це нейромережа лише для розпізнавання (`--weights cars.dxxwjz4`), а конвертер за потреби створює з його ваг і анотацій проекту новий проект. Навчання, продовжене з файлу `cars.dxxwjz4`, - це навчання нейромережі з cfg, яку зберігає проект: нейромережа проекту, змінена відтоді (шари інших розмірів), не завантажує ваг попередньої, а коли файл `cars.dxxwjz4` з ними відсунуто вбік, навчання знову починається з ваг проекту.

## Використання форматів

Код C++ проекту завантажує і зберігає ваги нейромережі darknet у будь-якому форматі через інтерфейси бібліотеки і подання `NetworkWeights` нейромережі:

```
#include "src/NetworksLoaders/DataDriversAliases.h"
#include "src/NetworksLoaders/network-weights/NetworkWeights.h"

using darknet_adaptor::loaders::NetworkWeights;

// net - нейромережа darknet ANetworkAdaptor (networkptr, який повертає його
// метод get()), решта (rest) тримає опис нейромережі і запис навчання
dxxdd::WeightsDocument rest;
rest.network = dxxdd::NetworkDescription{cfg_text, {"cat", "dog"}};

const dxxdd::IWeightsFileFormatPtr format =
    dxxdd::WeightsFileFormats::named("dxxwjz3");

format->create_writer()->save(NetworkWeights{*net}, "backup.dxxwjz3", net->n,
                              false, rest);
dxxdd::WeightsFileFormats::of_file("backup.dxxwjz3")
    ->create_reader()
    ->load(NetworkWeights{*net}, "backup.dxxwjz3", net->n, rest);

// весь файл, прочитаний у документ і перевірений на відповідність нейромережі
dxxdd::WeightsDocument document;
const NetworkWeights view{*net};

dxxdd::WeightsFileFormats::of_file("backup.dxxwjz3")
    ->create_reader()
    ->read("backup.dxxwjz3", &view, document);
```
