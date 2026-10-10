# Компоненти адаптера Darknet

Компоненти адаптера Darknet розміщені у просторі імен `darknet_adaptor` (під
[`src/lib/darknet-adaptor`](/src/lib/darknet-adaptor)). Вони утворюють місток між чистою
інфраструктурою C++ програми і оригінальним кодом Darknet на C/C++, тому решта
програми не залежить безпосередньо від оригінальних структур.

Окремий вихідний файл діаграми нижче -
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

## Призначення кожного класу

- **`DarknetController`** ([`DarknetController.h`](/src/lib/darknet-adaptor/DarknetController.h)) - точка входу адаптера. Маючи `app::ApplicationContext`, він будує `DarknetContext`, обирає відповідного робітника і викликає його метод `perform`. Створюється через статичний фабричний метод `create()`.
- **`DarknetContext`** ([`DarknetContext.h`](/src/lib/darknet-adaptor/DarknetContext.h)) - контекст даних адаптера. Він перетворює поля контексту програми у значення, очікувані оригінальними процедурами Darknet, і володіє внутрішнім `detector_context`. Він надає типізовані гетери і сетери для окремих параметрів Darknet. Його метод `init` також запускає потік-спостерігач зупинки, який перевіряє прапорець `app::ApplicationContext::stop` і переносить його до поля `detector_context::stop`, що його цикли мовою C перевіряють щоітерації, тож комбінація клавіш Ctrl+C припиняє тривале навчання - дивись секцію [Обробка сигналів операційної системи](/doc/sections/uk_UA/4-8-handling-the-OS-signals.md).
- **`IDarknetAdaptor`** ([`IDarknetAdaptor.h`](/src/lib/darknet-adaptor/adaptors/IDarknetAdaptor.h)) - абстрактний інтерфейс конкретного робітника Darknet. Він оголошує єдиний метод `perform(dctx)`, тому нових робітників можна додавати без зміни логіки контролера.
- **`ADetector`** / **`ADetectorTrainer`** - конкретні робітники, які завантажують нейромережі своїм завантажувачем (тим, що повертає метод `create_loader()`, за замовчуванням `OrigDefaultLoader`), виконують розпізнавання і навчання над підготованим `DarknetContext` і звільняють нейромережі після роботи.
- **`INetworkLoader`** ([`INetworkLoader.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/INetworkLoader.h)) - інтерфейс завантажувачів нейромереж: метод `load(dctx)` створює нейромережі, потрібні для роботи контексту, і передає їх контексту.
- **`OrigDefaultLoader`** ([`OrigDefaultLoader.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/OrigDefaultLoader/OrigDefaultLoader.h)) - перенесений на C++ оригінальний аналізатор файлів cfg нейромереж (функції `parse_network_cfg()`, `load_network()` та інші з оригінального `parser.c`). Він створює нейромережі для навчання (по одній на GPU), нейромережу для обчислення mAP, яка обчислює шарами першої нейромережі для навчання, і нейромережу для розпізнавання одного зображення з нормалізацією батчів, об'єднаною з її вагами. Нейромережі будуються з файлів cfg або з файлів dxxwjz2, dxxwjz2p, dxxwjz3 і dxxwjz3p, ваги кожного формату завантажують класи читання їхніх форматів, списки зображень навчання проекту dxxwjz2p чи dxxwjz3p - це списки його анотацій, які `DarknetContext` публікує у `TrainingDataRegistry`, а код darknet мовою C читає їх звідти, назви класів розпізнавання і навчання (його обчислення mAP) потрапляють у `DarknetContext`, а навчання починає свій `TrainingJournal`, через який навчання зберігає файли ваг (див. секцію [Файли ваг нейромереж](/doc/sections/uk_UA/4-7-4-network-weights-files.md)). Помилки аналізу, які в оригіналі завершували процес, кидають виняток `NetworkError`, який метод `load` записує у журнал.
- **`CfgSection`** ([`CfgSection.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/OrigDefaultLoader/CfgSection.h)) - одна секція `[section]` файлу cfg з її параметрами `key=value`, перенесені на C++ оригінальні функції `read_cfg()` і `option_find*()`.
- **`OrigWeightsReader`** ([`OrigWeightsReader.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsReader.h)) / **`OrigWeightsWriter`** ([`OrigWeightsWriter.h`](/src/lib/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsWriter.h)) - читання і запис оригінальних файлів ваг, перенесені на C++ оригінальні функції `load_weights_upto()`, `save_weights_upto()` і процедури окремих шарів, байт у байт сумісні з оригінальними файлами. Вони реалізують інтерфейси `IWeightsReader` і `IWeightsWriter`, які так само реалізують класи читання і запису файлів dxxwjz1; формати файлів ваг, їхні валідатори і конвертер між ними описано у секції [Файли ваг нейромереж](/doc/sections/uk_UA/4-7-4-network-weights-files.md).
- **`ANetworkAdaptor`** ([`ANetworkAdaptor.h`](/src/lib/darknet-adaptor/adaptors/ANetworkAdaptor/ANetworkAdaptor.h)) - власник пам'яті нейромережі: масиву шарів, робочого буфера, лічильників, спільних для всіх копій структури нейромережі тощо. Він реалізує процедури нейромережі, які виділяють пам'ять: `resize` (оригінальна `resize_network()`), `set_batch`, `fuse_conv_batchnorm`, `calculate_binary_weights` і `copy_weights_net`.
- **`ALayerAdaptor`** ([`ALayerAdaptor.h`](/src/lib/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h)) - власник пам'яті шару: кожен буфер є контейнером `std::vector` адаптера, на який вказує поле структури шару (`NULL` для порожнього контейнера). Буфери, які Darknet використовує спільно (вихід попереднього шару для шару `[dropout]`, ваги шару, що ділить їх з іншим шаром тощо), позичаються сетерами `borrow_*`. Нейромережа прив'язує структуру шару до її комірки у масиві шарів, тож код Darknet і адаптер працюють з однією структурою.
- **Класи шарів** ([`src/lib/darknet-adaptor/adaptors/Layers`](/src/lib/darknet-adaptor/adaptors/Layers)) - процедури `make` і `resize` кожного виду шарів (`ConvolutionalLayer`, `YoloLayer` та інші), перенесені на C++ оригінальні функції `make_*_layer()` і `resize_*_layer()`.
- **`NetworksRegistry`** / **NetworksCApi** ([`src/lib/darknet-adaptor/adaptors/NetworksCApi`](/src/lib/darknet-adaptor/adaptors/NetworksCApi)) - функції C API Darknet, які оригінальний код на C продовжує викликати (`parse_network_cfg()`, `load_weights()`, `save_weights()`, `resize_network()`, `set_batch_network()`, `free_network()` та інші), реалізовані на основі адаптерів. Код на C передає структуру нейромережі за значенням, тож реєстр знаходить нейромережу копії структури за її масивом шарів. Помилки завершують процес так само, як це робили оригінальні функції.
- **`StructAdaptorHelper`** ([`StructAdaptorHelper.h`](/src/lib/darknet-adaptor/adaptors/StructAdaptorHelper.h)) - помічник адаптерів, який надає полям структур вказівники на дані контейнерів.

## Пам'ять нейромереж

Оригінальний код Darknet на C у проекті лише обчислює: він ніколи не виділяє,
не перевиділяє і не звільняє пам'ять нейромережі. Нейромережі створює
завантажувач, і вони належать адаптерам C++, а `DarknetContext` передає їхні
структури оригінальному контексту обчислень і забирає їх після роботи. Кожна
копія структури нейромережі на C використовує ті самі буфери, а зміни
скалярних полів, які код на C робить у своїй копії (розмір батчу динамічного
міні-батчу тощо), адаптер переймає під час наступного виклику C API.

Перенесені процедури зміни розміру виправляють помилки роботи з пам'яттю
оригінальних процедур: буфери входу XNOR згорткового шару, розмиваючий підшар
згорткового шару і шару maxpool з антиаліасингом, буфери `x` і `x_norm` шару
`[batchnorm]` і буфер
стовпців шару `[local]` з відступом, більшим за його ядро, мають розмір для
нового входу, а наступний шар отримує нові розміри виходів шарів `[yolo]`,
`[Gaussian_yolo]` і `[region]`.

Вибір конкретного робітника тримає оригінальні точки входу Darknet ізольованими
за інтерфейсом `IDarknetAdaptor`, що й уможливлює поступове портування
оригінального коду.
