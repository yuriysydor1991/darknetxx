**Спроба мігрувати хоча-б частину Darknet проекту на мову C++**

Оригінальний репозиторій взято з [https://github.com/AlexeyAB/darknet](https://github.com/AlexeyAB/darknet)

Деякі частини поточного стану проекту взято з С++ шаблонного проекту [https://github.com/yuriysydor1991/cpp-app-template.git](https://github.com/yuriysydor1991/cpp-app-template.git)

**Міграція в процесі. Проект поки не працює.**

# Рекомендований анотатор зображень

Для того щоб навчити мережу на власному наборі даних знадобляться анотовані зображення з мітками обмежувальних рамок у форматі Darknet/YOLO. Рекомендується готувати такі анотації за допомогою інструмента `ImagesAnnotator` що доступний за адресою [https://github.com/yuriysydor1991/ImagesAnnotator](https://github.com/yuriysydor1991/ImagesAnnotator).

# Документація

Цей файл - домашня сторінка проекту. Детальна документація розділена на секції
нижче. Англійська версія цих самих секцій доступна у файлі
[README.md](/README.md).

1. [Вимоги](/doc/sections/uk_UA/1-requirements.md)
1. [Структура проекту](/doc/sections/uk_UA/2-project-structure.md)
1. [Компоненти проекту і архітектура](/doc/sections/uk_UA/3-components-and-architecture.md)
    1. [Компоненти виконання програми](/doc/sections/uk_UA/3-1-application-runtime-components.md)
    1. [Компоненти адаптера Darknet](/doc/sections/uk_UA/3-2-darknet-adaptor-components.md)
    1. [Допоміжні компоненти](/doc/sections/uk_UA/3-3-supporting-components.md)
1. [Побудова проекту](/doc/sections/uk_UA/4-project-build.md)
1. [Запуск головного виконуваного файлу](/doc/sections/uk_UA/5-run-the-executable.md)
1. [Встановлення](/doc/sections/uk_UA/6-installing.md)

Цю саму документацію можна згенерувати як HTML-сайт для перегляду (разом з
відображеними діаграмами класів PlantUML) за допомогою Doxygen - дивись секцію
[Побудова документації](/doc/sections/uk_UA/4-project-build.md#побудова-документації).
