# Компоненти проекту і архітектура

Дана секція описує доступні компоненти проекту `darknetxx`, їхню структуру
класів і призначення. Діаграми структури нижче написані мовою
[PlantUML](https://plantuml.com/), а їхні вихідні файли зберігаються у директорії
[`doc/diagrams`](/doc/diagrams), тому вони можуть відображатись як під час
побудови документації Doxygen, так і будь-яким окремим інструментом PlantUML.

Програма організована у декілька взаємодіючих рівнів:

1. [Компоненти виконання програми](/doc/sections/uk_UA/4-7-1-application-runtime-components.md) - інфраструктура, яка розбирає параметри командного рядка, будує контекст виконання і обирає програму для запуску (простір імен `app`).
1. [Компоненти адаптера Darknet](/doc/sections/uk_UA/4-7-2-darknet-adaptor-components.md) - місток, який керує оригінальним кодом Darknet для навчання чи розпізнавання (простір імен `darknet_adaptor`).
1. [Допоміжні компоненти](/doc/sections/uk_UA/4-7-3-supporting-components.md) - контролер OpenMP, підсистема журналювання і допоміжні утиліти.

## Огляд компонентів

Функція `main()` лише передає параметри командного рядка до
`app::ApplicationFactory`. Фабрика будує `app::ApplicationContext`, дозволяє
`app::CommandLineParser` заповнити його, після чого створює відповідного нащадка
`app::IApplication`. Типова `app::Application` керує
`darknet_adaptor::DarknetController`, який обирає і запускає конкретного робітника
Darknet (розпізнавач, тренер тощо). Контролер OpenMP і підсистема журналювання
підтримують увесь цей процес.

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
  [IApplication\n(+ Application,\nHelpPrinter,\nVersionPrinter)] as iapp
}

package "darknet-adaptor (Darknet bridge)" {
  [DarknetController] as dctrl
  [DarknetContext] as dctx
  [IDarknetAdaptor\n(+ ADetector,\nADetectorTrainer,\nALayer/ANetwork)] as adaptor
}

package "Supporting components" {
  [ompi::OMPController] as omp
  [log (SimpleLogger /\ncpplog4c)] as log
  [helpers::StringTools] as helpers
}

main --> factory
factory --> parser
factory --> ctx
factory --> iapp : creates by ctx
parser --> ctx : fills
iapp --> dctrl : default app drives
iapp --> omp : adjust threads
dctrl --> dctx
dctrl --> adaptor : selects worker
adaptor --> dctx : reads/writes
factory ..> log
iapp ..> log
dctrl ..> log
parser ..> helpers
@enduml
