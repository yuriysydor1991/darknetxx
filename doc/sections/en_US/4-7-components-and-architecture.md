# Project components and architecture

This section describes the available components of the `darknetxx` project, their
class structure and their purpose. The structure diagrams below are written in
the [PlantUML](https://plantuml.com/) language and their sources are kept under
the [`doc/diagrams`](/doc/diagrams) directory so they can be rendered both by the
Doxygen documentation build and by any standalone PlantUML tool.

The application is organized into a few cooperating layers:

1. [Application runtime components](/doc/sections/en_US/4-7-1-application-runtime-components.md) - the infrastructure that parses the command line, builds the run context and selects the application to execute, which uses the darknetxx library (the `app` namespace).
1. [Darknet adaptor components](/doc/sections/en_US/4-7-2-darknet-adaptor-components.md) - the bridge that drives the original Darknet code to train or to detect (the `darknet_adaptor` namespace).
1. [Supporting components](/doc/sections/en_US/4-7-3-supporting-components.md) - the OpenMP controller, the logging subsystem, the zlib compression controller and the helper utilities.
1. [Network weights files](/doc/sections/en_US/4-7-4-network-weights-files.md) - the original darknet weights files, the dxxwjz1 ones (the gzip compressed JSON documents of the weights) and the dxxwjz2 ones (the network description and the training record beside the weights), their readers, writers and validators and the converter between them (the `darknet_adaptor::loaders` namespace).
1. [The darknetxx library](/doc/sections/en_US/4-7-5-the-darknetxx-library.md) - the shared library the three layers above are built into, it's installable facade (the `darknetxx011` namespace) and it's implementation (the `darknetxx_impl` namespace).

The OS signals the whole application reacts on are covered by the `src/app/signals-handlers` subcomponent of the `app` layer, described by the [Handling the OS signals](/doc/sections/en_US/4-8-handling-the-OS-signals.md) section.

## Components overview

The `main()` function only forwards the command line arguments to the
`app::ApplicationFactory`. The factory builds an `app::ApplicationContext`, lets
the `app::CommandLineParser` fill it and then creates the appropriate
`app::IApplication` descendant. The `app::Application` converts the context into
the `darknetxx011::LibraryContext` of the darknetxx library by the
`converters::ApplicationContext2LibraryContext` and performs the work the
`darknetxx011::LibraryFacade` of the library creates of it (see
[The darknetxx library](/doc/sections/en_US/4-7-5-the-darknetxx-library.md)):
the training or the detection by the `darknet_adaptor::DarknetController`,
which selects and runs the concrete Darknet worker (a detector, a trainer,
etc.), or the network weights files conversion the conversion command line
flags ask for by the `darknet_adaptor::loaders::WeightsConverter`. The OpenMP
controller and the logging subsystem support the whole flow. The second binary
of the project, the `darknetxxConverter`, is built of the same components and
performs the conversions alone (see
[The darknetxxConverter binary](/doc/sections/en_US/5-35-the-darknetxxConverter-binary.md)),
while the third one, the `darknetxx-gui`, adds the `src/gui` component, which
trains in it's GTK 4 window by the background trainer of the library (see
[The darknetxx-gui binary](/doc/sections/en_US/5-36-the-darknetxx-gui-binary.md)).

The standalone source of the diagram below is
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
