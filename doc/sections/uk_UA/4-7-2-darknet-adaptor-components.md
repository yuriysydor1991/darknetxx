# Компоненти адаптера Darknet

Компоненти адаптера Darknet розміщені у просторі імен `darknet_adaptor` (під
[`src/darknet-adaptor`](/src/darknet-adaptor)). Вони утворюють місток між чистою
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

## Призначення кожного класу

- **`DarknetController`** ([`DarknetController.h`](/src/darknet-adaptor/DarknetController.h)) - точка входу адаптера. Маючи `app::ApplicationContext`, він будує `DarknetContext`, обирає відповідного робітника і викликає його метод `perform`. Створюється через статичний фабричний метод `create()`.
- **`DarknetContext`** ([`DarknetContext.h`](/src/darknet-adaptor/DarknetContext.h)) - контекст даних адаптера. Він перетворює поля контексту програми у значення, очікувані оригінальними процедурами Darknet, і володіє внутрішнім `detector_context`. Він надає типізовані гетери і сетери для окремих параметрів Darknet.
- **`IDarknetAdaptor`** ([`IDarknetAdaptor.h`](/src/darknet-adaptor/adaptors/IDarknetAdaptor.h)) - абстрактний інтерфейс конкретного робітника Darknet. Він оголошує єдиний метод `perform(dctx)`, тому нових робітників можна додавати без зміни логіки контролера.
- **`ADetector`** / **`ADetectorTrainer`** - конкретні робітники, які виконують розпізнавання і навчання над підготованим `DarknetContext`.
- **`ALayerAdaptor`** / **`ANetworkAdaptor`** - адаптери, які відображають оригінальні структури шару і мережі Darknet у код на стороні C++ проекту.
- **`StructAdaptorHelper`** ([`StructAdaptorHelper.h`](/src/darknet-adaptor/adaptors/StructAdaptorHelper.h)) - помічник, який перетворює оригінальні структури Darknet на C у структури проекту і навпаки.

Вибір конкретного робітника тримає оригінальні точки входу Darknet ізольованими
за інтерфейсом `IDarknetAdaptor`, що й уможливлює поступове портування
оригінального коду.
