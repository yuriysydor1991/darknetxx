## Вмикання бібліотеки darknetxx-datadrivers

Файли ваг нейромереж і проекти навчання - оригінальні файли ваг darknet, файли dxxwjz1, dxxwjz2, dxxwjz2p, dxxwjz3, dxxwjz3p, dxxwjz4 і dxxwjz4p, а також теки навчання YOLO4 для конвертування - читає, записує, перевіряє і конвертує окрема бібліотека [darknetxx-datadrivers](https://github.com/yuriysydor1991/darknetxx-datadrivers). Їхні документи, класи читання, запису і перевірки, конвертер між ними, секції файлу cfg і контролер стиснення zlib раніше були частиною цього репозиторію (директорій `src/NetworksLoaders` і `src/zlib`) і були винесені в цю бібліотеку, створену з гілки `applib` проекту [cpp-app-template](https://github.com/yuriysydor1991/cpp-app-template), тож darknetxx і будь-який інший інструмент спільно використовують одну й ту саму реалізацію форматів. Разом з ними туди переїхав і бінарний файл `darknetxxConverter` (див. секцію [Бінарний файл darknetxxConverter](/doc/sections/uk_UA/5-35-the-darknetxxConverter-binary.md)), тоді як головний бінарний файл `darknetxx` зберігає свій командний рядок конвертування (див. секцію [Конвертування файлів ваг нейромереж](/doc/sections/uk_UA/6-2-4-converting-the-network-weights-files.md)).

Тому бібліотека **обов'язкова**: опція CMake `ENABLE_DARKNETXX_DATA_DRIVERS` за замовчуванням має значення `ON`, і з вимкненою опцією проект не будується - конфігурація лише попереджає про опцію зі значенням `OFF`, але вихідні файли, написані з використанням бібліотеки, без неї не компілюються.

```
# в середині кореневої директорії проекту

cmake -S . -B build -DENABLE_DARKNETXX_DATA_DRIVERS=ON
```

Модуль [cmake/enablers/darknetxx-DarknetxxDataDrivers-enabler.cmake](/cmake/enablers/darknetxx-DarknetxxDataDrivers-enabler.cmake) шукає бібліотеку функцією `find_package()` за її конфігурацією пакета CMake і, якщо бібліотеку не знайдено, зупиняє конфігурацію з помилкою, яка пояснює, як її надати. Завантаження через Інтернет не передбачено: бібліотека використовується встановленою, так само як проект [ImagesAnnotator](https://github.com/yuriysydor1991/ImagesAnnotator) використовує власні бібліотеки даних, а її заголовні файли підключаються через версіоновану директорію `<DarknetxxDataDrivers-0.12/...>`, яку надає лише встановлення. Модуль приєднує бібліотеку до кожної цілі проекту (функцією `link_libraries()`), бо з нею компілюються об'єктні бібліотеки всієї директорії [`src`](/src) і виконувані файли тестів, тож модуль підключається ще до того, як додаються вихідні файли.

### Побудова і встановлення бібліотеки

Бібліотека не є пакетом дистрибутива, тож її будують з вихідних файлів і встановлюють ще до конфігурації проекту. Скрипт [misc/ci/install-darknetxx-datadrivers.sh](/misc/ci/install-darknetxx-datadrivers.sh) клонує, будує (побудова `Release`) і встановлює її однією командою, а його єдиним аргументом є префікс встановлення, за замовчуванням `/usr/local`:

```
# в середині кореневої директорії проекту

# у префікс /usr/local, пароль sudo запитується для кроку встановлення
misc/ci/install-darknetxx-datadrivers.sh

# у власний префікс
misc/ci/install-darknetxx-datadrivers.sh /opt/darknetxx
```

Змінні середовища скрипта `DXXDD_GIT` і `DXXDD_GIT_TAG` задають інший репозиторій та іншу гілку чи тег у ньому (за замовчуванням гілку `main` репозиторію [darknetxx-datadrivers](https://github.com/yuriysydor1991/darknetxx-datadrivers)) - саме так конвеєр перевіряє ще не випущену ревізію бібліотеки:

```
# в середині кореневої директорії проекту

DXXDD_GIT_TAG=my-branch misc/ci/install-darknetxx-datadrivers.sh /opt/darknetxx
```

Ті самі кроки вручну:

```
git clone https://github.com/yuriysydor1991/darknetxx-datadrivers.git
cmake -S darknetxx-datadrivers -B build-dxxdd -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/darknetxx
cmake --build build-dxxdd -j$(nproc)
cmake --install build-dxxdd
```

Бібліотеку, встановлену поза типовими префіксами пошуку, знаходять за допомогою звичної змінної CMake, а для встановлених бінарних файлів проекту такий префікс стає повним, щойно про нього дізнається завантажувач, наприклад, через `LD_LIBRARY_PATH` цього префікса (скрипт запускає `ldconfig` для префіксів `/usr` і `/usr/local` і нагадує про `LD_LIBRARY_PATH` для решти):

```
# в середині кореневої директорії проекту

cmake -S . -B build -DCMAKE_PREFIX_PATH=/opt/darknetxx
cmake --build build

# встановлені бінарні файли проекту під час виконання
LD_LIBRARY_PATH=/opt/darknetxx/lib darknetxx --version
```

Сама бібліотека будується з бібліотеками zlib і nlohmann JSON (системними або тими, які її конфігурація завантажує через наявне з'єднання) і, необов'язково, з бібліотекою OpenCV, яка читає розміри зображень тек YOLO4, яких не може прочитати заголовний файл `stb_image` бібліотеки (див. секцію [Обов'язкові інструменти для ОС на базі GNU/Лінукс](/doc/sections/uk_UA/3-1-required-tools-for-the-GNU-Linux-based-OS.md)). Вона встановлює:

| Встановлений файл | Містить |
| --- | --- |
| `<prefix>/lib/libDarknetxxDataDrivers-0.12.so.0.12.0` з посиланнями soname `.so.0` і namelink `.so` | спільну бібліотеку |
| `<prefix>/include/DarknetxxDataDrivers-0.12/*.h` | публічні заголовні файли простору імен `DarknetxxDataDrivers012` |
| `<prefix>/include/DarknetxxDataDrivers-0.12/darknet/*.h` | C типи нейромережі та шарів darknet, які включають джерельні коди darknet проекту |
| `<prefix>/lib/cmake/DarknetxxDataDrivers-0.12/*.cmake` | пакет CMake цілі `DarknetxxDataDrivers-0.12::DarknetxxDataDrivers-0.12` |
| `<prefix>/bin/darknetxxConverter` | бінарний файл конвертера файлів ваг нейромереж |

Директорія `lib` - це директорія `CMAKE_INSTALL_LIBDIR`, наприклад, `lib/x86_64-linux-gnu` для префікса `/usr` у Debian і Ubuntu. Бібліотека документує свої формати, свій API і власні опції побудови у своєму [репозиторії](https://github.com/yuriysydor1991/darknetxx-datadrivers).

### Інший репозиторій і тег

Ця побудова бібліотеку не завантажує, але маніфести пакувальників і образи Docker - flatpak, snap, docker і конвеєра Jenkins - будують її у власному ізольованому середовищі й беруть репозиторій з цих двох змінних кешу (див. секції [Вмикання підтримки генерації flatpak пакету](/doc/sections/uk_UA/5-11-enabling-the-flatpak-package-generation-support.md), [Вмикання пакувальника snap](/doc/sections/uk_UA/5-19-enabling-the-snap-package-manager.md) і [Вмикання підтримки запуску Docker-контейнера](/doc/sections/uk_UA/5-12-enabling-the-docker-container-build-and-run.md)):

```
cmake -S . -B build \
  -DDARKNETXX_DXXDD_GIT=https://github.com/yuriysydor1991/darknetxx-datadrivers.git \
  -DDARKNETXX_DXXDD_GIT_TAG=main
```

### Інша назва пакета

Бібліотека містить свою мажорну і мінорну версію у назвах встановленого пакета, цілі та бібліотеки - `DarknetxxDataDrivers-0.12`, відповідно до її публічного простору імен з версією `DarknetxxDataDrivers012`, - тож дві мінорні версії встановлюються повністю одна поряд з одною. Її власні опції `LIB_INCLUDE_MINOR_IN_NAME` (за замовчуванням `ON`), `LIB_INCLUDE_MICRO_IN_NAME` і `LIB_NAME_SUFFIX` скорочують або подовжують цю назву. Назву, яку конфігурація шукає функцією `find_package()`, задає змінна кешу `DARKNETXX_DXXDD_PACKAGE`:

```
cmake -S . -B build -DDARKNETXX_DXXDD_PACKAGE=DarknetxxDataDrivers-0.12
```

Вихідні файли підключають заголовні файли лише через директорію `<DarknetxxDataDrivers-0.12/...>` (див. нижче), тож пакет з іншою назвою теж має надавати саме цю директорію заголовних файлів.

| Опція CMake | За замовчуванням | Дія |
| --- | --- | --- |
| `ENABLE_DARKNETXX_DATA_DRIVERS` | `ON` | Шукає бібліотеку. `OFF` лише попереджає: без неї проект не компілюється. |
| `DARKNETXX_DXXDD_PACKAGE` | `DarknetxxDataDrivers-0.12` | Назва пакета (і цілі) CMake бібліотеки, яку треба шукати. |
| `DARKNETXX_DXXDD_GIT` | `https://github.com/yuriysydor1991/darknetxx-datadrivers.git` | Репозиторій, з якого маніфести пакувальників і образи Docker будують бібліотеку. |
| `DARKNETXX_DXXDD_GIT_TAG` | `main` | Гілка або тег цього репозиторію. |

### Що проект бере з бібліотеки

- документи ваг: `WeightsDocument` з `WeightsHeader` і шарами масивів, `NetworkDescription`, `TrainingRecord`, `Yolo4Annotations` з `Yolo4Image` і `Yolo4Box`;
- формати: реєстр `WeightsFileFormats` форматів `IWeightsFileFormat` з їхніми класами читання `IWeightsReader` і запису `IWeightsWriter` за назвами і розширеннями назв файлів, `NetworkSource` файлів, з яких будуються нейромережі, читання файлів cfg і назв класів `NetworkFiles` і `yolo4::Yolo4FolderLayout` тек YOLO4;
- секції файлу cfg: `CfgSection`, за якою завантажувач нейромереж розбирає cfg, і `CfgError`, яку вона кидає;
- `WeightsConverter` конвертування `WeightsConversion` та інтерфейс `INetworkWeights` нейромереж, у які завантажуються і з яких зберігаються файли, разом з `LayerType`, `WeightsSchema` і `WeightsLayout` їхніх масивів;
- `DxxDataDriversFacade::init_logger`, якому `darknetxx012::LibraryFacade::init_logger` бібліотеки darknetxx передає журнал програми, тож повідомлення форматів потрапляють у журнал програми.

Єдине місце, яке називає бібліотеку, - заголовний файл [src/NetworksLoaders/DataDriversAliases.h](/src/NetworksLoaders/DataDriversAliases.h), який підключає заголовний файл `<DarknetxxDataDrivers-0.12/DxxDataDriversFacade.h>` (той підключає всі інші публічні заголовні файли бібліотеки) і оголошує псевдонім `dxxdd` версіонованого простору імен `DarknetxxDataDrivers012`: префікс фіксує версію бібліотеки, з якою працює кожна одиниця трансляції, а загальні назви на кшталт `CfgSection.h` і `WeightsDocument.h` лежать поряд у корені підключень без префікса. Нейромережі darknet для форматів залишаються в цьому проекті, див. секцію [Файли ваг нейромереж](/doc/sections/uk_UA/4-7-4-network-weights-files.md).
