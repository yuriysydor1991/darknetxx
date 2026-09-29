## Обов'язкові інструменти для ОС на базі GNU/Лінукс

Для того щоб виконати побудову проекту-шаблону необхідно встановити компілятор GCC C++ разом з системою побудови проекту CMake, системою версіювання Git і файлами розробки OpenCV, від яких залежить код Darknet:

```
sudo apt install -y git g++ cmake libopencv-dev
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
