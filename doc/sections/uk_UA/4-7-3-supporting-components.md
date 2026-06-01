# Допоміжні компоненти

Допоміжні компоненти - це менші наскрізні частини, що використовуються рештою
програми: контролер OpenMP, підсистема журналювання і допоміжні утиліти.

Окремий вихідний файл діаграми нижче -
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

## Призначення кожного компонента

- **`ompi::OMPController`** ([`OMPController.h`](/src/OMP/OMPController.h)) - налаштовує середовище виконання OpenMP. Він читає бажану кількість робочих потоків з `app::ApplicationContext` (прапорець `--threads`) і відповідно налаштовує максимальну кількість потоків OpenMP. Створюється через статичний фабричний метод `create()`.
- **Підсистема журналювання** ([`src/log`](/src/log)) - надає макроси `LOGE`, `LOGW`, `LOGI`, `LOGD` і `LOGT`, визначені у [`log.h`](/src/log/log.h). Макроси передають повідомлення разом з іменем файлу і номером рядка до реалізації журналу. Типова `default_logger::DefaultLogger` ([`DefaultLogger.h`](/src/log/default-logger/DefaultLogger.h)) записує повідомлення у налаштований файл журналу і/або у стандартні потоки; також доступний альтернативний бекенд `cpplog4c`.
- **`helpers::StringTools`** ([`StringTools.h`](/src/helpers/StringTools.h)) - невеликі утиліти для роботи з рядками, спільні для всього проекту.

Підсистема журналювання ініціалізується рано об'єктом `app::ApplicationFactory`,
який використовує `CommandLineParser::get_custom_logfile` для врахування власного
шляху до файлу журналу ще до розбору решти командного рядка.
