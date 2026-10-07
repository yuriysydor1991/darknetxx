# Компоненти проекту і архітектура

Дана секція описує доступні компоненти проекту `darknetxx`, їхню структуру
класів і призначення. Діаграми структури нижче написані мовою
[PlantUML](https://plantuml.com/), а їхні вихідні файли зберігаються у директорії
[`doc/diagrams`](/doc/diagrams), тому вони можуть відображатись як під час
побудови документації Doxygen, так і будь-яким окремим інструментом PlantUML.

Програма організована у декілька взаємодіючих рівнів:

1. [Компоненти виконання програми](/doc/sections/uk_UA/4-7-1-application-runtime-components.md) - інфраструктура, яка розбирає параметри командного рядка, будує контекст виконання і обирає програму для запуску, яка використовує бібліотеку darknetxx (простір імен `app`).
1. [Компоненти адаптера Darknet](/doc/sections/uk_UA/4-7-2-darknet-adaptor-components.md) - місток, який керує оригінальним кодом Darknet для навчання чи розпізнавання (простір імен `darknet_adaptor`).
1. [Допоміжні компоненти](/doc/sections/uk_UA/4-7-3-supporting-components.md) - контролер OpenMP, підсистема журналювання, контролер стиснення zlib і допоміжні утиліти.
1. [Файли ваг нейромереж](/doc/sections/uk_UA/4-7-4-network-weights-files.md) - оригінальні файли ваг darknet, файли dxxwjz1 (стиснені gzip документи JSON з вагами) і файли dxxwjz2 (опис нейромережі і запис навчання поряд з вагами), класи їх читання, запису і перевірки та конвертер між ними (простір імен `darknet_adaptor::loaders`).
1. [Бібліотека darknetxx](/doc/sections/uk_UA/4-7-5-the-darknetxx-library.md) - спільна бібліотека, до якої будуються три рівні вище, її встановлюваний фасад (простір імен `darknetxx011`) і її реалізація (простір імен `darknetxx_impl`).

Сигнали операційної системи, на які реагує уся програма, охоплює підкомпонент `src/app/signals-handlers` рівня `app`, описаний у секції [Обробка сигналів операційної системи](/doc/sections/uk_UA/4-8-handling-the-OS-signals.md).

## Огляд компонентів

Функція `main()` лише передає параметри командного рядка до
`app::ApplicationFactory`. Фабрика будує `app::ApplicationContext`, дозволяє
`app::CommandLineParser` заповнити його, після чого створює відповідного нащадка
`app::IApplication`. `app::Application` перетворює контекст на
`darknetxx011::LibraryContext` бібліотеки darknetxx за допомогою
`converters::ApplicationContext2LibraryContext` і виконує роботу, яку з нього
створює `darknetxx011::LibraryFacade` бібліотеки (див.
[Бібліотека darknetxx](/doc/sections/uk_UA/4-7-5-the-darknetxx-library.md)):
навчання чи розпізнавання за допомогою `darknet_adaptor::DarknetController`,
який обирає і запускає конкретного робітника Darknet (розпізнавач, тренер
тощо), або конвертування файлів ваг нейромереж, про яке просять прапорці
конвертування командного рядка, за допомогою
`darknet_adaptor::loaders::WeightsConverter`. Контролер OpenMP і підсистема
журналювання підтримують увесь цей процес. Другий бінарний файл проекту,
`darknetxxConverter`, будується з тих самих компонентів і виконує лише
конвертування (див.
[Бінарний файл darknetxxConverter](/doc/sections/uk_UA/5-35-the-darknetxxConverter-binary.md)),
а третій, `darknetxx-gui`, додає компонент `src/gui`, який навчає у своєму
вікні GTK 4 за допомогою фонового тренера бібліотеки (див.
[Бінарний файл darknetxx-gui](/doc/sections/uk_UA/5-36-the-darknetxx-gui-binary.md)).

Окремий вихідний файл діаграми нижче -
[`doc/diagrams/components-overview.puml`](/doc/diagrams/components-overview.puml).

@startuml
skinparam componentStyle rectangle
skinparam shadowing false

package "Executable entry" {
  [main()] as main
}

package "app (application infrastructure)" {
  [ApplicationFactory] as factory
  [CommandLineParser] as parser
  [ApplicationContext] as ctx
  [IApplication\n(+ Application,\nGuiApplication,\nHelpPrinter,\nVersionPrinter)] as iapp
  [ISignalsHandler\n(+ SignalsHandler,\nStop/Pause/Continue/\nReload/User/Ignored)] as signals
}

package "converters" {
  [ApplicationContext2LibraryContext] as conv
}

package "gui (darknetxx-gui)" {
  [GtkmmIniter /\nMainWindow] as gui
}

package "lib (darknetxx library)" {
  [LibraryFacade] as facade
  [LibraryContext] as lctx
  [ILib\n(+ LibMain,\nWeightsConversionLib)] as ilib
  [ITrainer\n(+ BackgroundTrainer,\nYolo4Folder)] as trainer
}

package "darknet-adaptor (Darknet bridge)" {
  [DarknetController] as dctrl
  [DarknetContext] as dctx
  [IDarknetAdaptor\n(+ ADetector,\nADetectorTrainer)] as adaptor
  [Network infrastructure\n(NetworksLoaders, Layers,\nALayer/ANetwork adaptors,\nNetworksCApi)] as netinfra
  [Weights files\n(original, dxxwjz1, dxxwjz2,\nWeightsConverter)] as weights
  [Original Darknet\nC computation] as darknetc
}

package "Supporting components" {
  [ompi::OMPController] as omp
  [log (DefaultLogger /\ncpplog4c)] as log
  [helpers::StringTools] as helpers
  [zlibi::ZlibController] as zlib
}

main --> factory
factory --> parser
factory --> ctx
factory --> iapp : creates by ctx
factory --> signals : creates and installs
signals --> ctx : raises the OS signals flags
parser --> ctx : fills
iapp --> conv : converts ctx
conv --> lctx : fills
iapp --> facade : creates the work
iapp --> gui : the GUI app shows
gui --> trainer : trains a YOLO4 folder
facade --> ilib : creates
facade --> trainer : creates
trainer --> ilib : runs in it's thread
ilib --> omp : adjust threads
ilib --> dctrl : trains / detects
ilib --> weights : converts
dctrl --> dctx
dctx --> lctx : watches the stop request,\nreports the iterations
dctrl --> adaptor : selects worker
adaptor --> dctx : reads/writes
adaptor --> netinfra : loads the networks
netinfra --> dctx : hands the networks to
adaptor --> darknetc : trains / detects
darknetc --> netinfra : parses, resizes,\nreads and writes weights
netinfra --> weights : reads and writes
weights ..> zlib : compresses
factory ..> log
ilib ..> log
dctrl ..> log
parser ..> helpers
@enduml
