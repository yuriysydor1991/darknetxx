## Бінарний файл darknetxxConverter

Бінарний файл `darknetxxConverter` - конвертер файлів ваг нейромереж між
оригінальним форматом ваг darknet, форматами dxxwjz1, dxxwjz2, dxxwjz2p,
dxxwjz3, dxxwjz3p, dxxwjz4 і dxxwjz4p, а також тек навчання YOLO4 у проекти
dxxwjz2p, dxxwjz3p і dxxwjz4p і назад, який друкує cfg нейромережі YOLO4, що
його зберігають файли dxxwjz2, dxxwjz2p, dxxwjz3, dxxwjz3p, dxxwjz4 і
dxxwjz4p, і більше нічого не робить, - **більше не є бінарним файлом цього
проекту**: він переїхав у бібліотеку
[darknetxx-datadrivers](https://github.com/yuriysydor1991/darknetxx-datadrivers)
разом з форматами, які він конвертує (див. секцію
[Вмикання бібліотеки darknetxx-datadrivers](/doc/sections/uk_UA/5-39-enabling-the-darknetxx-datadrivers-library.md)),
і вона будує і встановлює його поряд із собою. Він зберіг назву і командний
рядок, які мав тут, - ті самі прапорці `--convert-*` і `--dump-cfg`, ті самі
формати, ту саму поведінку, - тож скрипти і конвеєри підготовки даних, які
його викликають, і далі працюють, а **до нього не скомпільовано ані
навчання, ані розпізнавання**, як і жодного коду darknet.

Головний бінарний файл `darknetxx` цього проекту зберігає той самий
командний рядок конвертування (див. секцію
[Конвертування файлів ваг нейромереж](/doc/sections/uk_UA/6-2-4-converting-the-network-weights-files.md)):
побудова цього проекту конвертує файли зовсім без бінарного файлу
`darknetxxConverter`, а довідкове повідомлення `darknetxx` перелічує
параметри конвертування поряд з параметрами навчання і розпізнавання.

### Як його отримати

Бінарний файл постачається з бібліотекою: скрипт
[misc/ci/install-darknetxx-datadrivers.sh](/misc/ci/install-darknetxx-datadrivers.sh)
або побудова CMake самої бібліотеки встановлює його у директорію `bin`
префікса встановлення, де він сам знаходить бібліотеку свого префікса:

```
# в середині кореневої директорії проекту

misc/ci/install-darknetxx-datadrivers.sh /opt/darknetxx

/opt/darknetxx/bin/darknetxxConverter --help
```

Пакет snap цього проекту містить його як застосунок `darknetxxConverter`
пакета snap (див. секцію
[Вмикання пакувальника snap](/doc/sections/uk_UA/5-19-enabling-the-snap-package-manager.md)),
образи Docker проекту встановлюють його разом з бібліотекою у свій префікс
`/usr` (див. секцію
[Вмикання підтримки запуску Docker-контейнера](/doc/sections/uk_UA/5-12-enabling-the-docker-container-build-and-run.md)),
тоді як пакет DEB цього проекту пакує лише спільну бібліотеку (див. секцію
[Вмикання підтримки генерування DEB-пакетів з cpack](/doc/sections/uk_UA/5-10-enabling-DEB-package-generation-with-cpack.md)):
бінарний файл постачається з пакетами самої бібліотеки.

### Використання

Бінарний файл приймає ті самі параметри конвертування, що й `darknetxx`, тож
секція [Конвертування файлів ваг нейромереж](/doc/sections/uk_UA/6-2-4-converting-the-network-weights-files.md)
повністю описує і його:

```
darknetxxConverter \
  --convert-from weights \
  --convert-src ./yolov3.weights \
  --convert-to dxxwjz2 \
  --convert-dst ./yolov3.dxxwjz2 \
  --convert-cfg ./yolov3.cfg \
  --convert-names ./coco.names

# нейромережа, яку треба навчити, з файлу cfg: файл dxxwjz3 без ваг
darknetxxConverter \
  --convert-to dxxwjz3 \
  --convert-dst ./to_train.dxxwjz3 \
  --convert-cfg ./yolov3.cfg \
  --convert-names ./coco.names

# тека YOLO4 у проект dxxwjz2p з її зображеннями
darknetxxConverter \
  --convert-from yolo4 \
  --convert-src ./cars \
  --convert-to dxxwjz2p \
  --convert-dst ./cars.dxxwjz2p

# cfg нейромережі, який зберігає файл dxxwjz3, у файл cfg
darknetxxConverter --dump-cfg ./yolov3.dxxwjz3 > ./yolov3.cfg
```

Параметри `--help` і `--version` називають цей бінарний файл і версію
бібліотеки, а довідка перелічує лише загальні параметри і параметри
конвертування. Якщо командний рядок не запитує ані конвертування, ані друку
cfg, запускати нічого (за цим бінарним файлом немає ані навчання, ані
розпізнавання), тож у відповідь друкується довідкове повідомлення і
повертається ненульовий статус завершення.

### Чим відрізняються два конвертери

Оригінальний файл ваг darknet не має власної структури - це масиви float
шарів один за одним, - тож обидва конвертери читають його за нейромережею
файлу cfg (параметра `--convert-cfg`), але створюють цю нейромережу
по-різному:

- бінарний файл `darknetxx` будує справжню нейромережу darknet з cfg, ту
  саму, яку він навчає і з якою розпізнає (`darknet_adaptor::loaders::NetworkWeightsConverter`
  директорії [`src/NetworksLoaders/network-weights`](/src/NetworksLoaders/network-weights));
- бінарний файл `darknetxxConverter` бібліотеки, у якому немає коду darknet,
  створює структуру нейромережі, яку описують секції cfg (`CfgNetworkStructure`
  бібліотеки): шари, розміри їхніх входів і виходів і масиви float їхніх ваг,
  без пам'яті для них.

Компонентний тест `CTEST_NetworkWeights` цього проекту перевіряє, що обидві
дають розкладку тих самих масивів для файлів cfg, які містить репозиторій
(файлів директорії [misc/examples/cfg](/misc/examples/cfg) і тестів
завантажувача нейромереж, для кожного, з якого darknet будує нейромережу),
тож обидва бінарні файли читають і записують файли однаково, байт у байт
(див. секцію
[Файли ваг нейромереж](/doc/sections/uk_UA/4-7-4-network-weights-files.md#конвертування)).
Тести самого бінарного файлу `darknetxxConverter` - це тести бібліотеки, а
інтеграційні тести `ITEST_converter` і `ITEST_converter_bash` цього проекту
запускають командний рядок конвертування бінарного файлу `darknetxx` (див.
секцію
[Вмикання інтеграційних тестів](/doc/sections/uk_UA/5-3-3-enabling-integration-testing.md)).

Опція `ENABLE_DARKNETXX_CONVERTER` і змінна кешу
`DARKNETXX_CONVERTER_BINARY_NAME`, які будували цей бінарний файл у цьому
проекті, зникли разом з ним.
