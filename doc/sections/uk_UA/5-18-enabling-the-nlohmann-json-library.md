## Вмикання інтеграції бібліотеки nlohmann JSON

Бібліотека [nlohmann JSON](https://github.com/nlohmann/json.git) потрібна лише тестам цього проекту. Файли ваг нейромереж dxxwjz (стиснені gzip документи JSON, див. секцію [Файли ваг нейромереж](/doc/sections/uk_UA/4-7-4-network-weights-files.md)) читає і записує бібліотека [darknetxx-datadrivers](https://github.com/yuriysydor1991/darknetxx-datadrivers), яка будується з власною бібліотекою nlohmann JSON і не встановлює жодного її заголовного файлу (див. секцію [Вмикання бібліотеки darknetxx-datadrivers](/doc/sections/uk_UA/5-39-enabling-the-darknetxx-datadrivers-library.md)), тоді як компонентні та інтеграційні тести цього проекту самі читають документи JSON, які записує бібліотека, за допомогою цілі `nlohmann_json::nlohmann_json` цього модуля.

CMake змінна `ENABLE_NLOHMANN_JSON` за замовчуванням має значення `ON` для проекту верхнього рівня і `OFF`, коли проект підключає інший проект, який не будує жодного з його тестів (див. кореневий [CMakeLists.txt](/CMakeLists.txt)). Якщо встановлену системну бібліотеку знайдено (пакет `nlohmann-json3-dev` у Debian і Ubuntu), використовується вона, інакше під час конфігурації з мережі завантажується її випуск `v3.11.3` (кеш-змінні `DARKNETXX_NLOHMANN_GIT` і `DARKNETXX_NLOHMANN_GIT_TAG` задають репозиторій і тег, а також модуль `nlohmann-json` маніфесту flatpak, з яким будується модуль маніфесту `darknetxx-datadrivers`). Змінна зі значенням `OFF` нічого не робить доступним, тож її вимикають разом з тестами.

```
# в середині кореневої директорії проекту

cmake -S . -B build -DENABLE_NLOHMANN_JSON=ON
```
