# Бібліотека darknetxx

Уся функціональність проекту - навчання і розпізнавання darknet,
конвертування файлів ваг нейромереж і фонове навчання тек YOLO4 - це
спільна бібліотека `darknetxx-0.11` директорії [`src/lib`](/src/lib),
впорядкована так, як гілка `applib` проекту
[cpp-app-template](https://github.com/yuriysydor1991/cpp-app-template)
впорядковує свою бібліотеку. Виконувані файли проекту (`darknetxx`,
`darknetxxConverter` і `darknetxx-gui`) користуються бібліотекою лише через її
фасад, так само як будь-яка інша програма (див. секцію
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
    +{static} conversion_formats() : FormatsDescriptions
    +{static} training_chart(weights) : PNG bytes
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
    +train(folder) : bool
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

  class Yolo4Folder {
    +valid() : bool
    +latest_weights() : string
    +chart() : string
    +enter(ctx) : bool
  }

  interface "logger::ILogger" as ILogger
}

package "darknetxx_impl (not installed)" {
  class LibFactory
  class LibMain
  class WeightsConversionLib
  class BackgroundTrainer
}

ILib <|.. LibMain
ILib <|.. WeightsConversionLib
ITrainer <|.. BackgroundTrainer
LibraryFacade ..> LibFactory : creates by
LibFactory ..> LibMain : training and detection
LibFactory ..> WeightsConversionLib : conversion
LibFactory ..> BackgroundTrainer : trainer
BackgroundTrainer --> LibMain : runs in it's thread
LibMain ..> "darknet_adaptor::DarknetController" : performs by
WeightsConversionLib ..> "darknet_adaptor::loaders::WeightsConverter" : converts by
LibraryContext --> ITrainingListener : reports to
ITrainingListener ..> TrainingProgress
BackgroundTrainer ..> Yolo4Folder : trains
LibraryFacade ..> ILogger : adopts
@enduml

## Фасад

Заголовки [`src/lib/facade/public`](/src/lib/facade/public) - це весь
встановлюваний інтерфейс бібліотеки у просторі імен `darknetxx011` (назва
проекту і його основна та додаткова версії):

- **`LibraryFacade`** - точка входу: створює контекст бібліотеки, роботу, про
  яку просить контекст, фоновий тренер, перелічує формати файлів ваг
  нейромереж, які приймає конвертування, малює зображення PNG графіка
  навчання, запис якого зберігає файл dxxwjz2 (`training_chart`), і переймає
  журнал користувача бібліотеки.
- **`LibraryContext`** - параметри роботи бібліотеки, названі так само, як
  параметри командного рядка `darknetxx`, слухач навчання і запит зупинки:
  власний (`request_stop()`) і прапорець користувача бібліотеки
  (`stop_with()`), той, який піднімає обробник сигналу ОС, наприклад.
- **`ILib`** - робота бібліотеки: `libcall(ctx)` виконує навчання,
  розпізнавання чи конвертування, яке називає контекст (спершу
  конвертування, потім `detect_cfg`, `train_cfg` і сам лише `latest_weights`
  файлу dxxwjz2, з яким працює розпізнавання або який навчається, див. секцію
  [Файли ваг нейромереж](/doc/sections/uk_UA/4-7-4-network-weights-files.md)).
  Навчання зберігає файли ваг формату `save_format`.
- **`ITrainer`** і **`Yolo4Folder`** - навчання теки YOLO4 (див. секцію
  [Бінарний файл darknetxx-gui](/doc/sections/uk_UA/5-36-the-darknetxx-gui-binary.md))
  у власному потоці, продовжене від найновіших ваг теки.
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
| [`training`](/src/lib/training) | `BackgroundTrainer` і `Yolo4Folder` |
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
