# Network weights files

The `darknetxx` reads and writes the weights of the networks in eight formats:

- the **original darknet weights file** (`*.weights`), the binary file the original Darknet reads and writes;
- the **dxxwjz1 file** (`*.dxxwjz1`, the **d**arknet**xx** **w**eights **j**son **z**ipped file of the first version), the gzip compressed JSON document of the same weights, readable by any JSON tool and by a human;
- the **dxxwjz2 file** (`*.dxxwjz2`, the file of the second version), the dxxwjz1 document of everything the network is detected and trained with beside its weights: the text of the network cfg file, the names of the classes with their indexes, the loss of every training iteration in the order of the training and the time the training took;
- the **dxxwjz2p file** (`*.dxxwjz2p`, the dxxwjz2 **p**roject file), the dxxwjz2 document of the YOLO4 training project: the network, the weights and the training record beside the YOLO4 annotations of the images, everything the training needs but the images themselves (see the [The dxxwjz2p project file format](#the-dxxwjz2p-project-file-format) section);
- the **dxxwjz3 file** (`*.dxxwjz3`, the file of the third version), the dxxwjz2 document of nothing but the JSON data: the network cfg is the JSON structure of the options of its sections, the `net` object of the `[net]` ones, instead of the text of the cfg file, the layer sections keep the arrays of their layers (see the [The dxxwjz3 file format of the third version](#the-dxxwjz3-file-format-of-the-third-version) section);
- the **dxxwjz3p file** (`*.dxxwjz3p`, the dxxwjz3 **p**roject file), the dxxwjz2p project of the dxxwjz3 document, the whole training project of the JSON data alone (see the [The dxxwjz3p project file format](#the-dxxwjz3p-project-file-format) section);
- the **dxxwjz4 file** (`*.dxxwjz4`, the file of the fourth version), the dxxwjz3 document of the description text: the comments and the description of the network and of it's data of any length (see the [The dxxwjz4 file format of the fourth version](#the-dxxwjz4-file-format-of-the-fourth-version) section);
- the **dxxwjz4p file** (`*.dxxwjz4p`, the dxxwjz4 **p**roject file), the whole training project in one file, the images of the annotations included: the ZIP archive of the single JSON document of everything the training needs and of the images in it's `data/` folder, which any zip tool unpacks (see the [The dxxwjz4p project file format](#the-dxxwjz4p-project-file-format) section).

The **YOLO4 training folder**, the one the ImagesAnnotator exports, converts into the dxxwjz2p project and back: it's the `yolo4` format of the conversion alone (see the [The YOLO4 training folder](#the-yolo4-training-folder) section).

The [ImagesAnnotator](https://github.com/yuriysydor1991/ImagesAnnotator) generates the files of the most recent formats itself, the dxxwjz3p project of its annotations and the dxxwjz3 network to train of its YOLO4 training folder (the dxxwjz2p and the dxxwjz2 ones too), and reads them back (see the [The files of the ImagesAnnotator](#the-files-of-the-imagesannotator) section).

The training and the detection of the `darknetxx` read the weights files of every format (the `--weights` command line parameter, the format of a file is told by its name extension), the training saves its weights into the dxxwjz3 files by default (see the [Training with the formats](#training-with-the-formats) section below) and the dxxwjz2, the dxxwjz2p, the dxxwjz3, the dxxwjz3p, the dxxwjz4 and the dxxwjz4p files alone are the networks the detection runs with. The training of the dxxwjz4p project unpacks it into the temporary directory and saves the network it trains into the dxxwjz4 file beside the project (see the [Training the dxxwjz4p project](#training-the-dxxwjz4p-project) section).

The weights of the formats convert into each other with no loss: the original weights file converted to the dxxwjz1, the dxxwjz2 or the dxxwjz3 one and back is the same file byte to byte (see the [Converting the network weights files](/doc/sections/en_US/6-2-4-converting-the-network-weights-files.md) section and the [The darknetxxConverter binary](/doc/sections/en_US/5-35-the-darknetxxConverter-binary.md) one), while the network description and the training record are kept by the dxxwjz2, the dxxwjz3 and the dxxwjz4 files and their projects alone, the annotations of the images by the dxxwjz2p, the dxxwjz3p and the dxxwjz4p projects, the images themselves and the description text by the dxxwjz4p project (the description by the dxxwjz4 file too).

The formats - their documents, their readers, writers and validators and the converter between them - are the ones of the standalone [darknetxx-datadrivers](https://github.com/yuriysydor1991/darknetxx-datadrivers) library the project is built against, which documents them in full: the keys of the JSON documents, the text of the floats, the ZIP archives of the projects and the checks of every validator. The sections below tell what the darknetxx takes of the library, how it hands the darknet networks over to it and what every format keeps, in short.

## The darknetxx-datadrivers library

The network weights files and the training projects formats lived in the [`src/NetworksLoaders`](/src/NetworksLoaders) directory of this project and the zlib compression of them in it's `src/zlib` one. They were extracted into the darknetxx-datadrivers library, made of the `applib` branch of the [cpp-app-template](https://github.com/yuriysydor1991/cpp-app-template), which the project consumes installed (see the [Enabling the darknetxx-datadrivers library](/doc/sections/en_US/5-39-enabling-the-darknetxx-datadrivers-library.md) section), so the darknetxx, it's `darknetxxConverter` binary and any other tool share one and the same implementation of the formats:

| The former darknetxx source | Held | The library now |
| --- | --- | --- |
| `src/NetworksLoaders/IWeightsReader.h`, `IWeightsWriter.h` and `IWeightsFileFormat.h` | the interfaces every format implements | the `IWeightsReader`, the `IWeightsWriter` and the `IWeightsFileFormat` of the public headers, which load the files into and save them of the `INetworkWeights` network |
| `src/NetworksLoaders/weights-model` | the format neutral model of the weights and it's validation | the `WeightsSchema`, the `WeightsLayout`, the `WeightsDocument` (the `NetworkDescription`, the `TrainingRecord`, the `Yolo4Annotations`, the `Yolo4Image` and the `Yolo4Box` of it), the `NetworkFiles` and the `WeightsReport` of the public headers, the validator of the documents inside |
| `src/NetworksLoaders/original-weights` | the original darknet weights file | inside the library |
| `src/NetworksLoaders/dxxwjz/dxxwjz1` ... `dxxwjz4p` | the dxxwjz1, the dxxwjz2, the dxxwjz2p, the dxxwjz3, the dxxwjz3p, the dxxwjz4 and the dxxwjz4p formats and the ZIP archives of the dxxwjz4p projects | inside the library |
| `src/NetworksLoaders/yolo4-folder` | the YOLO4 training folder of the conversion | inside the library, the `yolo4::Yolo4FolderLayout` of the files of the folder among the public headers |
| `src/NetworksLoaders/weights-formats` | the formats of their names and file name extensions, the files networks are made of | the `WeightsFileFormats` and the `NetworkSource` of the public headers |
| `src/NetworksLoaders/weights-converter` | the converter of the files between the formats | the `WeightsConverter` and the `WeightsConversion` of the public headers (the `NetworkRewriter` of the directory stayed here, see below) |
| `src/NetworksLoaders/OrigDefaultLoader/CfgSection.h` | the sections of the cfg file | the `CfgSection` of the public headers, which throws the `CfgError` |
| `src/zlib` | the `zlibi::ZlibController` and the gzip file streams | inside the library |
| `src/darknetxxConverter-target.cmake` | the `darknetxxConverter` binary | the binary of the library (see [The darknetxxConverter binary](/doc/sections/en_US/5-35-the-darknetxxConverter-binary.md) section) |

The public headers of the `include/DarknetxxDataDrivers-0.12` directory, the `DarknetxxDataDrivers012` namespace, are the whole interface of the library. The classes of the formats - the readers, the writers, the validators, the parsers and the codecs of every one of them - are built with the hidden symbols visibility and stay inside the shared object, so the darknetxx reaches the formats by the `WeightsFileFormats` registry alone: of their names (the ones of the command line) and of the file name extensions, every other path being the original weights file. The library carries no darknet code: the types of the layers are it's `LayerType` (the darknet `LAYER_TYPE` of the very same names and values), the networks the files are loaded into and saved of are the views of the library user over it's own networks (the `INetworkWeights`).

## The darknet networks of the formats

The darknet networks of the formats stay in this project, in the `darknet_adaptor::loaders` namespace:

| Source | Holds |
| --- | --- |
| [`src/NetworksLoaders/DataDriversAliases.h`](/src/NetworksLoaders/DataDriversAliases.h) | the one include of the library and the `dxxdd` alias of it's namespace |
| [`src/NetworksLoaders/OrigDefaultLoader`](/src/NetworksLoaders/OrigDefaultLoader) | the `OrigDefaultLoader` of the darknet networks of the cfg sections of the library, which loads the weights files of every format into them (see the [Darknet adaptor components](/doc/sections/en_US/4-7-2-darknet-adaptor-components.md) section) |
| [`src/NetworksLoaders/network-weights`](/src/NetworksLoaders/network-weights) | the `NetworkWeights` view of the library over the darknet network and the `NetworkWeightsConverter` of the files by the darknet network of the cfg |
| [`src/NetworksLoaders/network-rewriter`](/src/NetworksLoaders/network-rewriter) | the `NetworkRewriter` of the edited network, validated by the darknet network of the edited cfg (see the [Saving the edited network](#saving-the-edited-network) section) |

The journal of the training, the `TrainingJournal` of the [`TrainingJournal`](/src/darknet-adaptor/adaptors/TrainingJournal) directory, keeps the network description and the training record the weights files of the training keep, the `TrainingChart` of the [`TrainingChart`](/src/darknet-adaptor/adaptors/TrainingChart) directory draws the chart of the record (see the [Training with the formats](#training-with-the-formats) section), the `TrainingDataRegistry` of the [`TrainingData`](/src/darknet-adaptor/adaptors/TrainingData) directory hands the image lists and the labels of the dxxwjz2p and the dxxwjz3p projects over to the darknet C training code (see the [Training the dxxwjz2p project](#training-the-dxxwjz2p-project) section), all of them over the documents of the library.

The standalone source of the diagram below is
[`doc/diagrams/network-weights-files-classes.puml`](/doc/diagrams/network-weights-files-classes.puml).

@startuml
hide empty members

package "darknetxx-datadrivers (the DarknetxxDataDrivers012 namespace, the dxxdd alias)" {
  interface INetworkWeights {
    +layers() : int
    +type(layer) : LayerType
    +layout(options) : WeightsLayoutEntries
    +classes() : int
    +names() : string
    +seen() / saved_seen() : uint64
    +loaded_seen(seen)
  }

  class CfgNetworkStructure {
    +{static} of_text(cfg, name) : CfgNetworkStructurePtr
    +{static} of_file(path) : CfgNetworkStructurePtr
    +structure() : Layer[]
  }

  interface IWeightsReader {
    +load(net, filename, cutoff) : bool
    +load(net, filename, cutoff, rest) : bool
    +read(filename, net, document) : bool
    +peek(filename, rest, weights) : bool
  }

  interface IWeightsWriter {
    +save(net, filename, cutoff, save_ema) : bool
    +save(net, filename, cutoff, save_ema, rest) : bool
    +write(document, filename) : bool
  }

  interface IWeightsFileFormat {
    +name() / extension() / description() : string
    +keeps_structure() / keeps_network() : bool
    +keeps_annotations() / keeps_images() : bool
    +keeps_description() : bool
    +create_reader() : IWeightsReaderPtr
    +create_writer() : IWeightsWriterPtr
  }

  class WeightsFileFormats {
    +{static} all() : formats
    +{static} convertible() : formats
    +{static} named(name, formats) : IWeightsFileFormatPtr
    +{static} of_file(path) : IWeightsFileFormatPtr
  }

  class NetworkSource {
    +{static} describes(path) : bool
    +{static} read(path, description / rest) : bool
    +{static} base_of(path, iteration) : string
    +{static} trained_of(path) / resumed_of(path) : string
    +{static} keeps_images(path) : bool
  }

  class WeightsDocument {
    +header : WeightsHeader
    +layers : WeightsLayer[]
    +trailing : bytes
    +network : NetworkDescription
    +training : TrainingRecord
    +annotations : Yolo4Annotations
    +description : string
  }

  class WeightsConversion {
    +from / to : IWeightsFileFormatPtr
    +src / dst / cfg / names / description : string
    +stop_requested : function
  }

  class WeightsConverter {
    +convert(conversion) : bool
    +{static} write_destination(conversion, document) : bool
    #create_network(cfg) : INetworkWeightsPtr
  }

  class CfgSection {
    +{static} read_cfg(filename) : CfgSections
    +{static} read_cfg(text, name) : CfgSections
  }

  class "LayerType, WeightsSchema,\nWeightsLayout, WeightsReport" as Schema

  class "the formats: the original weights, the dxxwjz1 ... dxxwjz4p,\nthe YOLO4 folder, their validators, the ZlibController" as Formats <<inside the shared object>>

  class "darknetxxConverter" as DxxConverter <<binary>>
}

package "darknet_adaptor::loaders" {
  class NetworkWeights {
    +NetworkWeights(net)
    +NetworkWeights(adaptor)
    +{static} create(adaptor) : NetworkWeightsPtr
    +{static} layout_of(net, options) : WeightsLayoutEntries
    +{static} type_of(type) : LayerType / LAYER_TYPE
  }

  class NetworkWeightsConverter {
    +{static} create() : NetworkWeightsConverterPtr
    #create_network(cfg) : INetworkWeightsPtr
  }

  class NetworkRewriter {
    +{static} rewrite(src, dst, network, message) : bool
  }

  class OrigDefaultLoader {
    +load(dctx) : bool
    +parse_network_cfg_custom(cfg, batch, time_steps) : ANetworkAdaptorPtr
    +parse_network_text(cfg, name, batch, time_steps) : ANetworkAdaptorPtr
  }
}

class TrainingJournal {
  +record(iteration, loss, avg_loss)
  +record_map(iteration, map)
  +save(net, stem, ema) : bool
  +rest() : WeightsDocument
}

class TrainingDataRegistry {
  +{static} publish(annotations)
  +{static} withdraw()
}

class TrainingChart {
  +{static} render(rest, png) : bool
  +{static} render_file(weights, png) : bool
}

class "NetworksCApi (extern C)" as NetworksCApi {
  +load_weights_upto(net, filename, cutoff)
  +save_weights_upto(net, filename, cutoff, save_ema)
}

INetworkWeights <|.. CfgNetworkStructure
INetworkWeights <|.. NetworkWeights
WeightsConverter <|-- NetworkWeightsConverter
IWeightsFileFormat <|.. Formats
WeightsFileFormats o-- IWeightsFileFormat
IWeightsFileFormat ..> IWeightsReader : creates
IWeightsFileFormat ..> IWeightsWriter : creates
IWeightsReader ..> INetworkWeights : loads the files into
IWeightsWriter ..> INetworkWeights : saves the files of
IWeightsReader ..> WeightsDocument : reads
IWeightsWriter ..> WeightsDocument : writes
Formats ..> Schema : the arrays of the layers
CfgNetworkStructure ..> CfgSection : walks the cfg sections of
WeightsConverter ..> WeightsConversion
WeightsConverter ..> WeightsFileFormats : reads the source, writes the destination by
WeightsConverter ..> CfgNetworkStructure : the default network of the cfg
DxxConverter ..> WeightsConverter : converts by
NetworkSource ..> WeightsFileFormats : peeks the files by

NetworkWeights ..> Schema : walks the darknet layers into
NetworkWeightsConverter ..> OrigDefaultLoader : builds the darknet network of the cfg
NetworkWeightsConverter ..> NetworkWeights : hands the network over as
NetworkRewriter ..> OrigDefaultLoader : builds the edited network
NetworkRewriter ..> NetworkWeights : the arrays of the edited network
NetworkRewriter ..> CfgSection : compares the sections by
NetworkRewriter ..> WeightsConverter : writes the destination by
OrigDefaultLoader ..> CfgSection : parses the cfg by
OrigDefaultLoader ..> NetworkSource : parses the network of
OrigDefaultLoader ..> NetworkWeights : loads the weights into
OrigDefaultLoader ..> TrainingJournal : starts the journal of the training
OrigDefaultLoader ..> TrainingDataRegistry : the lists of the project training
NetworksCApi ..> NetworkWeights : loads and saves the weights through
NetworksCApi ..> WeightsFileFormats : the format of the file
TrainingJournal ..> NetworkWeights : saves the training weights through
TrainingJournal ..> IWeightsFileFormat : saves by
TrainingChart ..> WeightsFileFormats : peeks the files by
TrainingDataRegistry ..> WeightsDocument : the annotations of
"darknetxx_impl::WeightsConversionLib" ..> NetworkWeightsConverter : converts the library context by
"darknetxx012::LibraryFacade" ..> NetworkRewriter : saves the edited network by
@enduml

### The alias header

The [`DataDriversAliases.h`](/src/NetworksLoaders/DataDriversAliases.h) header is the one and only place of the whole project which names the library: it includes the `<DarknetxxDataDrivers-0.12/DxxDataDriversFacade.h>` header, which pulls every other public header of the library in (the documents, the formats registry, the network sources, the cfg sections and structure, the YOLO4 folder layout and the converter), and declares the `dxxdd` alias of the `DarknetxxDataDrivers012` namespace, the name of which carries the major and the minor version of the library. Every other source of the project reaches the library through the alias (the `dxxdd::WeightsDocument`, the `dxxdd::WeightsFileFormats` and the rest), so bumping the library to the namespace of the next version stays a one line change there. The header is included through the versioned `<DarknetxxDataDrivers-0.12/...>` directory, not through the plain `<DxxDataDriversFacade.h>` the unprefixed include root resolves too: the prefix pins the library version every translation unit talks to, and the generic names of the same root (the `CfgSection.h`, the `WeightsDocument.h`) are the ones a busy include path collides with.

### The NetworkWeights view

The library knows no darknet network: it's readers load the weights of the files into the arrays of the `INetworkWeights` view of the library user, it's writers save the arrays of the view into the files, the files of no structure of their own are read by the arrays of the view and the documents of the rest are validated against them. The **`NetworkWeights`** ([`NetworkWeights.h`](/src/NetworksLoaders/network-weights/NetworkWeights.h)) is that view over the darknet network struct:

- `layers()`, `type(layer)`, `classes()` and `names()` - the layers of the network, the library types of them, the classes of the last layer and the `names=` key of the cfg of the view made of a network adaptor;
- `layout(options)` - the float arrays of the network in the order of the weights files of the options (the saves, the EMA saves, the loads with their transpositions, up to the cutoff), the very arrays of the network struct the readers load into and the writers save of. The walk of the darknet network layers the original `save_weights_upto()` and `load_weights_upto()` did - the `WeightsLayout::of()` of the former weights model - moved into this project as the `NetworkWeights::layout_of()`, since it walks the darknet layers;
- `seen()`, `loaded_seen(seen)` and `saved_seen()` - the count of the images the network has seen, the seen of the loaded file with the current iteration of the network following it (the `get_current_batch()`), the seen of the saved file of the current iteration (the `get_current_iteration()` times the batch and the subdivisions), the way the original functions did.

The view is made of the network struct for the call alone (the `NetworkWeights{net}` the `OrigDefaultLoader`, the `load_weights_upto()` and the `save_weights_upto()` of the darknet C API and the `TrainingJournal` hand over to the readers and the writers), of the const network struct the documents are read and validated by, or of the network adaptor the view keeps alive (the `NetworkWeights::create()` of the `NetworkWeightsConverter`). The [`NetworkWeights.cpp`](/src/NetworksLoaders/network-weights/NetworkWeights.cpp) checks at the compile time that every `dxxdd::LayerType` is the darknet `LAYER_TYPE` of the same name value for value, so the two convert by the `static_cast` alone (the `NetworkWeights::type_of()` of both ways), and that the version of the weights files the library writes (the `WeightsHeader::CURRENT_MAJOR`, `CURRENT_MINOR` and `CURRENT_REVISION` of it) is the `MAJOR_VERSION`, the `MINOR_VERSION` and the `PATCH_VERSION` of the `version.h` of the darknet sources: a library of other types or of another version fails the build of the project instead of the reads of the files.

### The two networks of the cfg

The original darknet weights file keeps no structure of it's own, so it is read by the network of it's cfg file, which the library user makes (the `WeightsConverter::create_network()`), and the two binaries of the conversion make it two different ways:

- the **`NetworkWeightsConverter`** ([`NetworkWeightsConverter.h`](/src/NetworksLoaders/network-weights/NetworkWeightsConverter.h)) of this project is the `dxxdd::WeightsConverter` of the darknet networks: it's `create_network()` is the `NetworkWeights` view of the real darknet network the `OrigDefaultLoader::parse_network_cfg_custom()` builds of the cfg (the single image batch network of the least memory, the arrays of the weights don't depend on the batch), the very network the darknetxx trains and detects with. The `darknetxx_impl::WeightsConversionLib` of the darknetxx library, so the conversion flags of the `darknetxx` binary, converts by it.
- the **`CfgNetworkStructure`** of the library is the structure of the network the library makes of the cfg text itself, of no darknet code: it walks the cfg sections the way the `parse_network_cfg_custom()` of the darknet does - the `[net]` input, then every layer of the input the previous one outputs or the layers it's `from=` and `layers=` keys name - and makes the layers, the sizes of their inputs and outputs and the float arrays every darknet layer allocates, of no memory of them (the `nullptr` data). It is the default network of the `create_network()` of the library converter, so the `darknetxxConverter` binary of the library reads the original weights files by it.

The two have to be the layout of the very same arrays, so both binaries read and write the files the very same way. The **`CTEST_NetworkWeights`** component test ([`CTEST_NetworkWeights.cpp`](/src/NetworksLoaders/network-weights/tests/component/NetworkWeights/CTEST_NetworkWeights.cpp)) checks it for every cfg file of the [`misc/examples/cfg`](/misc/examples/cfg) directory and of the network loader tests: the count of the layers, the type of every layer, the classes, the `names=` key and the layout of the saves, the EMA saves, the loads, the transposed loads and the cutoff, entry by entry. The cfg files the darknet builds no network of from there (the ones of the files the darknet reads beside them, the trees and the maps) are skipped, while more than a half of the files has to be compared. The test checks the layer types both ways and the arrays of the view being the ones of the network as well, and round trips the arrays of a network through the original weights and the dxxwjz1 files of the view.

## The readers and the writers

Every format implements the three interfaces of the library:

- **`IWeightsReader`** - the `load(net, filename, cutoff)` method loads the weights of the layers before the cutoff into the arrays of the network view (the `load_weights_upto()` of the original Darknet), the `load(net, filename, cutoff, rest)` one keeps the rest of the file as well: the document without the arrays, of the network description and the training record of the formats which keep them. The `read(filename, net, document)` method reads the whole file into the format neutral `WeightsDocument`, the `peek(filename, rest, weights)` one reads the rest of the file up to it's first weights array and tells the file holds weights.
- **`IWeightsWriter`** - the `save(net, filename, cutoff, save_ema)` method saves the weights of the layers before the cutoff (the `save_weights_upto()` of the original Darknet: the current version, the seen of the current iteration, the EMA arrays of the convolutional layers for the `save_ema`), the `save(net, filename, cutoff, save_ema, rest)` one saves the network description and the training record of the rest beside them, which the formats of the weights alone leave out. The `write(document, filename)` method writes the document.
- **`IWeightsFileFormat`** - the format itself: it's name (the one of the command line), it's file name extension and description, whether it's files keep the structure of the network (the layers and the names of the arrays, so they're read without the network), the network description, the annotations, the images and the description text, the reader and the writer of it's files.

The `WeightsFileFormats` finds the formats of their names and of the file name extensions and lists the formats of the conversion: every one and the YOLO4 folder (it's `convertible()`), the `NetworkSource` reads the network description of the file a network is made of - the text of the cfg file or the description the dxxwjz2, the dxxwjz3 and the dxxwjz4 files and their projects keep - and the keys of the `[net]` section of it. The readers and the writers of the formats load and save the same arrays of the network the same way, so any file of a network loads the same network, while the `read()` and the `write()` of the documents are what converts the files between the formats.

## The original darknet weights file

The binary file the original Darknet reads and writes: the int32 `major`, `minor` and `revision` version triple, the count of the images the network has seen (the uint64 `seen` of the `(major * 10 + minor) >= 2` versions, the uint32 one of the older versions) and the float arrays of the layers with the weights one after another, in the order of the layers. The file keeps no structure of it's own: which float belongs to which array is told by the network alone (see the [The two networks of the cfg](#the-two-networks-of-the-cfg) section). The reader and the writer of the library are the C++ ports of the original `load_weights_upto()`, `save_weights_upto()` and the per layer routines, which read and write exactly the bytes the original did: the `dontload` layers and the batch normalization arrays of the `dontloadscales` ones skipped, the weights of the `flipped` convolutional layers and the connected weights of the files of the major or minor version above 1000 transposed, the truncated files and the extra bytes after the arrays kept. The validator compares the size of the file with the arrays of the network: the file of the size of neither the saved nor the loaded arrays is truncated or holds extra bytes (the warnings), the gzip compressed file and the file shorter than the header are the errors.

## The dxxwjz1 file format of the first version

The dxxwjz1 file is the gzip compressed UTF-8 JSON document of the network weights, which the stock gzip tools unpack (`gzip -dc model.dxxwjz1`) and any JSON tool reads:

```
{
  "format": "dxxwjz1",
  "version": 1,
  "generator": "DarknetxxDataDrivers-0.12 0.12.0",
  "header": {"major": 0, "minor": 2, "revision": 5, "seen": 64},
  "layers": [
    {"index": 0, "type": "convolutional", "arrays": {
      "biases": [0.5,-0.25],
      "weights": [0.125,"0x7fc00000",-0.0,1e-45]
    }},
    {"index": 1, "type": "maxpool"}
  ],
  "trailing": "AAE="
}
```

The `header` is the one of the original weights file, the `layers` are the objects of the layers of the weights of their `index` and `type` (the name of the cfg section of the layer), of the `arrays` named after the fields of the darknet layers (the `biases`, the `scales`, the `rolling_mean`, the `rolling_variance`, the `weights`) or of the `sublayers` of the recurrent layers, and the `trailing` is the base64 text of the bytes the original file keeps after the arrays. The finite floats are the shortest decimal text the float round trips of, the infinities and the NaNs the `"0x"` strings of their bit patterns, so every float of the file is kept bit by bit, and the readers find the arrays by their layers and names, so the file loads the network the original weights file does. The `generator` names the library which wrote the file and it's version, the `DarknetxxDataDrivers-0.12 0.12.0` of the darknetxx and of the `darknetxxConverter` binary alike, and is informational alone. The compressed file takes about 1.35 times the size of the original weights file.

## The dxxwjz2 file format of the second version

The dxxwjz2 file is the dxxwjz1 document of everything the network is detected and trained with: the `network` member of the text of the network cfg file and of the class names (the objects of their `index` and `name`) and the `training` member of the training record (the `seconds` the training took, the `iteration`, the `loss` and the `avg_loss` columns of every training iteration and the `map_iteration` and the `map` ones of every mAP calculation) beside the weights. So the cfg file of the network is recovered of the file with any JSON tool (`gzip -dc net.dxxwjz2 | jq -r .network.cfg > net.cfg`).

**The file of no weights is the network to train.** The dxxwjz2 file of no weights (no float in it's arrays) loads no weights and the network keeps it's initialization: the training trains such a file, while the detection refuses it (see the [Training with the formats](#training-with-the-formats) section). The file of the `network` alone (of no layers: the one the converter makes of a cfg file, see the [conversion](#the-conversion), or a tool exports) is the network to train from scratch, neither the seen of it's header nor it's training record is resumed.

## The dxxwjz2p project file format

The dxxwjz2p file is the dxxwjz2 document of the YOLO4 training project: the network description, the weights and the training record beside the `annotations` member of the YOLO4 annotations of the images, the project document of the [ImagesAnnotator](https://github.com/yuriysydor1991/ImagesAnnotator) (of it's 0.12.0 structure) of the folders of the images (their `db` paths, relative to the folder of the project file or absolute), the images of their `rel_path`, `width`, `height` and `validation` flag and the boxes of their class `name` and pixel `rect`. So the project is everything the training needs but the images themselves: the single dxxwjz2p file of the absolute image paths, or the dxxwjz2p file of the image paths relative to it beside the folders of the images, which moves with them as a whole. The file is the dxxwjz2 one for everything else: the detection runs the network of the project alone, the converter makes the dxxwjz2 weights file of it of no annotations, the dxxwjz2p file of no weights is the network to train.

## The dxxwjz3 file format of the third version

The dxxwjz3 file is the dxxwjz2 document of nothing but the JSON data: the `network` member keeps the JSON structure of the network cfg instead of the text of the cfg file - the `net` object of the options of the `[net]` section, the `layers` array of the layer sections of their `type` and `options` with the `arrays` of their layers, the `classes` array of the class names - and the trailing bytes are the JSON array of their values, so any JSON tool reads and edits every option of the network (`gzip -dc net.dxxwjz3 | jq .network.net.batch`). The options are the JSON values of the darknet ones: the integers, the fractions of the floats, the strings, the arrays of the comma separated values and the `null` of the option line of no `=`.

The readers turn the network member into the cfg text of the network, so the rest of the darknetxx works with the cfg text of the dxxwjz3 file the way it does with the one of the dxxwjz2 file, and the `--dump-cfg` flag prints it (see the [Printing the network cfg of the file](/doc/sections/en_US/6-2-4-converting-the-network-weights-files.md#printing-the-network-cfg-of-the-file) section). The JSON keeps no comments and no white space of the cfg file, nor the repeated keys of a section: the cfg text converted into the dxxwjz3 file and back is the same network of the normalized cfg text, which converts into the same dxxwjz3 file again. The dxxwjz3 file is the dxxwjz2 one for everything else: the training saves it by default (the `--save-format dxxwjz3`), the converter converts it into the other formats and back of the weights of the same bytes, the file of no weights is the network to train, the one the ImagesAnnotator writes beside the cfg of the YOLO4 folder it exports (see the [The files of the ImagesAnnotator](#the-files-of-the-imagesannotator) section).

## The dxxwjz3p project file format

The dxxwjz3p file is the dxxwjz2p project of the dxxwjz3 document, so the whole training project is the JSON data alone: the network member and the trailing bytes are the dxxwjz3 ones, the annotations member is the dxxwjz2p one. The ImagesAnnotator exports it's annotations into the dxxwjz3p project of the network to train (see the [The files of the ImagesAnnotator](#the-files-of-the-imagesannotator) section). The file is the dxxwjz2p one for everything else: the `--train-cfg project.dxxwjz3p` training trains the images of it's annotations and updates the project, the training GUI opens it, the detection runs it's network, the converter converts it into the dxxwjz2p project and back of the same weights and annotations and into the dxxwjz3 weights file of no annotations.

## The dxxwjz4 file format of the fourth version

The dxxwjz4 file is the dxxwjz3 document of the `description` member: the comments and the description of the network and of it's data the users write, of any length (the megabytes of the text included), the JSON string of the UTF-8 text of any lines, quotes and languages, which the writers write before the network, so a glance at the head of the file shows it (`gzip -dc net.dxxwjz4 | head -c 2000`). The description of the conversion is the `--convert-description <TEXT>` command line parameter, over the one of the source (see the [Converting the network weights files](/doc/sections/en_US/6-2-4-converting-the-network-weights-files.md) section); the files of the earlier formats keep no description, so the conversion into them drops it with a warning. The dxxwjz4 file is the network the training of the dxxwjz4p project saves beside the project (see the [Training the dxxwjz4p project](#training-the-dxxwjz4p-project) section), the training and the detection read it the way they read the dxxwjz3 one.

## The dxxwjz4p project file format

The dxxwjz4p file is **the whole training project in one file**: the network, the weights, the training record, the description text, the annotations of the images **and the images themselves**, so the project is copied, mailed and archived as the single file and trains on any machine with no folders of the images beside it. The file is the standard ZIP archive (of the Zip64 records of the members and the archives of 4 GiB and more) of the single JSON document of the `project.json` member, the first one, and of the copies of the images of it's annotations in the `data/` folder (the same names of the images of the different folders get the `_<n>` suffix), so any zip tool lists and extracts it of no darknetxx at all:

```
$ unzip -l cars.dxxwjz4p
Archive:  cars.dxxwjz4p
  Length      Date    Time    Name
---------  ---------- -----   ----
  9410833  2026-11-02 10:00   project.json
        0  2026-11-02 10:00   data/
   184220  2026-11-02 10:00   data/park.jpg
   201311  2026-11-02 10:00   data/park_1.jpg
    96112  2026-11-02 10:00   data/street.png
---------                     -------
  9892476                     5 files
```

The JSON document is the dxxwjz3p one of the description member of the dxxwjz4 file, the db paths of the annotations are relative to the root of the archive, so the project packed by hand of the JSON document and of the images is read as well (`cd cars && zip -r ../cars.dxxwjz4p project.json data`). The readers refuse what isn't the complete project of it's images safely unpacked: the members of no path inside of the project, of the types other than the regular files and the folders, the repeated, the encrypted members and the ones of the compression methods other than the stored and the deflated ones, the corrupted and the truncated archives, the images of the annotations the archive has no member of.

### The unpacked projects

The project is read of the folder it's unpacked into: the `darknetxx-dxxwjz4p-<name>-<random>` folder of the temporary directory (the one of the `TMPDIR` environment variable, the `/tmp` of none), of the owner's access alone. The project is unpacked the first time the process reads it and once the file changed, so the reads of the same project (the peeks of the network, the load of the weights, the training) share the folder, which is removed at the end of the process:

```
The /srv/cars.dxxwjz4p project is unpacked into /tmp/darknetxx-dxxwjz4p-cars-5f0c1e93a7d2b4c6
```

The images of the annotations read are the ones of the folder, so the dxxwjz4p project converted into the dxxwjz3p or the dxxwjz2p one (of the image paths alone) gets the copies of the images beside the destination first: the `data/park.jpg` of the project is the `data/park.jpg` beside the `out/cars.dxxwjz3p` file, the same layout of the archive unpacked. The existing image of another content isn't replaced, the conversion fails instead.

## The YOLO4 training folder

The YOLO4 training folder, the one the ImagesAnnotator exports, is the `yolo4` format of the conversion alone: of no file name extension, so neither the training nor the detection takes a folder for a weights file. The reader of the library reads the folder into the document of the dxxwjz2p project the way the training reads it: the first `cfg/*.cfg` network, the `data/obj.names` classes, the images of the `train=` and the `valid=` lists of the cfg with their sizes and the boxes of their label files, the latest weights of the backup folder (the original weights file of them read by the structure of the cfg of the folder, the `CfgNetworkStructure`, unless the conversion names a cfg file of it's own). The writer writes the document into the new folder of the `cfg/<name>.cfg` network, the `data/obj.names`, `data/obj.data`, `data/train.txt` and `data/val.txt` files, the copies of the images in the `data/obj/` folder with their labels and the `backup/<name>_last.dxxwjz3` weights; the existing folder of files is no destination, so no folder of the user is deleted. The folder converted into the dxxwjz2p file and back is of the same lists, labels, class names and weights.

## The files of the ImagesAnnotator

The [ImagesAnnotator](https://github.com/yuriysydor1991/ImagesAnnotator), the annotator of the images the darknetxx recommends, generates the files of the most recent formats itself: the `DarknetXX` submenu of its export menu (and the formats of the `--convert-to` of its command line) writes the annotations of its project into the four layouts of the [ImagesAnnotator-DataExporters](https://github.com/yuriysydor1991/ImagesAnnotator-DataExporters) library, which the darknetxx trains with no conversion:

| The entry (the format) | The layout | The training inside of it |
| --- | --- | --- |
| `dxxwjz3p project` (`dxxwjz3p`) | the `yolov4-obj.dxxwjz3p` project beside the `images/` folder of the copies of the images and the empty `backup/` one | `darknetxx --train-cfg yolov4-obj.dxxwjz3p` |
| `dxxwjz2p project` (`dxxwjz2p`) | the same of the `yolov4-obj.dxxwjz2p` project | `darknetxx --train-cfg yolov4-obj.dxxwjz2p` |
| `dxxwjz3 with YOLO4 folder` (`dxxwjz3`) | the YOLO4 training folder of the `cfg/yolov4-obj.dxxwjz3` network to train beside it's `cfg/yolov4-obj.cfg` | `darknetxx --train-cfg cfg/yolov4-obj.dxxwjz3` |
| `dxxwjz2 with YOLO4 folder` (`dxxwjz2`) | the same of the `cfg/yolov4-obj.dxxwjz2` network to train | `darknetxx --train-cfg cfg/yolov4-obj.dxxwjz2` |

The training GUI opens the projects and the folders as well. The network is the YOLO v4 one of the YOLO4 folder the ImagesAnnotator exports, written for the classes of the project (the sorted names of its annotations), of no weights, so the training starts from scratch. The files are the ones the darknetxx writes, byte to byte but the `generator` key: the networks of the folders are the ones the converter makes of the cfg and the class names files of the folder (`--convert-to dxxwjz3 --convert-dst <file> --convert-cfg cfg/yolov4-obj.cfg --convert-names data/obj.names`), the projects are the ones the converter writes back of them. The annotations of a project are the integer pixel rects of the images of the `images` folder relative to the project file, so the folder of the project moves as a whole, and no image is the validation one: the mAP is calculated on the training images.

The `DarknetXX` submenu of the import menu (and the formats of the `--convert-from`) reads the four layouts back by the [ImagesAnnotator-DataImporters](https://github.com/yuriysydor1991/ImagesAnnotator-DataImporters) library: the annotations of the dxxwjz2p and the dxxwjz3p projects, the ones the training has saved the weights into included, and the YOLO4 folders, of the class names of their network when they have no `data/obj.names` file. The import refuses the documents of another format or version and steps over the weights a chunk at a time, so the project of the hundreds of megabytes of the weights is read in the memory of its annotations. The dxxwjz1 and the original weights files keep the weights alone, so they have no layout.

```
# the annotations project of the ImagesAnnotator to the dxxwjz3p project
ImagesAnnotatorConverter \
  --convert-from imagesannotator \
  --convert-src ./cars.json \
  --convert-to dxxwjz3p \
  --convert-dst ./cars

# the training inside of the project folder saves the weights into the project
(cd ./cars && darknetxx --train-cfg yolov4-obj.dxxwjz3p)

# the annotations of the trained project back to the annotations project
ImagesAnnotatorConverter \
  --convert-from dxxwjz3p \
  --convert-src ./cars/yolov4-obj.dxxwjz3p \
  --convert-to imagesannotator \
  --convert-dst ./cars-trained.json
```

Checked on 2026-10-11 against the ImagesAnnotator `bc4b605`, the ImagesAnnotator-DataExporters `d730625` (0.12.0) and the ImagesAnnotator-DataImporters `814d224` (0.14.0).

## The format neutral model

The formats read the files into and write them of the format neutral model of the library, the public headers of which the darknetxx works with:

- **`WeightsDocument`** - the content of a weights file: the `WeightsHeader`, the layers with their arrays and the bytes after the arrays, which keep every byte of the original weights file (the truncated arrays, the part of a float the truncated file ends in and the extra bytes included), beside the `NetworkDescription` (the cfg text and the class names), the `TrainingRecord` (the seconds, the losses of the iterations and the mAPs of the mAP calculations), the `Yolo4Annotations` of the `Yolo4Image` and the `Yolo4Box` (the annotations of the projects) and the `description` text the dxxwjz4 and the dxxwjz4p files keep.
- **`WeightsSchema`** and **`WeightsLayout`** - the arrays and the sub-layers of every layer type, their order and their names, the `WeightsLayoutOptions` and the `WeightsLayoutEntry` of the arrays of a network in the order of the files, which the network makes itself (the `NetworkWeights::layout_of()` of the darknet networks, the walk of the cfg sections of the `CfgNetworkStructure`).
- **`NetworkFiles`** - the reads of the text of the cfg file and of the names of the darknet class names file.
- **`WeightsReport`** - the findings of the validators of the library: the errors, which keep a file from being loaded or converted, and the warnings of the files which load, but not the way their authors might have expected (the truncated file, the extra bytes, the unknown keys of a JSON document).

## The conversion

The **`WeightsConverter`** of the library performs the **`WeightsConversion`** of a file of one format into another one (or into the same one): the network of the cfg file is made (the original weights file requires it, the rest is validated against it when it's given), the source is validated and read into the document, the document is written into the temporary file of the destination directory, which replaces the destination once complete. A failed conversion leaves the destination as it was and no temporary file behind, and the `stop_requested` of the conversion (the stop request of the application, a Ctrl+C) stops the conversion between it's steps.

The network of the cfg file is the one of the `create_network()` of the converter: the darknetxx converts by the `NetworkWeightsConverter` of the darknet network of the cfg (the `darknetxx_impl::WeightsConversionLib` of the darknetxx library creates it of the conversion parameters of the library context), the `darknetxxConverter` binary of the library by the `CfgNetworkStructure` of the cfg sections, the layouts of which the `CTEST_NetworkWeights` test checks are the same (see the [The two networks of the cfg](#the-two-networks-of-the-cfg) section). The YOLO4 folder source of no cfg file of the conversion reads the original weights file of it's backup folder by the `CfgNetworkStructure` of the cfg of the folder in both binaries.

The rest of the conversion is the one of the library: the original weights file converted to the dxxwjz1, the dxxwjz2 or the dxxwjz3 one and back is the same file byte to byte, whatever it holds; the dxxwjz2, the dxxwjz3 and the dxxwjz4 destinations keep the network description (the cfg text of the cfg file of the conversion over the one of the source, the class names of the class names file of the conversion over the classes of the source, over the `names=` file of the cfg) and the training record of the source; the dxxwjz4 and the dxxwjz4p ones the description text of the `--convert-description` over the one of the source; the dxxwjz2p, the dxxwjz3p and the dxxwjz4p ones the annotations of the project and of the YOLO4 folder sources (the dxxwjz4p destination packs the images of them, the dxxwjz2p and the dxxwjz3p ones of the dxxwjz4p source get the copies of them beside, see the [The unpacked projects](#the-unpacked-projects) section), while the project converted into the dxxwjz2 or the dxxwjz3 file is the weights file of the inference of it, of no annotations. The conversion of no source file makes the network to train of the cfg file: the file of the network description alone, of no layers, which converts into the formats of the network description alone. The command line of the conversion is described by the [Converting the network weights files](/doc/sections/en_US/6-2-4-converting-the-network-weights-files.md) section.

## Saving the edited network

The **`NetworkRewriter`** ([`NetworkRewriter.h`](/src/NetworksLoaders/network-rewriter/NetworkRewriter.h)) of this project writes the edited network description (the cfg text and the class names, the ones the network editors edit, see the [The network editor binaries](/doc/sections/en_US/5-38-the-network-editor-binaries.md) section) into the file of a format keeping it (the dxxwjz3p project, for example, of the file name extension of the destination) through the temporary file the way the conversion does (the `WeightsConverter::write_destination()` of the darknetxx-datadrivers library), the `LibraryFacade::save_network()` of the darknetxx library. It stays in this project, since it validates the edit by the darknet network of the edited cfg, which the darknetxx-datadrivers library has none of. The source file (the project opened, the file of the save as or none for the new network) goes on into the destination: the annotations, the training record, the header and the weights, the source and the destination may be the same file.

The darknet builds the network of the cfg first (the single image inference network, the `OrigDefaultLoader::parse_network_text()`), so the cfg the darknet refuses is no save, nor is the network the training can't run: the `[route]` layer of the input layers of the different sizes (which the parse warns of and the first resize of the training refuses) and the layer of no outputs. The weights of a layer go on while every layer up to it keeps it's cfg section (the type and the options of the source cfg) and it's arrays are of the sizes of the arrays of the built network (the `NetworkWeights::layout_of()` of it, which the `[net]` channels, width and height size too): so the edit of the `[net]` options alone keeps every weight, the training record and the seen images, while the edit of a layer drops the weights of the layer and of the ones after it. The document of the dropped weights is the network to train of the weights kept, the transfer learning one (the darknet `partial` and the `-clear` way): of no seen images, no training record and no trailing bytes, the one of no weights at all is the network to train from scratch. The save tells the dropped weights:

```
The weights of the layer 11 and of the ones after it were dropped, the network changed them
```

The images of the annotations are the absolute paths of the source in the memory, so the project saved into another folder goes on referring to the images of the source one (the folders below the one of the destination are written relative to it).

## Training with the formats

The training saves its weights into the dxxwjz3 files by default (the `--save-format` command line parameter names the format of them, the `LibraryContext::save_format` of the library): the `<base>_<iteration>_<avg loss>` (the numbered ones, of the average loss of their iteration, e.g. the `net_1200_0.734521`), the `<base>_last`, the `<base>_final`, the `<base>_best` and the `<base>_ema` files of the backup directory, with the extension of the format. The C training code saves them by the `weights_saver` of its context, which the `DarknetContext` points to the **`TrainingJournal`** ([`TrainingJournal.h`](/src/darknet-adaptor/adaptors/TrainingJournal/TrainingJournal.h)) of the training:

- the journal starts with the network description of the training: the text of the network cfg file of the training (or of the dxxwjz2 one) and the class names of the `--names` file, of the dxxwjz2 weights file, of the dxxwjz2 network file or of the `names=` file of the cfg, the first of them given;
- it resumes the training record of the dxxwjz2 weights file the training starts from up to the iteration of the weights (the record of the iterations after it is dropped, the `--clear` drops the whole record with the iterations), the seconds of it's clock start at the seconds of the record;
- it records the losses of every iteration and the mAP of every mAP calculation once, in the ascending order of the iterations, the iteration saved before it's report included, and saves every weights file of the time of the training so far.

So the file of a training resumed of a dxxwjz2 one keeps the losses and the mAPs of every training iteration in their order and the whole time of all the trainings, and the average loss of the resumed training goes on of the last one of the record. The chart of every training start is the new `chart_<base>_<YYYY-MM-DD--HH-MM-SS>.png` file of the date and the time of the start, so no chart of the trainings resumed before is overwritten. The whole record of the resumed weights (the average losses and the mAP line) is redrawn on it and the chart is saved at once, then every 100 iterations (every iteration of the `--save-charts-every-iter`), at the end and at the stop of the training; the `--chart-path` image goes on for the weights of no record alone (the original and the dxxwjz1 ones). The chart of the record of a dxxwjz2 file is drawn at any moment as well (the `--draw-chart` command line parameter, the `TrainingChart` class and the `LibraryFacade::training_chart()` of the library, the chart the GUI shows as soon as the training folder is opened).

The mAP calculation of the training (the `--map`) takes the class names the same way, so the class names of the dxxwjz2 file need no names file; the names of a count other than the classes of the network fail the training.

The detection and the training read the weights files of every format, the format told by the file name extension, and the file of the network is a cfg file or a dxxwjz2 one (the dxxwjz3 and the dxxwjz4 files and the dxxwjz3p and the dxxwjz4p projects are read the way the dxxwjz2 and the dxxwjz2p ones are):

| Command line | The network | The weights |
| --- | --- | --- |
| `--train-cfg net.cfg --weights w` | of the `net.cfg` | of the `w` file of any format, the training record of the dxxwjz2 one resumed |
| `--train-cfg net.dxxwjz2` | of the cfg the file keeps | of the file, none for the file of no weights (the training from scratch) |
| `--detect-cfg net.cfg --weights w` | of the `net.cfg` | of the `w` file of any format, the dxxwjz2 one of no weights refused |
| `--detect-cfg net.dxxwjz2` | of the cfg the file keeps | of the file |
| `--weights net.dxxwjz2` alone | of the cfg the file keeps | the detection of the weights of the file, the training of the network of the file of no weights |
| `--train-cfg project.dxxwjz2p` | of the cfg the file keeps | of the file, the training of the images of the annotations of the file, which updates it (see below) |
| `--detect-cfg project.dxxwjz2p`, `--weights project.dxxwjz2p` alone | of the cfg the file keeps | the ones of the dxxwjz2 file |
| `--train-cfg project.dxxwjz4p` | of the cfg the project keeps | of the `project.dxxwjz4` file beside the project once it's there, of the project otherwise; the training of the images of the project unpacked into the temporary directory, which saves the `project.dxxwjz4` file (see the [Training the dxxwjz4p project](#training-the-dxxwjz4p-project) section) |

The backups of a training are named after the file of it's network: the `<base>` of the `net.cfg` and of the `net.dxxwjz2` files is the `net`, and the one of the backup files of a training (the `net_last`, the `net_final`, the `net_best`, the `net_ema` and the `net_<iteration>_<avg loss>` dxxwjz2 files of the iteration of their weights, the `net_<iteration>` ones of the earlier versions too) is the `net` too, so the training resumed of the backup file alone goes on with the same files.

## Training the dxxwjz2p project

The dxxwjz3p project trains the same way, of the dxxwjz3p file of it's latest weights. The training of the dxxwjz2p file (the `--train-cfg project.dxxwjz2p` command line, the dxxwjz2p project the training GUI opens) trains the network of the file, resumed from it's weights, on the images of it's annotations: the image lists and the labels of the training are the ones of the annotations, over the `train=` and the `valid=` lists of the cfg and the label files beside the images. The `OrigDefaultLoader` names them the lists of the training networks and the `DarknetContext` of the training publishes them in the `TrainingDataRegistry` of the [`TrainingData`](/src/darknet-adaptor/adaptors/TrainingData) directory, which it withdraws at the end of the training. The darknet C code reads them by the `get_paths()` of the `dxxwjz2p:train` and the `dxxwjz2p:valid` list names and the `read_boxes()` of the label paths of the images, which ask the registry before the files (the `get_training_data_paths()`, the `has_training_data_paths()` and the `read_training_data_boxes()` C functions of the `TrainingDataCApi`). So the images stay where they are, the project keeps no copies of them and no list or label file is written.

Every save of the training updates the project as well: the dxxwjz2p file of the latest weights, of the training record and of the annotations is written into the `<project>.saving` file beside the project, which replaces the project once complete. So the training of the project resumes from it's latest weights, while the backup folder of the `backup=` key of it's cfg beside the project keeps the backups of the save format, and the charts are drawn beside the project.

## Training the dxxwjz4p project

The training of the dxxwjz4p project (the `--train-cfg cars.dxxwjz4p` command line, the dxxwjz4p project the training GUI opens) trains the way the training of the dxxwjz3p project does (see above), of the project unpacked into the temporary directory: the darknetxx unpacks the project into the `darknetxx-dxxwjz4p-cars-<random>` folder of the `/tmp` (of the `TMPDIR`) as the training starts and reads the JSON document and the images of there, the folder is removed at the end of the training process. The project keeps the images, so it isn't rewritten by every save of the training (the archive of the images of the gigabytes), the files of the training are beside it instead:

| The file | Holds |
| --- | --- |
| `cars.dxxwjz4p` | the project the training reads, which the training doesn't change |
| `cars.dxxwjz4` | the network the training trains: the latest weights, the training record and the description of the project, written by every save of the training (through the `cars.dxxwjz4.saving` file); the training of the project resumes from it once it's there |
| `chart_cars_<YYYY-MM-DD--HH-MM-SS>.png` | the chart of every training start, beside the project whatever the working directory of the training is |
| the backup folder of the `backup=` key of the cfg | the backups of the save format (`--save-format`), the relative one of the working directory |

```
$ darknetxx --train-cfg /srv/projects/cars.dxxwjz4p --dont-show
The /srv/projects/cars.dxxwjz4p project is unpacked into /tmp/darknetxx-dxxwjz4p-cars-5f0c1e93a7d2b4c6
The network of the /srv/projects/cars.dxxwjz4p project is saved into /srv/projects/cars.dxxwjz4, the charts beside it
...
$ ls /srv/projects
cars.dxxwjz4  cars.dxxwjz4p  chart_cars_2026-11-02--10-00-00.png
```

The `cars.dxxwjz4` file is the network of the inference alone (`--weights cars.dxxwjz4`), the converter makes the new project of it's weights and the annotations of the project when needed. The training resumed of the `cars.dxxwjz4` file is the training of the network of it's cfg the project keeps: the network of the project edited since (the layers of the other sizes) doesn't load the weights of the earlier one, the `cars.dxxwjz4` file of them moved aside starts the training from the weights of the project again.

## Using the formats

The C++ code of the project loads and saves the weights of a darknet network with any format through the interfaces of the library and the `NetworkWeights` view of the network:

```
#include "src/NetworksLoaders/DataDriversAliases.h"
#include "src/NetworksLoaders/network-weights/NetworkWeights.h"

using darknet_adaptor::loaders::NetworkWeights;

// net is the darknet network of an ANetworkAdaptor (the networkptr it's get()
// returns), the rest keeps the network description and the training record
dxxdd::WeightsDocument rest;
rest.network = dxxdd::NetworkDescription{cfg_text, {"cat", "dog"}};

const dxxdd::IWeightsFileFormatPtr format =
    dxxdd::WeightsFileFormats::named("dxxwjz3");

format->create_writer()->save(NetworkWeights{*net}, "backup.dxxwjz3", net->n,
                              false, rest);
dxxdd::WeightsFileFormats::of_file("backup.dxxwjz3")
    ->create_reader()
    ->load(NetworkWeights{*net}, "backup.dxxwjz3", net->n, rest);

// the whole file read into the document, validated against the network
dxxdd::WeightsDocument document;
const NetworkWeights view{*net};

dxxdd::WeightsFileFormats::of_file("backup.dxxwjz3")
    ->create_reader()
    ->read("backup.dxxwjz3", &view, document);
```
