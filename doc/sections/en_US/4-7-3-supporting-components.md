# Supporting components

The supporting components are the smaller, cross cutting pieces used by the rest
of the application: the OpenMP controller, the logging subsystem and the helper
utilities.

The standalone source of the diagram below is
[`doc/diagrams/supporting-classes.puml`](/doc/diagrams/supporting-classes.puml).

@startuml
hide empty members

package "ompi" {
  class OMPController {
    +{static} create() : OMPControllerPtr
    +adjust(appCtx) : bool
    +adjust_threads(maxThreads) : bool
  }
}

package "log" {
  class DefaultLogger <<namespace>> {
    +{static} init(filepath, lvl, print)
    +{static} log(lvl, file, line, msg)
  }
  note bottom of DefaultLogger
    LOGE / LOGW / LOGI / LOGD / LOGT
    macros expand into DefaultLogger::log
  end note
}

package "helpers" {
  class StringTools {
    +string utilities
  }
}

OMPController --> "app::ApplicationContext" : reads threads
OMPController ..> DefaultLogger : logs
@enduml

## Purpose of each component

- **`ompi::OMPController`** ([`OMPController.h`](/src/OMP/OMPController.h)) - configures the OpenMP runtime. It reads the desired number of worker threads from the `app::ApplicationContext` (the `--threads` flag) and adjusts the OpenMP maximum threads accordingly. It is created through its static `create()` factory method.
- **Logging subsystem** ([`src/log`](/src/log)) - provides the `LOGE`, `LOGW`, `LOGI`, `LOGD` and `LOGT` macros defined in [`log.h`](/src/log/log.h). The macros forward the message together with the source file and line to the logger implementation. The default `default_logger::DefaultLogger` ([`DefaultLogger.h`](/src/log/default-logger/DefaultLogger.h)) writes the messages to the configured log file and/or to the standard streams; an alternative `cpplog4c` backend is also available.
- **`helpers::StringTools`** ([`StringTools.h`](/src/helpers/StringTools.h)) - small string manipulation utilities shared across the project.

The logging subsystem is initialized early by the `app::ApplicationFactory`,
which uses `CommandLineParser::get_custom_logfile` to honor the custom log file
path before the rest of the command line is parsed.
