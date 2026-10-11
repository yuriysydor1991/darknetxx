# Supporting components

The supporting components are the smaller, cross cutting pieces used by the rest
of the application: the OpenMP controller, the logging subsystem, the zlib
compression controller and the helper utilities.

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

package "zlibi" {
  class ZlibController {
    +{static} create() : ZlibControllerPtr
    +compress(buffer) / uncompress(buffer)
    +compress_to_file(path, buffer) : bool
    +uncompress_from_file(path) : buffer
    +create_file_writer(path, level) : GzipOStream
    +create_file_reader(path) : GzipIStream
    +{static} is_gzip_file(path) : bool
  }

  class "GzipOStream / GzipIStream" as GzipStreams {
    +open(path) : bool
    +close() : bool
    +failed() : bool
  }

  class GzipStreamBuf
}

OMPController --> "app::ApplicationContext" : reads threads
OMPController ..> DefaultLogger : logs
ZlibController ..> GzipStreams : opens
GzipStreams *-- GzipStreamBuf
@enduml

## Purpose of each component

- **`ompi::OMPController`** ([`OMPController.h`](/src/OMP/OMPController.h)) - configures the OpenMP runtime. It reads the desired number of worker threads from the `app::ApplicationContext` (the `--threads` flag) and adjusts the OpenMP maximum threads accordingly. It is created through its static `create()` factory method.
- **Logging subsystem** ([`src/log`](/src/log)) - provides the `LOGE`, `LOGW`, `LOGI`, `LOGD` and `LOGT` macros defined in [`log.h`](/src/log/log.h). The macros forward the message together with the source file and line to the logger implementation. The default `default_logger::DefaultLogger` ([`DefaultLogger.h`](/src/log/default-logger/DefaultLogger.h)) writes the messages to the configured log file and/or to the standard streams; an alternative `cpplog4c` backend is also available.
- **`helpers::StringTools`** ([`StringTools.h`](/src/helpers/StringTools.h)) - small string manipulation utilities shared across the project.
- **`zlibi::ZlibController`** ([`ZlibController.h`](/src/zlib/ZlibController.h)) - the zlib compression controller of the cpp-app-template `appZlib` branch, adapted for the dxxwjz1 network weights files (see the [Network weights files](/doc/sections/en_US/4-7-4-network-weights-files.md) section): the in memory compression (the zlib wrapper) and the gzip files of the buffers of any size, the zlib 32 bits counters notwithstanding, and the gzip files written and read through the standard streams a chunk at a time (the `GzipOStream` and the `GzipIStream` of [`GzipFileStreams.h`](/src/zlib/GzipFileStreams.h)), which keeps neither the whole file nor the whole data in the memory. The stream buffer runs the zlib deflate and inflate of the gzip wrapper itself: it verifies the whole gzip stream (the CRC and the size of the trailer included) and fails on the truncated or the corrupted file, which the zlib `gzread()` takes for a complete one when the data ends at the boundary of its read.

The logging subsystem is initialized early by the `app::ApplicationFactory`,
which uses `CommandLineParser::get_custom_logfile` to honor the custom log file
path before the rest of the command line is parsed.
