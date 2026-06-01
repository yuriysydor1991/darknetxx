# Darknet adaptor components

The Darknet adaptor components live in the `darknet_adaptor` namespace (under
[`src/darknet-adaptor`](/src/darknet-adaptor)). They form the bridge between the
clean C++ application infrastructure and the original Darknet C/C++ code, so the
rest of the application does not depend on the original structures directly.

The standalone source of the diagram below is
[`doc/diagrams/darknet-adaptor-classes.puml`](/doc/diagrams/darknet-adaptor-classes.puml).

@startuml
hide empty members

class DarknetController {
  +{static} create() : DarknetControllerPtr
  +perform(appCtx) : bool
  #create_context(appCtx) : DarknetContextPtr
  #create_appropriate_worker(dctx) : IDarknetAdaptorPtr
}

class DarknetContext {
  +DarknetContext(appCtx)
  +init() : bool
  +get_darknet_ctx() : detector_context
  +get/set_Dont_show(), get/set_Map(), ... : accessors
}

interface IDarknetAdaptor {
  +perform(dctx) : bool
}

class ADetector {
  +perform(dctx) : bool
}
class ADetectorTrainer {
  +perform(dctx) : bool
}
class ALayerAdaptor {
  +perform(dctx) : bool
}
class ANetworkAdaptor {
  +perform(dctx) : bool
}

class StructAdaptorHelper {
  +converts orig Darknet structs
}

IDarknetAdaptor <|.. ADetector
IDarknetAdaptor <|.. ADetectorTrainer
IDarknetAdaptor <|.. ALayerAdaptor
IDarknetAdaptor <|.. ANetworkAdaptor

DarknetController --> DarknetContext : creates
DarknetController ..> IDarknetAdaptor : selects worker
IDarknetAdaptor --> DarknetContext : reads/writes
DarknetContext ..> StructAdaptorHelper : struct mapping
DarknetContext --> "app::ApplicationContext" : built from
@enduml

## Purpose of each class

- **`DarknetController`** ([`DarknetController.h`](/src/darknet-adaptor/DarknetController.h)) - the adaptor entry point. Given an `app::ApplicationContext` it builds a `DarknetContext`, selects the appropriate worker and calls its `perform` method. It is created through its static `create()` factory method.
- **`DarknetContext`** ([`DarknetContext.h`](/src/darknet-adaptor/DarknetContext.h)) - the adaptor data context. It translates the application context fields into the values expected by the original Darknet routines and owns the underlying `detector_context`. It exposes typed getters and setters for the individual Darknet parameters.
- **`IDarknetAdaptor`** ([`IDarknetAdaptor.h`](/src/darknet-adaptor/adaptors/IDarknetAdaptor.h)) - the abstract interface of a concrete Darknet worker. It declares the single `perform(dctx)` method so new workers can be added without touching the controller logic.
- **`ADetector`** / **`ADetectorTrainer`** - the concrete workers that run the detection and the training over the prepared `DarknetContext`.
- **`ALayerAdaptor`** / **`ANetworkAdaptor`** - the adaptors that map the original Darknet layer and network structures into the C++ side of the project.
- **`StructAdaptorHelper`** ([`StructAdaptorHelper.h`](/src/darknet-adaptor/adaptors/StructAdaptorHelper.h)) - the helper that converts the original Darknet C structures to and from the project structures.

The concrete worker selection keeps the original Darknet entry points isolated
behind the `IDarknetAdaptor` interface, which is what makes the gradual porting
of the original code possible.
