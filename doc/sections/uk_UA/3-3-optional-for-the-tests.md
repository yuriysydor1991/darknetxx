## Необов'язкові пакети для тестів

Якщо необхідно побудувати і виконати доступні тести необхідно встановити GTest або забезпечити доступ до мережі Інтернет у випадку самостійного встановлення скриптами системи збірки CMake проекту-шаблону. Для встановлення необхідно виконати команду:

```
sudo apt install -y googletest
```

Тести файлів ваг нейромереж самі читають стиснені gzip документи JSON, які записує бібліотека [darknetxx-datadrivers](https://github.com/yuriysydor1991/darknetxx-datadrivers), тож вони потребують бібліотеки nlohmann JSON (системної або тієї, яку завантажує конфігурація, див. секцію [Вмикання інтеграції бібліотеки nlohmann JSON](/doc/sections/uk_UA/5-18-enabling-the-nlohmann-json-library.md)) і системних файлів розробки zlib (див. секцію [Бібліотека zlib](/doc/sections/uk_UA/5-23-enabling-the-zlib-library.md)):

```
sudo apt install -y nlohmann-json3-dev zlib1g-dev
```

Більш детально у секції [тести проекту](/doc/sections/uk_UA/4-4-project-tests.md).
