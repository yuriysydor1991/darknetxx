## Використання бібліотеки darknetxx

Проект будує спільну бібліотеку `darknetxx-0.12` усієї своєї функціональності
(див. секцію [Бібліотека darknetxx](/doc/sections/uk_UA/4-7-5-the-darknetxx-library.md))
і виконувані файли над нею. Інший проект CMake використовує бібліотеку,
доступну через FetchContent чи встановлену і знайдену командою
`find_package`.

В обох випадках бібліотека будується зі встановленою бібліотекою
[darknetxx-datadrivers](https://github.com/yuriysydor1991/darknetxx-datadrivers)
форматів файлів ваг нейромереж, яку ніщо не завантажує: спершу встанови її і
вкажи конфігурації її префікс змінною `CMAKE_PREFIX_PATH` (див. секцію
[Вмикання бібліотеки darknetxx-datadrivers](/doc/sections/uk_UA/5-39-enabling-the-darknetxx-datadrivers-library.md)).
Бібліотека `libdarknetxx-0.12.so` під час виконання завантажує бібліотеку
`libDarknetxxDataDrivers-0.12.so.0`, тож завантажувач має знаходити обидві,
тоді як жоден публічний заголовний файл бібліотеки darknetxx не називає
жодного типу бібліотеки darknetxx-datadrivers, тож програма бібліотеки
darknetxx не підключає жодного її заголовного файлу.

### Параметри

| Параметр CMake | За замовчуванням | Дія |
| --- | --- | --- |
| `ENABLE_DARKNETXX_EXECUTABLES` | `ON` для проекту верхнього рівня, `OFF` для проекту іншого | Будує і встановлює виконуваний файл `darknetxx` і виконувані файли GUI (`darknetxx-train-gui`, `darknetxx-recognition-gui` і файли редактора нейромереж). `OFF` будує і встановлює лише бібліотеку. |
| `DARKNETXX_LIB_INSTALL_DEV_FILES` | `ON` | Встановлює публічні заголовки, файли пакету CMake і посилання `libdarknetxx-0.12.so` разом з бібліотекою. `OFF` встановлює лише бібліотеку виконання. |
| `DARKNETXX_LIBRARY_NAME` | `darknetxx-<основна>.<додаткова>` | Назва бібліотеки, її цілі, директорії заголовків і пакету CMake. |

Лише бібліотека з файлами розробки:

```
# у кореневій директорії проекту

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DENABLE_DARKNETXX_EXECUTABLES=OFF
cmake --build build
cmake --install build --prefix /usr/local
```

встановлює

| Встановлений файл | `DARKNETXX_LIB_INSTALL_DEV_FILES=ON` | `OFF` |
| --- | --- | --- |
| `<префікс>/lib/libdarknetxx-0.12.so.0.12.0` і посилання soname `.so.0.12` | так | так |
| посилання `<префікс>/lib/libdarknetxx-0.12.so` | так | ні |
| публічні заголовки `<префікс>/include/darknetxx-0.12/*.h` | так | ні |
| файли пакету `<префікс>/lib/cmake/darknetxx-0.12/*.cmake` | так | ні |
| дані darknet `<префікс>/share/darknetxx/<версія>` (шрифт міток, списки назв) | так | так |

Директорія `lib` - це `CMAKE_INSTALL_LIBDIR`, `lib/x86_64-linux-gnu` для
префікса `/usr` Debian і Ubuntu, наприклад.

### FetchContent

Проект іншого проекту будує лише бібліотеку і жодних тестів, інструментів і
пакетів цього, хай що кажуть його однойменні параметри (див. кореневий
[CMakeLists.txt](/CMakeLists.txt)). Модулі CMake, змінні кешу, функції і цілі
проекту названі за ним, тож вони не змішуються з такими ж іншого проекту,
похідного від cpp-app-template.

```cmake
include(FetchContent)

FetchContent_Declare(
  darknetxx
  GIT_REPOSITORY https://github.com/yuriysydor1991/darknetxx.git
  GIT_TAG master
)
FetchContent_MakeAvailable(darknetxx)

add_executable(my-trainer main.cpp)
target_link_libraries(my-trainer darknetxx-0.12::darknetxx-0.12)
```

```
# конфігурація проекту з FetchContent вище

cmake -S . -B build -DCMAKE_PREFIX_PATH=/opt/darknetxx
```

### find_package

```cmake
find_package(darknetxx-0.12 REQUIRED CONFIG)

add_executable(my-trainer main.cpp)
target_link_libraries(my-trainer darknetxx-0.12::darknetxx-0.12)
```

### Код

Програма звертається до бібліотеки через
[`LibraryFacade`](/src/lib/facade/public/LibraryFacade.h) і включає заголовки
за назвою з префіксом бібліотеки, яку не затінює інша бібліотека з такими ж
назвами:

```cpp
#include <darknetxx-0.12/LibraryFacade.h>

int main()
{
  auto ctx = darknetxx012::LibraryFacade::create_library_context();

  ctx->convert_from = "weights";
  ctx->convert_src = "yolov4.weights";
  ctx->convert_cfg = "yolov4.cfg";
  ctx->convert_to = "dxxwjz1";
  ctx->convert_dst = "yolov4.dxxwjz1";

  auto lib = darknetxx012::LibraryFacade::create_library(ctx);

  return lib != nullptr && lib->libcall(ctx) ? 0 : 1;
}
```

Контекст з `train_cfg` того самого виклику `create_library` навчає
нейромережу, а з `detect_cfg` - розпізнає, тоді як
`LibraryFacade::create_trainer` навчає у фоні проект навчання, який
`LibraryFacade::open_training_project` відкриває за шляхом (теку YOLO4 чи файл
проекту dxxwjz2p, dxxwjz3p або dxxwjz4p), і повідомляє про ітерації `training_listener` контексту, а
`LibraryFacade::training_chart`
повертає зображення PNG графіка навчання файлу ваг dxxwjz2, dxxwjz3 чи dxxwjz4 (порожнє для файлу
без запису навчання), а `LibraryFacade::network_cfg` - текст cfg darknet
нейромережі, який зберігає файл dxxwjz2, dxxwjz2p, dxxwjz3, dxxwjz3p, dxxwjz4 чи dxxwjz4p (порожній
для решти), `LibraryFacade::network_description` - текст cfg разом з назвами
класів, а `LibraryFacade::save_network` зберігає змінені текст cfg і назви
класів у такий файл (ним зберігають бінарні файли редактора нейромереж).
Програма з власним журналом передає
його функцією `LibraryFacade::init_logger` перед будь-яким іншим викликом,
інакше бібліотека веде журнал у файл `darknetxx-0.12.0.log` робочої
директорії і в консоль.
