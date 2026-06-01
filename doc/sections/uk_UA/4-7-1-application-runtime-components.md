# Компоненти виконання програми

Компоненти виконання програми розміщені у просторі імен `app` (під
[`src/app`](/src/app)) і утворюють повторно використовувану інфраструктуру,
успадковану з проекту
[шаблону C++ програми](https://github.com/yuriysydor1991/cpp-app-template). Вони
відповідальні за розбір параметрів командного рядка, побудову контексту виконання
і вибір поведінки програми для запуску.

Окремий вихідний файл діаграми нижче -
[`doc/diagrams/application-runtime-classes.puml`](/doc/diagrams/application-runtime-classes.puml).

@startuml
hide empty members

interface IApplication {
  +run(ctx) : int
  +{static} INVALID : int
}

class Application {
  +run(ctx) : int
  #deal_omp(ctx) : bool
  #stop_catch() : bool
  -{static} init_signals() : bool
  -{static} handle_sigint(sig)
}

class ApplicationHelpPrinter {
  +run(ctx) : int
}

class ApplicationVersionPrinter {
  +run(ctx) : int
}

class ApplicationFactory {
  +create_default_context(argc, argv) : ApplicationContext
  +create_default_arg_parser() : CommandLineParser
  +create_application(ctx) : IApplication
  +create_help_printer() : IApplication
  +create_version_printer() : IApplication
  +run(argc, argv) : int
  +{static} execute(argc, argv) : int
}

class CommandLineParser {
  +parse_args(ctx) : bool
  +{static} get_custom_logfile(argc, argv) : string
  #parse_arg(ctx, param, hasNext, next, idx) : bool
  #requires_data(param) : bool
}

class CMDParamNames {
  +{static} HELPW, VERSIONW, ... : string
}

struct ApplicationContext {
  +argc : int&
  +argv : char**&
  +print_help_and_exit : bool
  +print_version_and_exit : bool
  +train_cfg / detect_cfg / weights : string
  +thresh / iou_thresh / hier_thresh : float
  +threads : unsigned
  +stop : atomic_bool
  +push_error(desc)
}

IApplication <|.. Application
IApplication <|.. ApplicationHelpPrinter
IApplication <|.. ApplicationVersionPrinter

ApplicationFactory ..> IApplication : creates
ApplicationFactory ..> CommandLineParser : creates
ApplicationFactory ..> ApplicationContext : creates
CommandLineParser ..> CMDParamNames : uses names
CommandLineParser --> ApplicationContext : fills
Application --> ApplicationContext : holds actx
@enduml

## Призначення кожного класу

- **`IApplication`** ([`IApplication.h`](/src/app/IApplication.h)) - абстрактний інтерфейс, реалізований кожною запускною програмою. Він оголошує єдиний метод `run(ctx)` і спільний статус помилки `INVALID`. Нові високорівневі поведінки програми додаються реалізацією цього інтерфейсу, а не розгалуженням всередині наявного класу.
- **`Application`** ([`Application.h`](/src/app/Application.h)) - типова реалізація програми. Вона налаштовує потоки OpenMP, встановлює обробник `SIGINT` для коректної зупинки роботи і керує адаптером Darknet для виконання запитаної дії навчання чи розпізнавання.
- **`ApplicationHelpPrinter`** ([`ApplicationHelpPrinter.h`](/src/app/ApplicationHelpPrinter.h)) - друкує довідкове повідомлення, яке представляє усі доступні параметри командного рядка. Дивись секцію [Доступні параметри командного рядка](/doc/sections/uk_UA/6-2-3-available-command-line-parameters.md) для повного переліку.
- **`ApplicationVersionPrinter`** ([`ApplicationVersionPrinter.h`](/src/app/ApplicationVersionPrinter.h)) - друкує версію програми, git коміт побудови і дату конфігурації, взяті зі згенерованого файлу `project-global-decls.h`.
- **`ApplicationFactory`** ([`ApplicationFactory.h`](/src/app/ApplicationFactory.h)) - корінь композиції. Його статичний метод `execute(argc, argv)` (викликаний з `main`) створює контекст, запускає розпізнавач і створює відповідного нащадка `IApplication` згідно з розпізнаними прапорцями (довідка, версія чи типова програма).
- **`CommandLineParser`** ([`CommandLineParser.h`](/src/app/CommandLineParser.h)) - розбирає масив `argv` у поля `ApplicationContext` і перевіряє, що параметри, які потребують значення, його отримали. Дивись [Доступні параметри командного рядка](/doc/sections/uk_UA/6-2-3-available-command-line-parameters.md).
- **`CMDParamNames`** ([`CMDParamNames.h`](/src/app/CMDParamNames.h)) - єдине місце, що зберігає рядкові константи прапорців командного рядка (наприклад `--help`, `--train-cfg`). І розпізнавач, і друкувальник довідки використовують ці константи, тому назви прапорців не можуть розійтись.
- **`ApplicationContext`** ([`ApplicationContext.h`](/src/app/ApplicationContext.h)) - об'єкт даних, що переносить розпізнані параметри командного рядка, значення конфігурації, перелік помилок і кооперативний прапорець `stop` через увесь процес виконання програми.
