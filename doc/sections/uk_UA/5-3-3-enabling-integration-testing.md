### Вмикання інтеграційних тестів

Інтеграційні тести перевіряють увесь виконуваний файл `darknetxx` так, як ним користуються: вони тренують крихітну нейромережу з нуля розпізнавати цифри від `0` до `9`, а потім запускають розпізнавання на кожному тренувальному зображенні й очікують розпізнавання цифри кожного з них. Набір тренувальних даних складається із зображень цифр з директорії [misc/data/labels](/misc/data/labels), по вісім розмірів кожної цифри, отож власних даних тестам не потрібно.

Той самий тест існує у двох варіантах, які відрізняються лише способом запуску виконуваного файлу `darknetxx` і перевірки його результатів:

| Тест | Чим виконується |
| --- | --- |
| [ITEST_digits](/src/tests/integration/digits) | виконуваний файл GoogleTest [ITEST_digits.cpp](/src/tests/integration/digits/ITEST_digits.cpp), який запускає розпізнавання `darknetxx` і розбирає його результати у форматі JSON, тоді як тренування виконує скрипт CMake [train-digits.cmake](/src/tests/integration/digits/train-digits.cmake) |
| [ITEST_digits_bash](/src/tests/integration/digits-bash) | лише скрипт bash [ITEST_digits_bash.sh](/src/tests/integration/digits-bash/ITEST_digits_bash.sh) без жодного тестового фреймворку: він запускає і тренування, і розпізнавання `darknetxx` та розбирає результати у форматі JSON за допомогою `awk` |

Для того щоб зробити інтеграційні тести доступними (для побудови і запуску) необхідно переконфігурувати проект з увімкненою опцією `ENABLE_INTEGRATION_TESTS` за допомогою команди (для GNU/Linux):

```
# у кореневій директорії проекту

cmake -B build -S . -DENABLE_INTEGRATION_TESTS=ON && cmake --build build
```

Варіанти складаються з наступних тестів CTest:

| Тест | Виконує |
| --- | --- |
| `ITEST_digits_training` | тренування запуском `darknetxx --train-cfg`, попередньо стерши ваги попереднього запуску |
| `ITEST_digits.recognizes_digit_<N>.noArgs` | розпізнавання `darknetxx --detect-cfg` на кожному з восьми зображень цифри `<N>`, очікуючи, що найвпевненіше розпізнаний об'єкт і є тією цифрою |
| `ITEST_digits_bash_training` | те саме тренування, що й тест `ITEST_digits_training`, скриптом bash |
| `ITEST_digits_bash.recognizes_digit_<N>` | ті самі перевірки розпізнавання, що й тести `ITEST_digits.recognizes_digit_<N>.noArgs`, скриптом bash |

Тренування є фікстурою CTest для тестів розпізнавання свого варіанту, отож `ctest` самостійно виконує його першим, навіть коли тести відфільтровано за їх назвами:

```
# у директорії побудови проекту

# варіант GoogleTest
ctest -R "ITEST_digits\." --output-on-failure

# варіант bash
ctest -R "ITEST_digits_bash\." --output-on-failure
```

Тренування кожного з варіантів триває близько 15 секунд для побудови `Release`, менше хвилини для побудови `Debug` і близько двох хвилин для побудови `Debug` з опцією `ENABLE_SANITIZERS`, а його тести розпізнавання тривають ще близько пів хвилини. Швидкий скрипт побудови [debug-configure.sh](/scripts/build/debug-configure.sh) вмикає опцію `ENABLE_INTEGRATION_TESTS`, отож команда `scripts/build/debug.sh --test` виконує інтеграційні тести разом з рештою тестів (див. секцію [Швидкі скрипти побудови](/doc/sections/uk_UA/5-34-quick-build-scripts.md)).

Збірка з опцією `ENABLE_SANITIZERS_THREADS` передає ThreadSanitizer тестів файл придушень [misc/.tsan-suppress](/misc/.tsan-suppress). Він придушує лише відому гонитву даних потоків тренування шару yolo, які ведуть статистику тренування у спільному лічильнику так само, як первісний Darknet, тож будь-яка інша гонитва даних і надалі завершує тренування помилкою.

Усі файли тесту - згенерований набір даних, конфігурація нейромережі, натреновані ваги, графік тренування, вивід кожного запуску `darknetxx` і результати розпізнавання - залишаються всередині директорії його варіанту дерева побудови, `src/tests/integration/digits` чи `src/tests/integration/digits-bash`.

Варіант bash не потребує жодної власної побудови, отож його скрипт [ITEST_digits_bash.sh](/src/tests/integration/digits-bash/ITEST_digits_bash.sh) так само вручну перевіряє будь-який виконуваний файл `darknetxx`, зокрема встановлений. Скрипт отримує виконуваний файл і директорію тесту, всередині якої залишає усі файли тесту:

```
# у кореневій директорії проекту

src/tests/integration/digits-bash/ITEST_digits_bash.sh train "$(command -v darknetxx)" /tmp/itest-digits misc/data/labels

for digit in 0 1 2 3 4 5 6 7 8 9; do
  src/tests/integration/digits-bash/ITEST_digits_bash.sh recognize "$(command -v darknetxx)" /tmp/itest-digits "${digit}"
done
```
