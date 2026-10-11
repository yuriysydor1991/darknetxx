## Бінарний файл darknetxxConverter

Окрім програми `darknetxx`, проект будує другий бінарний файл,
`darknetxxConverter`, який конвертує файли ваг нейромереж між оригінальним
форматом ваг darknet, форматами dxxwjz1, dxxwjz2, dxxwjz2p, dxxwjz3 і dxxwjz3p,
а також теки навчання YOLO4 у проекти dxxwjz2p і dxxwjz3p і назад, друкує cfg
нейромережі YOLO4, який зберігають файли dxxwjz2, dxxwjz2p, dxxwjz3 і dxxwjz3p,
і більше нічого не робить. Це те саме
конвертування, яке головний бінарний файл пропонує через власний
[командний рядок](/doc/sections/uk_UA/6-2-4-converting-the-network-weights-files.md),
з тими самими прапорцями, форматами і поведінкою, бо й код, що за ними стоїть,
той самий. Другий бінарний файл додає не нову можливість, а ще один варіант
розгортання: **до нього не скомпільовано ані навчання, ані розпізнавання**,
тож скрипт чи конвеєр підготовки даних, якому потрібно лише перетворювати
файли ваг з одного формату в інший, запускає бінарний файл, у якому більше
нічого немає.

### Використання

Бінарний файл приймає ті самі параметри конвертування, що й головний, тож
секція [Конвертування файлів ваг нейромереж](/doc/sections/uk_UA/6-2-4-converting-the-network-weights-files.md)
повністю описує і його:

```
# з директорії побудови

./src/darknetxxConverter \
  --convert-from weights \
  --convert-src ./yolov3.weights \
  --convert-to dxxwjz2 \
  --convert-dst ./yolov3.dxxwjz2 \
  --convert-cfg ./yolov3.cfg \
  --convert-names ./coco.names

# нейромережа, яку треба навчити, з файлу cfg: файл dxxwjz2 без ваг
./src/darknetxxConverter \
  --convert-to dxxwjz2 \
  --convert-dst ./to_train.dxxwjz2 \
  --convert-cfg ./yolov3.cfg \
  --convert-names ./coco.names

# тека YOLO4 у проект dxxwjz2p з її зображеннями
./src/darknetxxConverter \
  --convert-from yolo4 \
  --convert-src ./cars \
  --convert-to dxxwjz2p \
  --convert-dst ./cars.dxxwjz2p

# cfg нейромережі, який зберігає файл dxxwjz3, у файл cfg
./src/darknetxxConverter --dump-cfg ./yolov3.dxxwjz3 > ./yolov3.cfg
```

Параметри `--help` і `--version` працюють так само, як і в головному бінарному
файлі, але називають саме цей файл, а довідка перелічує лише параметри
конвертування. Якщо командний рядок не запитує ані конвертування, ані друку
cfg, запускати нічого (за цим бінарним файлом немає ані навчання, ані
розпізнавання), тож у відповідь друкується довідкове повідомлення і
повертається ненульовий статус завершення.

### Побудова

Обидва бінарні файли будуються за замовчуванням, тож звичайна побудова проекту
створює `darknetxx` і `darknetxxConverter` поруч, а крок встановлення
встановлює обидва.

| Опція CMake | За замовчуванням | Дія |
| --- | --- | --- |
| `ENABLE_DARKNETXX_CONVERTER` | `ON` | Будує бінарний файл `darknetxxConverter`. Щоб побудувати лише головну програму, встанови значення `OFF`. |
| `DARKNETXX_CONVERTER_BINARY_NAME` | `<project binary name>Converter` | Назва другого бінарного файлу і його цілі CMake. |

```
# в середині кореневої директорії проекту

cmake -B build -S . -DENABLE_DARKNETXX_CONVERTER=OFF
```

Файли dxxwjz1, dxxwjz2, dxxwjz2p, dxxwjz3 і dxxwjz3p є стисненими gzip
документами JSON, тож
обидва бінарні файли
потребують бібліотек [zlib](/doc/sections/uk_UA/5-23-enabling-the-zlib-library.md)
і [nlohmann JSON](/doc/sections/uk_UA/5-18-enabling-the-nlohmann-json-library.md):
системних або тих, які завантажуються під час конфігурації.

### Як бінарні файли спільно використовують код

Другий бінарний файл не дублює жодного коду. Обидва бінарні файли мають
спільну точку входу `src/main.cpp`, компонент `src/app` передає свої вихідні
файли їм обом (див. [src/app/CMakeLists.txt](/src/app/CMakeLists.txt)) і обидва
лінкуються з бібліотекою darknetxx (див. секцію
[Бібліотека darknetxx](/doc/sections/uk_UA/4-7-5-the-darknetxx-library.md)),
за допомогою `darknetxx011::LibraryFacade` якої `app::Application` виконує
конвертування так само, як навчання і розпізнавання головного бінарного файлу.

[src/darknetxxConverter-target.cmake](/src/darknetxxConverter-target.cmake)
компілює ціль конвертера з визначенням препроцесора `DARKNETXX_CONVERTER_ONLY`,
яке повідомляє `ApplicationFactory`, що типової програми немає, довідковому
повідомленню - перелічувати лише параметри конвертування, а довідковому
повідомленню і повідомленню про версію - назву запущеного бінарного файлу
(див. [src/app/ApplicationBinaryName.h](/src/app/ApplicationBinaryName.h)).
Більше ніщо у проекті не розгалужується за цим визначенням.

Саме конвертування виконує `WeightsConverter` інфраструктури нейромереж (див.
секцію [Файли ваг нейромереж](/doc/sections/uk_UA/4-7-4-network-weights-files.md)),
якому `darknetxx_impl::WeightsConversionLib` бібліотеки передає параметри
командного рядка контексту і запит зупинки програми. Бінарний файл покривають інтеграційні тести
`ITEST_converter` і `ITEST_converter_bash` (див. секцію
[Вмикання інтеграційних тестів](/doc/sections/uk_UA/5-3-3-enabling-integration-testing.md)).
