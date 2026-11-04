## Обов'язкові інструменти для ОС на базі MS Windows

Для того щоб мати можливість розробляти і будувати проект необхідно завантажити MSVC інсталятор з сторінки [https://visualstudio.microsoft.com/downloads/](https://visualstudio.microsoft.com/downloads/) і увімкнути встановлення інструменти для нативної розробки для C++.

Окремо інсталятор на кожен необхідний проект можна завантажити з окремих джерел (**можлива необхідність пошуку і встановлення інших пакунків**):
- Система вірсіювання Git за адресою [https://git-scm.com/downloads/win](https://git-scm.com/downloads/win)
- Система побудови CMake за адресою [https://cmake.org/download/](https://cmake.org/download/)
- Пакунки Windows 10 SDK за адресою [https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/](https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/)

Проект будується з бібліотекою [darknetxx-datadrivers](https://github.com/yuriysydor1991/darknetxx-datadrivers) у кожній ОС: конфігурація проекту шукає встановлену бібліотеку і зупиняється з помилкою, якщо її не знайдено (див. секцію [Вмикання бібліотеки darknetxx-datadrivers](/doc/sections/uk_UA/5-39-enabling-the-darknetxx-datadrivers-library.md)).
