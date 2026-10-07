# Application runtime components

The application runtime components live in the `app` namespace (under
[`src/app`](/src/app)) and form the reusable infrastructure inherited from the
[C++ application template](https://github.com/yuriysydor1991/cpp-app-template)
project. They are responsible for parsing the command line, building the run
context and selecting which application behavior to execute.

The standalone source of the diagram below is
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

## Purpose of each class

- **`IApplication`** ([`IApplication.h`](/src/app/IApplication.h)) - the abstract interface implemented by every runnable application. It declares the single `run(ctx)` method and the common `INVALID` error status. New high level application behaviors are added by implementing this interface instead of branching inside an existing class.
- **`Application`** ([`Application.h`](/src/app/Application.h)) - the default application implementation. It traps the invalid computations of a `Debug` build, converts the context into the `darknetxx011::LibraryContext` of the darknetxx library and performs the work the `darknetxx011::LibraryFacade` creates of it: the training, the detection or the network weights files conversion (see the [The darknetxx library](/doc/sections/en_US/4-7-5-the-darknetxx-library.md) section). A run a stop signal ended returns the `128 + signal number` status. It installs no OS signal handler of its own, since the `src/app/signals-handlers` subcomponent covers them for every application of the project.
- **`GuiApplication`** ([`GuiApplication.h`](/src/app/GuiApplication.h)) - the default application of the `darknetxx-gui` binary, which shows the training window of the `src/gui` component in place of the command line work (see the [The darknetxx-gui binary](/doc/sections/en_US/5-36-the-darknetxx-gui-binary.md) section).
- **`converters::ApplicationContext2LibraryContext`** ([`ApplicationContext2LibraryContext.h`](/src/converters/ApplicationContext2LibraryContext.h)) - copies the darknet and the conversion parameters of the `ApplicationContext` into the `darknetxx011::LibraryContext` and links the `stop` flag of the context the signals handlers raise to the stop request of the library.
- **`ApplicationHelpPrinter`** ([`ApplicationHelpPrinter.h`](/src/app/ApplicationHelpPrinter.h)) - prints the help message that represents all the available command line parameters. See the [Available command line parameters](/doc/sections/en_US/6-2-3-available-command-line-parameters.md) section for the full list.
- **`ApplicationVersionPrinter`** ([`ApplicationVersionPrinter.h`](/src/app/ApplicationVersionPrinter.h)) - prints the application version, build git commit and configure date taken from the generated `project-global-decls.h` file.
- **`ApplicationFactory`** ([`ApplicationFactory.h`](/src/app/ApplicationFactory.h)) - the composition root. Its static `execute(argc, argv)` method (called from `main`) creates the context, runs the parser and instantiates the appropriate `IApplication` descendant in accordance with the parsed flags (help, version, the network weights files conversion or the default application) and hands the logger of the application over to the darknetxx library. Compiled with the `DARKNETXX_CONVERTER_ONLY` definition (the `darknetxxConverter` binary) it has no default application: a command line which asks for no conversion gets the failed help message. It also creates the default `ISignalsHandler` implementation and hands the context over to its `install` method right before the application start, so the handled signals are released as soon as the `run` method returns.
- **`CommandLineParser`** ([`CommandLineParser.h`](/src/app/CommandLineParser.h)) - parses the `argv` array into the `ApplicationContext` fields and validates that the parameters which require a value are given one and that the conversion flags go together. See [Available command line parameters](/doc/sections/en_US/6-2-3-available-command-line-parameters.md).
- **`ApplicationBinaryName.h`** ([`ApplicationBinaryName.h`](/src/app/ApplicationBinaryName.h)) - the name of the binary being run (the `darknetxx`, the `darknetxxConverter` or the `darknetxx-gui` one), which the help and the version printers print.
- **`CMDParamNames`** ([`CMDParamNames.h`](/src/app/CMDParamNames.h)) - the single place that holds the command line flag string constants (for example `--help`, `--train-cfg`). Both the parser and the help printer reference these constants so the flag names cannot drift.
- **`ISignalsHandler`** ([`ISignalsHandler.h`](/src/app/signals-handlers/ISignalsHandler.h)) - the interface of the `src/app/signals-handlers` subcomponent, implemented both by the `SignalsHandler` facade and by every single signals category handler, so the rest of the application depends on the interface alone.
- **`SignalsHandler`** ([`SignalsHandler.h`](/src/app/signals-handlers/SignalsHandler.h)) - the facade which covers every OS signals category of the application with a single `install` call. The covered categories, the flags they raise and the way a new one is added are described by the [Handling the OS signals](/doc/sections/en_US/4-8-handling-the-OS-signals.md) section.
- **`ApplicationContext`** ([`ApplicationContext.h`](/src/app/ApplicationContext.h)) - the data object that carries the parsed command line parameters, configuration values, the error list and the cooperative atomic flags - the `stop`, the `pause`, the `reload` and the two user defined request ones the signals handlers raise - through the whole application run.
