# Project components and architecture

This section describes the available components of the `darknetxx` project, their
class structure and their purpose. The structure diagrams below are written in
the [PlantUML](https://plantuml.com/) language and their sources are kept under
the [`doc/diagrams`](/doc/diagrams) directory so they can be rendered both by the
Doxygen documentation build and by any standalone PlantUML tool.

The application is organized into a few cooperating layers:

1. [Application runtime components](/doc/sections/en_US/4-7-1-application-runtime-components.md) - the infrastructure that parses the command line, builds the run context and selects the application to execute (the `app` namespace).
1. [Darknet adaptor components](/doc/sections/en_US/4-7-2-darknet-adaptor-components.md) - the bridge that drives the original Darknet code to train or to detect (the `darknet_adaptor` namespace).
1. [Supporting components](/doc/sections/en_US/4-7-3-supporting-components.md) - the OpenMP controller, the logging subsystem, the zlib compression controller and the helper utilities.
1. [Network weights files](/doc/sections/en_US/4-7-4-network-weights-files.md) - the original darknet weights files and the dxxwjz1 ones (the gzip compressed JSON documents of the weights), their readers, writers and validators and the converter between them (the `darknet_adaptor::loaders` namespace).

The OS signals the whole application reacts on are covered by the `src/app/signals-handlers` subcomponent of the `app` layer, described by the [Handling the OS signals](/doc/sections/en_US/4-8-handling-the-OS-signals.md) section.

## Components overview

The `main()` function only forwards the command line arguments to the
`app::ApplicationFactory`. The factory builds an `app::ApplicationContext`, lets
the `app::CommandLineParser` fill it and then creates the appropriate
`app::IApplication` descendant. The default `app::Application` drives the
`darknet_adaptor::DarknetController` which selects and runs the concrete Darknet
worker (a detector, a trainer, etc.), while the conversion command line flags
make the `app::converter::ApplicationConverterComponent`, which converts the
network weights files by the `darknet_adaptor::loaders::WeightsConverter`. The
OpenMP controller and the logging subsystem support the whole flow. The second
binary of the project, the `darknetxxConverter`, is built of the same
components without the `app::Application` (see
[The darknetxxConverter binary](/doc/sections/en_US/5-35-the-darknetxxConverter-binary.md)).

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
  [IApplication\n(+ Application,\nHelpPrinter,\nVersionPrinter,\nConverterComponent)] as iapp
  [ISignalsHandler\n(+ SignalsHandler,\nStop/Pause/Continue/\nReload/User/Ignored)] as signals
}

package "darknet-adaptor (Darknet bridge)" {
  [DarknetController] as dctrl
  [DarknetContext] as dctx
  [IDarknetAdaptor\n(+ ADetector,\nADetectorTrainer)] as adaptor
  [Network infrastructure\n(NetworksLoaders, Layers,\nALayer/ANetwork adaptors,\nNetworksCApi)] as netinfra
  [Weights files\n(original, dxxwjz1,\nWeightsConverter)] as weights
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
iapp --> dctrl : default app drives
iapp --> omp : adjust threads
dctrl --> dctx
dctx --> ctx : watches the stop flag
dctrl --> adaptor : selects worker
adaptor --> dctx : reads/writes
adaptor --> netinfra : loads the networks
netinfra --> dctx : hands the networks to
adaptor --> darknetc : trains / detects
darknetc --> netinfra : parses, resizes,\nreads and writes weights
netinfra --> weights : reads and writes
iapp --> weights : converts (the\nconversion flags)
weights ..> zlib : compresses
factory ..> log
iapp ..> log
dctrl ..> log
parser ..> helpers
@enduml
