# Допоміжні компоненти

Допоміжні компоненти - це менші наскрізні частини, що використовуються рештою
програми: контролер OpenMP, підсистема журналювання, контролер стиснення zlib і
допоміжні утиліти.

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

## Призначення кожного компонента

- **`ompi::OMPController`** ([`OMPController.h`](/src/lib/OMP/OMPController.h)) - налаштовує середовище виконання OpenMP. Він читає бажану кількість робочих потоків з `app::ApplicationContext` (прапорець `--threads`) і відповідно налаштовує максимальну кількість потоків OpenMP. Створюється через статичний фабричний метод `create()`.
- **Підсистема журналювання** ([`src/log`](/src/log)) - надає макроси `LOGE`, `LOGW`, `LOGI`, `LOGD` і `LOGT`, визначені у [`log.h`](/src/log/log.h). Макроси передають повідомлення разом з іменем файлу і номером рядка до реалізації журналу. Типова `default_logger::DefaultLogger` ([`DefaultLogger.h`](/src/log/default-logger/DefaultLogger.h)) записує повідомлення у налаштований файл журналу і/або у стандартні потоки; також доступний альтернативний бекенд `cpplog4c`.
- **`helpers::StringTools`** ([`StringTools.h`](/src/lib/helpers/StringTools.h)) - невеликі утиліти для роботи з рядками, спільні для всього проекту.
- **`zlibi::ZlibController`** ([`ZlibController.h`](/src/lib/zlib/ZlibController.h)) - контролер стиснення zlib з гілки `appZlib` проекту cpp-app-template, адаптований для файлів ваг нейромереж dxxwjz1 (див. секцію [Файли ваг нейромереж](/doc/sections/uk_UA/4-7-4-network-weights-files.md)): стиснення в пам'яті (з обгорткою zlib) і файли gzip для буферів будь-якого розміру попри 32-бітні лічильники zlib, а також файли gzip, які записуються і читаються через стандартні потоки частинами (`GzipOStream` і `GzipIStream` з [`GzipFileStreams.h`](/src/lib/zlib/GzipFileStreams.h)), тож у пам'яті не тримається ані весь файл, ані всі дані. Буфер потоку сам виконує deflate та inflate zlib з обгорткою gzip: він перевіряє весь потік gzip (включно з CRC і розміром із завершального блоку) і завершується помилкою на обрізаному чи пошкодженому файлі, який функція zlib `gzread()` приймає за повний, якщо дані закінчуються на межі її читання.

Підсистема журналювання ініціалізується рано об'єктом `app::ApplicationFactory`,
який використовує `CommandLineParser::get_custom_logfile` для врахування власного
шляху до файлу журналу ще до розбору решти командного рядка.
