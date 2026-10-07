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
  #perform(ctx) : int
  #create_and_convert_libctx(ctx) : LibraryContextPtr
  #create_ctx_converter() : CtxConverterPtr
  #{static} stopped_status(ctx) : int
  #{static} init_computation_traps()
}

class GuiApplication {
  #perform(ctx) : int
  #create_gui_initer() : IGUIIniter
}

class "converters::ApplicationContext2LibraryContext" as ApplicationContext2LibraryContext {
  +convert(appctx, libctx) : bool
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
  +create_default_signals_handler() : ISignalsHandler
  +create_application(ctx) : IApplication
  +create_default_application() : IApplication
  +create_library_application() : IApplication
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

interface ISignalsHandler {
  +install(ctx) : bool
  +uninstall()
}

class SignalsHandler {
  +install(ctx) : bool
  +uninstall()
  #create_handlers() : vector<ISignalsHandler>
}

note right of SignalsHandler
  The covered OS signals categories are
  described by the OS signals handlers
  section of the very same documentation
end note

struct ApplicationContext {
  +argc : int&
  +argv : char**&
  +print_help_and_exit : bool
  +print_version_and_exit : bool
  +train_cfg / detect_cfg / weights : string
  +thresh / iou_thresh / hier_thresh : float
  +threads : unsigned
  +stop / pause / reload : atomic_bool
  +first_user_request : atomic_bool
  +second_user_request : atomic_bool
  +convert_from / convert_src / convert_to : string
  +convert_dst / convert_cfg : string
  +push_error(desc)
  +conversion_requested() : bool
}

IApplication <|.. Application
IApplication <|.. ApplicationHelpPrinter
IApplication <|.. ApplicationVersionPrinter
Application <|-- GuiApplication
ISignalsHandler <|.. SignalsHandler

ApplicationFactory ..> IApplication : creates
ApplicationFactory ..> CommandLineParser : creates
ApplicationFactory ..> ApplicationContext : creates
ApplicationFactory ..> ISignalsHandler : creates and installs
CommandLineParser ..> CMDParamNames : uses names
CommandLineParser --> ApplicationContext : fills
Application --> ApplicationContext : holds actx
SignalsHandler --> ApplicationContext : raises the signals flags
Application ..> ApplicationContext2LibraryContext : converts actx by
ApplicationContext2LibraryContext ..> "darknetxx011::LibraryContext" : fills
Application ..> "darknetxx011::LibraryFacade" : performs the work it creates
GuiApplication ..> "gui::IGUIIniter" : shows the window of
ApplicationFactory ..> "darknetxx011::LibraryFacade" : hands the logger over
@enduml

## Призначення кожного класу

- **`IApplication`** ([`IApplication.h`](/src/app/IApplication.h)) - абстрактний інтерфейс, реалізований кожною запускною програмою. Він оголошує єдиний метод `run(ctx)` і спільний статус помилки `INVALID`. Нові високорівневі поведінки програми додаються реалізацією цього інтерфейсу, а не розгалуженням всередині наявного класу.
- **`Application`** ([`Application.h`](/src/app/Application.h)) - типова реалізація програми. Вона перехоплює хибні обчислення збірки `Debug`, перетворює контекст на `darknetxx011::LibraryContext` бібліотеки darknetxx і виконує роботу, яку з нього створює `darknetxx011::LibraryFacade`: навчання, розпізнавання чи конвертування файлів ваг нейромереж (див. секцію [Бібліотека darknetxx](/doc/sections/uk_UA/4-7-5-the-darknetxx-library.md)). Виконання, яке перервав сигнал зупинки, повертає статус `128 + номер сигналу`. Власного обробника сигналів операційної системи вона не встановлює, оскільки їх охоплює підкомпонент `src/app/signals-handlers` для кожної програми проекту.
- **`GuiApplication`** ([`GuiApplication.h`](/src/app/GuiApplication.h)) - типова програма бінарного файлу `darknetxx-gui`, яка замість роботи командного рядка показує вікно навчання компонента `src/gui` (див. секцію [Бінарний файл darknetxx-gui](/doc/sections/uk_UA/5-36-the-darknetxx-gui-binary.md)).
- **`converters::ApplicationContext2LibraryContext`** ([`ApplicationContext2LibraryContext.h`](/src/converters/ApplicationContext2LibraryContext.h)) - копіює параметри darknet і конвертування `ApplicationContext` до `darknetxx011::LibraryContext` і пов'язує прапорець `stop` контексту, який підіймають обробники сигналів, із запитом зупинки бібліотеки.
- **`ApplicationHelpPrinter`** ([`ApplicationHelpPrinter.h`](/src/app/ApplicationHelpPrinter.h)) - друкує довідкове повідомлення, яке представляє усі доступні параметри командного рядка. Дивись секцію [Доступні параметри командного рядка](/doc/sections/uk_UA/6-2-3-available-command-line-parameters.md) для повного переліку.
- **`ApplicationVersionPrinter`** ([`ApplicationVersionPrinter.h`](/src/app/ApplicationVersionPrinter.h)) - друкує версію програми, git коміт побудови і дату конфігурації, взяті зі згенерованого файлу `project-global-decls.h`.
- **`ApplicationFactory`** ([`ApplicationFactory.h`](/src/app/ApplicationFactory.h)) - корінь композиції. Його статичний метод `execute(argc, argv)` (викликаний з `main`) створює контекст, запускає розпізнавач і створює відповідного нащадка `IApplication` згідно з розпізнаними прапорцями (довідка, версія, конвертування файлів ваг нейромереж чи типова програма) і передає журнал програми бібліотеці darknetxx. Скомпільований з визначенням `DARKNETXX_CONVERTER_ONLY` (бінарний файл `darknetxxConverter`), він не має типової програми: на командний рядок, який не запитує конвертування, друкується довідкове повідомлення зі статусом помилки. Він також створює типову реалізацію інтерфейсу `ISignalsHandler` і передає контекст до її методу `install` безпосередньо перед запуском програми, тож оброблювані сигнали звільняються одразу після завершення методу `run`.
- **`CommandLineParser`** ([`CommandLineParser.h`](/src/app/CommandLineParser.h)) - розбирає масив `argv` у поля `ApplicationContext` і перевіряє, що параметри, які потребують значення, його отримали, а прапорці конвертування вказано разом. Дивись [Доступні параметри командного рядка](/doc/sections/uk_UA/6-2-3-available-command-line-parameters.md).
- **`ApplicationBinaryName.h`** ([`ApplicationBinaryName.h`](/src/app/ApplicationBinaryName.h)) - назва запущеного бінарного файлу (`darknetxx`, `darknetxxConverter` або `darknetxx-gui`), яку друкують друкувальники довідки і версії.
- **`CMDParamNames`** ([`CMDParamNames.h`](/src/app/CMDParamNames.h)) - єдине місце, що зберігає рядкові константи прапорців командного рядка (наприклад `--help`, `--train-cfg`). І розпізнавач, і друкувальник довідки використовують ці константи, тому назви прапорців не можуть розійтись.
- **`ISignalsHandler`** ([`ISignalsHandler.h`](/src/app/signals-handlers/ISignalsHandler.h)) - інтерфейс підкомпонента `src/app/signals-handlers`, реалізований як фасадом `SignalsHandler`, так і кожним окремим обробником категорії сигналів, тож решта програми залежить лише від інтерфейсу.
- **`SignalsHandler`** ([`SignalsHandler.h`](/src/app/signals-handlers/SignalsHandler.h)) - фасад, який охоплює кожну категорію сигналів операційної системи одним викликом `install`. Оброблювані категорії, прапорці, які вони підіймають, і спосіб додавання нової описано у секції [Обробка сигналів операційної системи](/doc/sections/uk_UA/4-8-handling-the-OS-signals.md).
- **`ApplicationContext`** ([`ApplicationContext.h`](/src/app/ApplicationContext.h)) - об'єкт даних, що переносить розпізнані параметри командного рядка, значення конфігурації, перелік помилок і кооперативні атомарні прапорці - `stop`, `pause`, `reload` та два прапорці запитів користувача, які підіймають обробники сигналів - через увесь процес виконання програми.
