## Обов'язкові інструменти для ОС на базі GNU/Лінукс

Для того щоб виконати побудову проекту-шаблону необхідно встановити компілятор GCC C++ разом з системою побудови проекту CMake, системою версіювання Git, файлами розробки OpenCV, від яких залежить код Darknet, і файлами розробки zlib та nlohmann JSON, від яких залежать файли ваг нейромереж dxxwjz1 (якщо двох останніх бібліотек у системі немає, під час конфігурації вони завантажуються з мережі, див. секції [Вмикання інтеграції zlib](/doc/sections/uk_UA/5-23-enabling-the-zlib-library.md) і [Вмикання інтеграції бібліотеки nlohmann JSON](/doc/sections/uk_UA/5-18-enabling-the-nlohmann-json-library.md)):

```
sudo apt install -y git g++ cmake libopencv-dev zlib1g-dev nlohmann-json3-dev
```

[Бінарні файли GUI](/doc/sections/uk_UA/5-36-the-GUI-binaries.md) потребують також файлів розробки GTKmm 4 (Debian, Ubuntu 24.04 чи новіші), якщо їх не вимкнено параметром `-DENABLE_DARKNETXX_GUI=OFF` чи не вимкнено усі виконувані файли параметром `-DENABLE_DARKNETXX_EXECUTABLES=OFF` (див. секцію [Використання бібліотеки darknetxx](/doc/sections/uk_UA/5-37-using-the-darknetxx-library.md)):

```
sudo apt install -y libgtkmm-4.0-dev
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

Запущені звичайним користувачем, скрипти запитують пароль `sudo`.
