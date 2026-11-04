## Обов'язкові інструменти для ОС на базі GNU/Лінукс

Для того щоб виконати побудову проекту-шаблону необхідно встановити компілятор GCC C++ разом з системою побудови проекту CMake, системою версіювання Git, файлами розробки OpenCV, від яких залежить код Darknet, і файлами розробки zlib та nlohmann JSON, з якими будується бібліотека [darknetxx-datadrivers](https://github.com/yuriysydor1991/darknetxx-datadrivers) файлів ваг нейромереж (якщо двох останніх бібліотек у системі немає, її конфігурація завантажує їх з мережі); бібліотеками nlohmann JSON і zlib тести цього проекту також читають документи JSON бібліотеки (див. секції [Вмикання інтеграції бібліотеки nlohmann JSON](/doc/sections/uk_UA/5-18-enabling-the-nlohmann-json-library.md) і [Бібліотека zlib](/doc/sections/uk_UA/5-23-enabling-the-zlib-library.md)):

```
sudo apt install -y git g++ cmake libopencv-dev zlib1g-dev nlohmann-json3-dev
```

Проект будується з бібліотекою darknetxx-datadrivers, з її форматами файлів ваг нейромереж і проектів навчання, а бібліотека не є пакетом дистрибутива і не завантажується конфігурацією проекту, яка лише шукає її функцією `find_package()`: побудуй і встанови її ще до конфігурації проекту. Скрипт проекту клонує, будує і встановлює її однією командою за допомогою інструментів вище:

```
# в середині кореневої директорії проекту

misc/ci/install-darknetxx-datadrivers.sh
```

Він встановлює бібліотеку у префікс `/usr/local`; передай інший префікс як його єдиний аргумент і вкажи його конфігурації параметром `-DCMAKE_PREFIX_PATH=<prefix>` (див. секцію [Вмикання бібліотеки darknetxx-datadrivers](/doc/sections/uk_UA/5-39-enabling-the-darknetxx-datadrivers-library.md)).

[Бінарні файли GUI](/doc/sections/uk_UA/5-36-the-GUI-binaries.md) потребують також файлів розробки GTKmm 4 (Debian, Ubuntu 24.04 чи новіші), якщо їх не вимкнено параметром `-DENABLE_DARKNETXX_GUI=OFF` чи не вимкнено усі виконувані файли параметром `-DENABLE_DARKNETXX_EXECUTABLES=OFF` (див. секцію [Використання бібліотеки darknetxx](/doc/sections/uk_UA/5-37-using-the-darknetxx-library.md)):

```
sudo apt install -y libgtkmm-4.0-dev
```

[Бінарний файл редактора нейромереж](/doc/sections/uk_UA/5-38-the-network-editor-binaries.md) Qt 6 будується, щойно знайдено файли розробки Qt 6 Widgets і Svg:

```
sudo apt install -y qt6-base-dev qt6-svg-dev
```

Директорія [scripts/packages](/scripts/packages) містить скрипти, які встановлюють мінімальний набір пакетів, потрібних для побудови і запуску проекту локально, включно з фреймворком GTest для тестів:

| Скрипт | Встановлює пакети для |
| --- | --- |
| [install-debian.sh](/scripts/packages/install-debian.sh) | Debian, командою `apt-get install` |
| [install-ubuntu.sh](/scripts/packages/install-ubuntu.sh) | Ubuntu, яка має спільний з Debian набір пакетів |

```
# в середині кореневої директорії проекту

scripts/packages/install-ubuntu.sh
```

Запущені звичайним користувачем, скрипти запитують пароль `sudo`. Вони встановлюють лише пакети дистрибутива і наприкінці нагадують про скрипт `misc/ci/install-darknetxx-datadrivers.sh` вище, який встановлює бібліотеку darknetxx-datadrivers.
