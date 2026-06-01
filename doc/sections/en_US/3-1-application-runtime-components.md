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

## Purpose of each class

- **`IApplication`** ([`IApplication.h`](/src/app/IApplication.h)) - the abstract interface implemented by every runnable application. It declares the single `run(ctx)` method and the common `INVALID` error status. New high level application behaviors are added by implementing this interface instead of branching inside an existing class.
- **`Application`** ([`Application.h`](/src/app/Application.h)) - the default application implementation. It adjusts the OpenMP threads, installs the `SIGINT` handler used to stop the work gracefully and drives the Darknet adaptor to perform the requested train or detect action.
- **`ApplicationHelpPrinter`** ([`ApplicationHelpPrinter.h`](/src/app/ApplicationHelpPrinter.h)) - prints the help message that represents all the available command line parameters. See the [Run the executable](/doc/sections/en_US/5-run-the-executable.md#available-command-line-parameters) section for the full list.
- **`ApplicationVersionPrinter`** ([`ApplicationVersionPrinter.h`](/src/app/ApplicationVersionPrinter.h)) - prints the application version, build git commit and configure date taken from the generated `project-global-decls.h` file.
- **`ApplicationFactory`** ([`ApplicationFactory.h`](/src/app/ApplicationFactory.h)) - the composition root. Its static `execute(argc, argv)` method (called from `main`) creates the context, runs the parser and instantiates the appropriate `IApplication` descendant in accordance with the parsed flags (help, version or the default application).
- **`CommandLineParser`** ([`CommandLineParser.h`](/src/app/CommandLineParser.h)) - parses the `argv` array into the `ApplicationContext` fields and validates that the parameters which require a value are given one. See [Introducing custom command line parameters](/doc/sections/en_US/2-project-structure.md#introducing-custom-command-line-parameters).
- **`CMDParamNames`** ([`CMDParamNames.h`](/src/app/CMDParamNames.h)) - the single place that holds the command line flag string constants (for example `--help`, `--train-cfg`). Both the parser and the help printer reference these constants so the flag names cannot drift.
- **`ApplicationContext`** ([`ApplicationContext.h`](/src/app/ApplicationContext.h)) - the data object that carries the parsed command line parameters, configuration values, the error list and the cooperative `stop` flag through the whole application run.
