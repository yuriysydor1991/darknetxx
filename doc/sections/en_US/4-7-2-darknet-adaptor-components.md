# Darknet adaptor components

The Darknet adaptor components live in the `darknet_adaptor` namespace (under
[`src/darknet-adaptor`](/src/darknet-adaptor), the network loaders of the
`darknet_adaptor::loaders` one under [`src/NetworksLoaders`](/src/NetworksLoaders)).
They form the bridge between the
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
  +set_train_networks(nets)
  +set_map_network(net)
  +set_detect_network(net)
  +release_networks()
  -start_stop_watcher()
  -stop_stop_watcher()
  -watch_the_app_stop_flag()
}

interface IDarknetAdaptor {
  +perform(dctx) : bool
}

class ADetector {
  +perform(dctx) : bool
  #create_loader() : INetworkLoaderPtr
}
class ADetectorTrainer {
  +perform(dctx) : bool
  #create_loader() : INetworkLoaderPtr
}

interface INetworkLoader {
  +load(dctx) : bool
}

class OrigDefaultLoader {
  +load(dctx) : bool
  +parse_network_cfg(cfg) : ANetworkAdaptorPtr
  +parse_network_cfg_custom(cfg, batch, time_steps) : ANetworkAdaptorPtr
  +load_network(cfg, weights, clear) : ANetworkAdaptorPtr
  +load_network_custom(cfg, weights, clear, batch) : ANetworkAdaptorPtr
}

class CfgSection {
  +{static} read_cfg(filename) : CfgSections
  +find_int/find_float/find_str(key, def)
  +unused()
}

interface IWeightsReader {
  +load(net, filename, cutoff, rest) : bool
  +read(filename, net, document) : bool
}

interface IWeightsWriter {
  +save(net, filename, cutoff, save_ema, rest) : bool
  +write(document, filename) : bool
}

interface IWeightsFileFormat {
  +create_reader() : IWeightsReaderPtr
  +create_writer() : IWeightsWriterPtr
}

class OrigWeightsReader
class OrigWeightsWriter

class TrainingJournal {
  +record(iteration, loss, avg_loss)
  +record_map(iteration, map)
  +save(net, stem, ema) : bool
}

class ANetworkAdaptor {
  +get() : networkptr
  +layers(adaptors)
  +allocate_layers(count)
  +bind_layer(index, adaptor)
  +resize(w, h)
  +set_batch(b)
  +fuse_conv_batchnorm()
  +calculate_binary_weights()
  +{static} copy_weights_net(train, map)
}

class ALayerAdaptor {
  +get() : layerptr
  +bind(slot)
  +relink()
  +borrow_output(buffer), borrow_weights(buffer), ...
}

class "layers::ConvolutionalLayer, layers::YoloLayer, ..." as Layers {
  +{static} make(...) : ALayerAdaptorPtr
  +{static} resize(adaptor, w, h)
}

class NetworksRegistry {
  +{static} add(net, owned)
  +{static} find(view) : ANetworkAdaptorPtr
  +{static} remove(view) : bool
}

class "NetworksCApi (extern C)" as NetworksCApi {
  +parse_network_cfg(), load_network(), free_network()
  +load_weights(), save_weights()
  +resize_network(), set_batch_network()
  +fuse_conv_batchnorm(), calculate_binary_weights()
}

class StructAdaptorHelper {
  +data_or_null(buffer)
}

IDarknetAdaptor <|.. ADetector
IDarknetAdaptor <|.. ADetectorTrainer
IDarknetAdaptor <|.. ALayerAdaptor
IDarknetAdaptor <|.. ANetworkAdaptor
INetworkLoader <|.. OrigDefaultLoader
IWeightsReader <|.. OrigWeightsReader
IWeightsWriter <|.. OrigWeightsWriter
StructAdaptorHelper <|-- ALayerAdaptor
StructAdaptorHelper <|-- ANetworkAdaptor

DarknetController --> DarknetContext : creates
DarknetController ..> IDarknetAdaptor : selects worker
IDarknetAdaptor --> DarknetContext : reads/writes
ADetector ..> INetworkLoader : loads the network
ADetectorTrainer ..> INetworkLoader : loads the networks
OrigDefaultLoader ..> CfgSection : reads the cfg
OrigDefaultLoader ..> Layers : makes the layers
OrigDefaultLoader ..> IWeightsFileFormat : loads the weights of any format
OrigDefaultLoader ..> TrainingJournal : starts the journal of the training
IWeightsFileFormat ..> IWeightsReader : creates
IWeightsFileFormat ..> IWeightsWriter : creates
DarknetContext --> TrainingJournal : records the iterations, saves the weights by
OrigDefaultLoader --> ANetworkAdaptor : makes
OrigDefaultLoader ..> DarknetContext : hands the networks to
Layers --> ALayerAdaptor : makes and resizes
ANetworkAdaptor "1" *-- "*" ALayerAdaptor : binds to the layers array
DarknetContext --> NetworksRegistry : registers the networks
NetworksCApi ..> NetworksRegistry : finds the network of the view
NetworksCApi ..> OrigDefaultLoader : parses
NetworksCApi ..> IWeightsFileFormat : reads and writes the weights of
DarknetContext --> "app::ApplicationContext" : built from
@enduml

## Purpose of each class

- **`DarknetController`** ([`DarknetController.h`](/src/darknet-adaptor/DarknetController.h)) - the adaptor entry point. Given an `app::ApplicationContext` it builds a `DarknetContext`, selects the appropriate worker and calls its `perform` method. It is created through its static `create()` factory method.
- **`DarknetContext`** ([`DarknetContext.h`](/src/darknet-adaptor/DarknetContext.h)) - the adaptor data context. It translates the application context fields into the values expected by the original Darknet routines and owns the underlying `detector_context`. It exposes typed getters and setters for the individual Darknet parameters. Its `init` method also starts a stop watcher thread which polls the `app::ApplicationContext::stop` flag and mirrors it into the `detector_context::stop` field the original C loops check every iteration, so a Ctrl+C keys combination ends a long training run - see the [Handling the OS signals](/doc/sections/en_US/4-8-handling-the-OS-signals.md) section.
- **`IDarknetAdaptor`** ([`IDarknetAdaptor.h`](/src/darknet-adaptor/adaptors/IDarknetAdaptor.h)) - the abstract interface of a concrete Darknet worker. It declares the single `perform(dctx)` method so new workers can be added without touching the controller logic.
- **`ADetector`** / **`ADetectorTrainer`** - the concrete workers that load the networks through their network loader (the `create_loader()` one, the `OrigDefaultLoader` by default), run the detection and the training over the prepared `DarknetContext` and release the networks afterwards.
- **`INetworkLoader`** ([`INetworkLoader.h`](/src/NetworksLoaders/INetworkLoader.h)) - the interface of the network loaders: the `load(dctx)` method makes the networks the work of the context needs and hands them to the context.
- **`OrigDefaultLoader`** ([`OrigDefaultLoader.h`](/src/NetworksLoaders/OrigDefaultLoader/OrigDefaultLoader.h)) - the C++ port of the original network cfg parser (the `parse_network_cfg()`, the `load_network()` and the like of the original `parser.c`). It makes the training networks (one per GPU), the mAP calculation network, which computes with the layers of the first training one, and the single image detection network with the batch normalization fused into it's weights. The networks are made of the cfg files or of the dxxwjz2, the dxxwjz2p, the dxxwjz3 and the dxxwjz3p ones, the weights of every format are loaded by the readers of their formats, the image lists of the training of a dxxwjz2p or a dxxwjz3p project are the ones of it's annotations, which the `DarknetContext` publishes in the `TrainingDataRegistry` the darknet C code reads them of, the class names of the detection and of the training (the mAP calculation of it) go into the `DarknetContext` and the training starts it's `TrainingJournal`, which the training saves the weights files by (see the [Network weights files](/doc/sections/en_US/4-7-4-network-weights-files.md) section). The parse errors, which ended the process in the original, throw the `NetworkError` exception the `load` method logs.
- **`CfgSection`** ([`CfgSection.h`](/src/NetworksLoaders/OrigDefaultLoader/CfgSection.h)) - one `[section]` of the cfg file with it's `key=value` options, the port of the original `read_cfg()` and `option_find*()` functions.
- **`OrigWeightsReader`** ([`OrigWeightsReader.h`](/src/NetworksLoaders/original-weights/OrigWeightsReader.h)) / **`OrigWeightsWriter`** ([`OrigWeightsWriter.h`](/src/NetworksLoaders/original-weights/OrigWeightsWriter.h)) - the original weights files reader and writer, the ports of the original `load_weights_upto()`, `save_weights_upto()` and the per layer routines, byte to byte compatible with the original files. They implement the `IWeightsReader` and the `IWeightsWriter` interfaces the dxxwjz1 files reader and writer implement as well; the weights files formats, their validators and the converter between them are described by the [Network weights files](/doc/sections/en_US/4-7-4-network-weights-files.md) section.
- **`ANetworkAdaptor`** ([`ANetworkAdaptor.h`](/src/darknet-adaptor/adaptors/ANetworkAdaptor/ANetworkAdaptor.h)) - the owner of the memory of a network: the layers array, the workspace, the counters every copy of the network struct shares and the like. It implements the network routines which allocate the memory: the `resize` (the original `resize_network()`), the `set_batch`, the `fuse_conv_batchnorm`, the `calculate_binary_weights` and the `copy_weights_net` ones.
- **`ALayerAdaptor`** ([`ALayerAdaptor.h`](/src/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h)) - the owner of the memory of a layer: every buffer is a `std::vector` of the adaptor the layer struct field points at (the `NULL` one for the empty container). The buffers Darknet aliases (the previous layer output of the `[dropout]` layer, the weights of a layer sharing them with another one and the like) are borrowed with the `borrow_*` setters. The network binds the layer struct into it's slot of the layers array, so the Darknet code and the adaptor share the one struct.
- **The layer classes** ([`src/darknet-adaptor/adaptors/Layers`](/src/darknet-adaptor/adaptors/Layers)) - the `make` and the `resize` routines of every layer kind (`ConvolutionalLayer`, `YoloLayer` and the rest), the ports of the original `make_*_layer()` and `resize_*_layer()` functions.
- **`NetworksRegistry`** / **NetworksCApi** ([`src/darknet-adaptor/adaptors/NetworksCApi`](/src/darknet-adaptor/adaptors/NetworksCApi)) - the Darknet C API functions the original C code keeps calling (`parse_network_cfg()`, `load_weights()`, `save_weights()`, `resize_network()`, `set_batch_network()`, `free_network()` and the like), implemented on top of the adaptors. The C code passes the network struct by value, so the registry finds the network of a struct copy by it's layers array. The errors end the process the way the original functions did.
- **`StructAdaptorHelper`** ([`StructAdaptorHelper.h`](/src/darknet-adaptor/adaptors/StructAdaptorHelper.h)) - the helper of the adaptors, which gives the struct fields the pointers of the containers.

## Network memory

The original Darknet C code of the project only computes: it never allocates,
reallocates nor frees the memory of a network. The networks are made by the
loader and belong to the C++ adaptors, the `DarknetContext` hands their struct
views to the original computation context and takes them back after the work.
Every C copy of a network struct shares it's buffers, the scalar changes the C
code makes to it's copy (the batch of the dynamic mini batch and the like) are
taken over by the adaptor on the next C API call.

The ported resize routines fix the memory errors of the original ones: the
XNOR input buffers of the convolutional layer, the antialiasing blur sub-layer
of the convolutional and the maxpool layers, the `x` and the `x_norm` buffers
of the `[batchnorm]` layer and the column buffer of the `[local]` layer of the
padding bigger than it's kernel are sized for the resized input, the outputs
of the `[yolo]`, the `[Gaussian_yolo]` and the `[region]` layers passed to the
next layer are the resized ones.

The concrete worker selection keeps the original Darknet entry points isolated
behind the `IDarknetAdaptor` interface, which is what makes the gradual porting
of the original code possible.
