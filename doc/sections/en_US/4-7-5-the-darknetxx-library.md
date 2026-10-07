# The darknetxx library

The whole functionality of the project - the darknet training and detection,
the network weights files conversion and the background training of the
YOLO4 folders - is the shared `darknetxx-0.11` library of the
[`src/lib`](/src/lib) directory, laid out the way the
[cpp-app-template](https://github.com/yuriysydor1991/cpp-app-template) `applib`
branch lays out it's library. The executables of the project (`darknetxx`,
`darknetxxConverter` and `darknetxx-gui`) use the library through it's facade
alone, the very same way any other program does (see the
[Using the darknetxx library](/doc/sections/en_US/5-37-using-the-darknetxx-library.md)
section).

The standalone source of the diagram below is
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

## The facade

The [`src/lib/facade/public`](/src/lib/facade/public) headers are the whole
installable interface of the library, in the `darknetxx011` namespace (the
project name and it's major and minor version):

- **`LibraryFacade`** - the entry point: it creates the library context, the
  work the context asks for, the background trainer, lists the network weights
  files formats the conversion accepts, draws the PNG image of the training
  chart the record of a dxxwjz2 file keeps (`training_chart`) and adopts the
  logger of the library user.
- **`LibraryContext`** - the parameters of a library work, named the way the
  `darknetxx` command line parameters are, the training listener and the stop
  request: the own one (`request_stop()`) and the flag of the library user
  (`stop_with()`), the one an OS signal handler raises, for example.
- **`ILib`** - a work of the library: `libcall(ctx)` performs the training, the
  detection or the conversion the context names (the conversion first, then
  the `detect_cfg`, the `train_cfg` and the dxxwjz2 `latest_weights` alone,
  which is detected with or trained, see the
  [Network weights files](/doc/sections/en_US/4-7-4-network-weights-files.md)
  section). The training saves the weights files of the `save_format`.
- **`ITrainer`** and **`Yolo4Folder`** - the training of a YOLO4 folder (see the
  [The darknetxx-gui binary](/doc/sections/en_US/5-36-the-darknetxx-gui-binary.md)
  section) in a thread of it's own, resumed from the newest weights of the
  folder.
- **`ITrainingListener`** and **`TrainingProgress`** - the iterations and the
  end of a training, reported from the thread the training runs in.
- **`ILogger`** - the logger of the library user, which the
  `LibraryFacade::init_logger` hands over to the library, so the whole program
  logs into one place. It is the logging contract of every cpp-app-template
  derived library, which is why it carries neither the library namespace nor
  the library include guard.
- **`LibraryAPI.h`** - the `DARKNETXX_LIB_API` macro of the exported classes:
  the library is built with the hidden symbols visibility, so nothing else of
  it leaves the shared object.

## The implementation

The rest of the [`src/lib`](/src/lib) directory is private to the library:

| Directory | Holds |
| --- | --- |
| [`libmain`](/src/lib/libmain) | the `darknetxx_impl::LibFactory` of the works the facade gives out, the `LibMain` (the training and the detection by the `DarknetController`, after the OpenMP threads adjust) and the `WeightsConversionLib` (the network weights files conversion) |
| [`training`](/src/lib/training) | the `BackgroundTrainer` and the `Yolo4Folder` |
| [`darknet-adaptor`](/src/lib/darknet-adaptor) | the bridge to the original Darknet code (see the [Darknet adaptor components](/doc/sections/en_US/4-7-2-darknet-adaptor-components.md) section) |
| [`OMP`](/src/lib/OMP), [`zlib`](/src/lib/zlib), [`helpers`](/src/lib/helpers) | the supporting components (see the [Supporting components](/doc/sections/en_US/4-7-3-supporting-components.md) section) |

The library carries a copy of the [`src/log`](/src/log) logging subsystem of
it's own, as every executable of the project does: the
`default_logger::DefaultLogger` proxy of the logging macros forwards to the
`logger::ILogger` it holds, the own `default_logger::RealDefaultLogger` or the
adopted one. The executables hand their logger over to the library right after
their logging initialization, inside the `app::ApplicationFactory::run`
method.

The component tests of the library internals link the
`DarknetXXLibraryObjects` target of the objects the library is made of, while
the shared library is linked by the executables and the users of it.
