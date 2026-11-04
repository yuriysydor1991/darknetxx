## Вмикання пакувальника snap

Для того щоб увімкнути для побудови ціль `snap`, що призначена для генерування [SNAP](https://snapcraft.io/docs/) бінарного дистрибуційного пакунку проекту, необхідно сконфігурувати CMake проект з увімкненою змінною `ENABLE_SNAP`:

```
cmake -S . -B build -DENABLE_SNAP=ON
```

Детальніше у файлі [cmake/enablers/packagers/darknetxx-snap-enabler.cmake](/cmake/enablers/packagers/darknetxx-snap-enabler.cmake) про ціль `snap`.

Рецепт [misc/packagers/snapcraft.yaml.in](/misc/packagers/snapcraft.yaml.in) спершу будує бібліотеку [darknetxx-datadrivers](https://github.com/yuriysydor1991/darknetxx-datadrivers) форматів файлів ваг нейромереж (див. секцію [Вмикання бібліотеки darknetxx-datadrivers](/doc/sections/uk_UA/5-39-enabling-the-darknetxx-datadrivers-library.md)): частина `darknetxx-datadrivers` клонує репозиторій змінної кешу `DARKNETXX_DXXDD_GIT` на гілці змінної `DARKNETXX_DXXDD_GIT_TAG` і виконує її побудову `Release` у префікс `/usr` пакета snap з пакетами розробки OpenCV, zlib і nlohmann JSON, додаючи до пакета бібліотеки виконання zlib і OpenCV, з якими вона лінкується, а частина проекту будується після неї (`after`). Пакет snap містить бінарний файл `darknetxxConverter`, який встановлює бібліотека, як застосунок `darknetxxConverter` поряд із застосунком `darknetxx` (див. секцію [Бінарний файл darknetxxConverter](/doc/sections/uk_UA/5-35-the-darknetxxConverter-binary.md)).
