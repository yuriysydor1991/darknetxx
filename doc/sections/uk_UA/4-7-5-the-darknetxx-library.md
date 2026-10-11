# Бібліотека darknetxx

Уся функціональність проекту - навчання і розпізнавання darknet,
конвертування файлів ваг нейромереж, фонове навчання тек YOLO4 та проектів
dxxwjz2p і dxxwjz3p і розпізнавання зображень нейромережами файлів ваг - це спільна
бібліотека `darknetxx-0.11` директорії [`src/lib`](/src/lib), впорядкована
так, як гілка `applib` проекту
[cpp-app-template](https://github.com/yuriysydor1991/cpp-app-template)
впорядковує свою бібліотеку. Виконувані файли проекту (`darknetxx`,
`darknetxxConverter`, `darknetxx-train-gui` і `darknetxx-recognition-gui`)
користуються бібліотекою лише через її фасад, так само як будь-яка інша
програма (див. секцію
[Використання бібліотеки darknetxx](/doc/sections/uk_UA/5-37-using-the-darknetxx-library.md)).

Окремий вихідний файл діаграми нижче -
[`doc/diagrams/library-classes.puml`](/doc/diagrams/library-classes.puml).

@startuml
hide empty members

package "darknetxx011 (the installed facade/public headers)" {
  class LibraryFacade {
    +{static} create_library_context() : LibraryContextPtr
    +{static} create_library(ctx) : ILibPtr
    +{static} create_trainer(ctx) : ITrainerPtr
    +{static} open_training_project(path) : ATrainingProjectPtr
    +{static} create_recognizer() : IRecognizerPtr
    +{static} conversion_formats() : FormatsDescriptions
    +{static} weights_extensions() : string list
    +{static} training_chart(weights) : PNG bytes
    +{static} network_cfg(weights) : string
    +{static} init_logger(logger)
  }

  class LibraryContext {
    +train_cfg / detect_cfg / latest_weights : string
    +threads, thresh, ... (the command line ones)
    +convert_from / convert_src / convert_to : string
    +convert_dst / convert_cfg : string
    +training_listener : ITrainingListenerPtr
    +conversion_requested() : bool
    +request_stop() / withdraw_stop()
    +stop_requested() : bool
    +stop_with(flag)
  }

  interface ILib {
    +libcall(ctx) : bool
  }

  interface ITrainer {
    +train(project) : bool
    +stop()
    +running() : bool
  }

  interface ITrainingListener {
    +on_iteration(progress)
    +on_finished(succeeded)
  }

  class TrainingProgress {
    +done_fraction() : double
    +iterations_text() / loss_text() / details_text() : string
  }

  abstract class ATrainingProject {
    +valid() : bool
    +path() : path
    +cfg() / latest_weights() : string
    +chart() : string
    +enter(ctx) : bool
    #{abstract} names() : string
  }

  class Yolo4Folder

  interface IRecognizer {
    +recognize(ctx) : Recognition
  }

  class Recognition {
    +error : string
    +objects : RecognizedObject list
    +png : PNG bytes
    +seconds : double
    +succeeded() : bool
    +summary_text() : string
  }

  class RecognizedObject {
    +name : string
    +probability : float
    +x, y, width, height : int
    +boxed() : bool
    +text() : string
  }

  interface "logger::ILogger" as ILogger
}

package "darknetxx_impl (not installed)" {
  class LibFactory
  class LibMain
  class WeightsConversionLib
  class BackgroundTrainer
  class DxxwjzProject
  class NetworkRecognizer
  interface IRecognitionMethod
  class ObjectsDetection
  class ImageClassification
  class RecognitionPainter
}

ILib <|.. LibMain
ILib <|.. WeightsConversionLib
ITrainer <|.. BackgroundTrainer
IRecognizer <|.. NetworkRecognizer
IRecognitionMethod <|.. ObjectsDetection
IRecognitionMethod <|.. ImageClassification
LibraryFacade ..> LibFactory : creates by
LibFactory ..> LibMain : training and detection
LibFactory ..> WeightsConversionLib : conversion
LibFactory ..> BackgroundTrainer : trainer
LibFactory ..> NetworkRecognizer : recognizer
NetworkRecognizer --> IRecognitionMethod : the one of the network layers
NetworkRecognizer ..> RecognitionPainter : boxes the objects by
NetworkRecognizer ..> "darknet_adaptor::loaders::OrigDefaultLoader" : loads by
NetworkRecognizer ..> Recognition : gives
Recognition --> RecognizedObject
BackgroundTrainer --> LibMain : runs in it's thread
LibMain ..> "darknet_adaptor::DarknetController" : performs by
WeightsConversionLib ..> "darknet_adaptor::loaders::WeightsConverter" : converts by
LibraryContext --> ITrainingListener : reports to
ITrainingListener ..> TrainingProgress
ATrainingProject <|-- Yolo4Folder
ATrainingProject <|-- DxxwjzProject
LibraryFacade ..> ATrainingProject : opens of the folder or the project file
BackgroundTrainer ..> ATrainingProject : trains
LibraryFacade ..> ILogger : adopts
@enduml

## Фасад

Заголовки [`src/lib/facade/public`](/src/lib/facade/public) - це весь
встановлюваний інтерфейс бібліотеки у просторі імен `darknetxx011` (назва
проекту і його основна та додаткова версії):

- **`LibraryFacade`** - точка входу: створює контекст бібліотеки, роботу, про
  яку просить контекст, фоновий тренер, розпізнавач зображень, перелічує
  формати файлів ваг нейромереж, які приймає конвертування, малює зображення
  PNG графіка навчання, запис якого зберігає файл dxxwjz2 (`training_chart`),
  повертає текст cfg нейромережі, який зберігає файл dxxwjz2, dxxwjz2p, dxxwjz3
  чи dxxwjz3p (`network_cfg`), переймає журнал користувача бібліотеки,
  відкриває проекти навчання за їхніми шляхами (`open_training_project`) і
  перелічує розширення назв файлів ваг кожного формату (`weights_extensions`).
- **`LibraryContext`** - параметри роботи бібліотеки, названі так само, як
  параметри командного рядка `darknetxx`, слухач навчання і запит зупинки:
  власний (`request_stop()`) і прапорець користувача бібліотеки
  (`stop_with()`), той, який піднімає обробник сигналу ОС, наприклад.
- **`ILib`** - робота бібліотеки: `libcall(ctx)` виконує навчання,
  розпізнавання чи конвертування, яке називає контекст (спершу
  конвертування, потім `detect_cfg`, `train_cfg` і сам лише `latest_weights`
  файлу dxxwjz2, з яким працює розпізнавання або який навчається, див. секцію
  [Файли ваг нейромереж](/doc/sections/uk_UA/4-7-4-network-weights-files.md)).
  Навчання зберігає файли ваг формату `save_format`, за замовчуванням -
  файли dxxwjz3.
- **`ITrainer`**, **`ATrainingProject`** і **`Yolo4Folder`** - навчання
  проекту навчання (див. секцію
  [Бінарний файл darknetxx-train-gui](/doc/sections/uk_UA/5-36-the-GUI-binaries.md#бінарний-файл-darknetxx-train-gui))
  у власному потоці, продовжене від найновіших ваг проекту: теки YOLO4
  (`Yolo4Folder`) чи файлу проекту dxxwjz2p або dxxwjz3p; усі за їхніми
  шляхами відкриває `LibraryFacade::open_training_project()`.
- **`IRecognizer`**, **`Recognition`** і **`RecognizedObject`** -
  розпізнавання файлу зображення нейромережею файлу ваг будь-якого формату, з
  файлом cfg, якщо файл ваг не зберігає свою нейромережу (див. секцію
  [Бінарний файл darknetxx-recognition-gui](/doc/sections/uk_UA/5-36-the-GUI-binaries.md#бінарний-файл-darknetxx-recognition-gui)):
  об'єкти, які виявляє нейромережа з шарами виявлення об'єктів, або класи
  всього зображення для інших нейромереж, ймовірність яких більша за `thresh`
  контексту, зображення PNG з об'єктами в рамках і час, який зайняло
  розпізнавання, або причина невдачі. Нейромережа залишається завантаженою для
  наступних зображень з тими самими файлами.
- **`ITrainingListener`** і **`TrainingProgress`** - ітерації і завершення
  навчання, про які повідомляє потік, у якому навчання виконується.
- **`ILogger`** - журнал користувача бібліотеки, який
  `LibraryFacade::init_logger` передає бібліотеці, щоб уся програма вела
  журнал в одне місце. Це домовленість журналювання кожної бібліотеки,
  похідної від cpp-app-template, тому він не має ані простору імен, ані
  захисту включення бібліотеки.
- **`LibraryAPI.h`** - макрос `DARKNETXX_LIB_API` експортованих класів:
  бібліотека будується з прихованою видимістю символів, тож ніщо інше з неї
  не виходить за межі спільного об'єкта.

## Реалізація

Решта директорії [`src/lib`](/src/lib) приватна для бібліотеки:

| Директорія | Містить |
| --- | --- |
| [`libmain`](/src/lib/libmain) | `darknetxx_impl::LibFactory` робіт, які видає фасад, `LibMain` (навчання і розпізнавання за допомогою `DarknetController` після налаштування потоків OpenMP) і `WeightsConversionLib` (конвертування файлів ваг нейромереж) |
| [`training`](/src/lib/training) | `BackgroundTrainer`, `ATrainingProject`, `Yolo4Folder` і `DxxwjzProject` файлів проектів dxxwjz2p і dxxwjz3p |
| [`recognition`](/src/lib/recognition) | `NetworkRecognizer`, який розпізнає нейромережею для розпізнавання, створеною `OrigDefaultLoader`, за допомогою `ObjectsDetection` чи `ImageClassification` (реалізацій `IRecognitionMethod`) і обводить об'єкти рамками за допомогою `RecognitionPainter` |
| [`darknet-adaptor`](/src/lib/darknet-adaptor) | міст до оригінального коду Darknet (див. секцію [Компоненти адаптера Darknet](/doc/sections/uk_UA/4-7-2-darknet-adaptor-components.md)) |
| [`OMP`](/src/lib/OMP), [`zlib`](/src/lib/zlib), [`helpers`](/src/lib/helpers) | допоміжні компоненти (див. секцію [Допоміжні компоненти](/doc/sections/uk_UA/4-7-3-supporting-components.md)) |

Бібліотека має власну копію підсистеми журналювання
[`src/log`](/src/log), як і кожен виконуваний файл проекту: проксі
`default_logger::DefaultLogger` макросів журналювання передає записи
`logger::ILogger`, який він тримає, власному
`default_logger::RealDefaultLogger` чи перейнятому. Виконувані файли
передають свій журнал бібліотеці одразу після ініціалізації свого
журналювання, у методі `app::ApplicationFactory::run`.

Компонентні тести внутрішніх частин бібліотеки підключають ціль
`DarknetXXLibraryObjects` об'єктів, з яких складається бібліотека, а спільну
бібліотеку підключають виконувані файли і її користувачі.
