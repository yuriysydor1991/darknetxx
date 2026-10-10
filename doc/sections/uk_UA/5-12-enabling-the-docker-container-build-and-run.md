## Вмикання підтримки запуску Docker-контейнера

Щоб увімкнути і запустити Docker контейнер разом з цільовим бінарним виконуваним файлом поточного проекту-шаблону необхідно встановити у значення `ON` змінну `ENABLE_DOCKER` за допомогою команди:

```
# в середині кореневої директорії проекту

cmake -S . -B build -DENABLE_DOCKER=ON
```

Після чого можна запустити побудову контейнера Docker і слідуючий запуск цільового виконуваного файлу проекту за допомогою комнади (CMake-ціль `docker-single-run`):

```
# в середині кореневої директорії проекту

cmake --build build --target docker-single-run 
```

Для деталей CMake-цілі побудови `docker-single-run` слід звернутись до файлу `cmake/dockerers/darknetxx-docker-single-runner-target.cmake`.

### Скрипти запуску

Директорія [scripts/docker](/scripts/docker) містить скорочення, які виконують обидві наведені вище команди, отож увесь запуск виконується однією командою:

| Скрипт | Запускає CMake-ціль |
| --- | --- |
| [single-run.sh](/scripts/docker/single-run.sh) | `docker-single-run` |
| [single-build.sh](/scripts/docker/single-build.sh) | `docker-single-build` |
| [cpu-train-gui.sh](/scripts/docker/cpu-train-gui.sh) | `docker-single-run-darknetxx-cpu-train-gui` для теки YOLO4 з першого аргументу (див. секцію [Бінарний файл darknetxx-train-gui](/doc/sections/uk_UA/5-36-the-GUI-binaries.md#бінарний-файл-darknetxx-train-gui)) |

```
# в середині кореневої директорії проекту

scripts/docker/single-run.sh
```

Кожен з них конфігурує директорію `build/release` з параметром `ENABLE_DOCKER` у значенні `ON` і потім будує потрібну ціль, отож жодної власної команди `docker` не виконується. CMake-ціль `docker-single-run` залежить від цілі `docker-single-build`, а отже скрипт [single-run.sh](/scripts/docker/single-run.sh) будує і образ теж, а скрипт [single-build.sh](/scripts/docker/single-build.sh) потрібен лише для побудови образу без запуску контейнера.

Скрипти приймають ті ж самі параметри, що і [Швидкі скрипти побудови](/doc/sections/uk_UA/5-34-quick-build-scripts.md), оскільки саме їх вони і викликають:

```
# в середині кореневої директорії проекту

# повторне використання вже сконфігурованої директорії build/release
scripts/docker/single-run.sh --no-reconfigure

# локальний сокет служби Docker замість типової адреси tcp://127.0.0.1:2375
scripts/docker/single-run.sh -DDOCKER_HOST_STR=unix:///var/run/docker.sock
```

Слід пам'ятати, що параметр `ENABLE_DOCKER` робить наявність виконуваного файлу `docker` у системі обов'язковою для кроку конфігурації, отож на машині без встановленого Docker скрипти завершать крок конфігурації помилкою (див. секцію вимог [Необов'язкові пакети для запуску контейнера Docker](/doc/sections/uk_UA/3-10-optional-for-docker-container-runs.md)).

### Користувач контейнерів

Образи навчання та розпізнавання (`ENABLE_DOCKER_DARKNETXX_CPU_TRAIN`, `ENABLE_DOCKER_DARKNETXX_GPU_TRAIN`, `ENABLE_DOCKER_DARKNETXX_PREDICTOR` та образи оригінального Darknet `ENABLE_DOCKER_DARKNET` і `ENABLE_DOCKER_DARKNET_PREDICTOR`) запускають свої контейнери від імені звичайного користувача `darknetxx`, а не `root`, як навчання з командного рядка, так і навчання [darknetxx-train-gui](/doc/sections/uk_UA/5-36-the-GUI-binaries.md#бінарний-файл-darknetxx-train-gui). Образ надає цьому користувачу ідентифікатори користувача, який конфігурує проект (значення `id -u` та `id -g`), отож резервні копії ваг, графіки, результати розпізнавання та журнали, які контейнери записують у змонтовану теку YOLO4, належать користувачу хост-системи, і той змінює чи видаляє їх без `sudo`. CMake-змінні `DOCKER_USER_UID` та `DOCKER_USER_GID` задають інші ідентифікатори:

```
# в середині кореневої директорії проекту

cmake -S . -B build -DENABLE_DOCKER=ON -DENABLE_DOCKER_DARKNETXX_CPU_TRAIN=ON \
  -DYOLO4_TRAINING_DATA_DIR=/path/to/the/yolo4/folder \
  -DDOCKER_USER_UID=1001 -DDOCKER_USER_GID=1001
```

Ідентифікатори є аргументами побудови образу `USER_UID` та `USER_GID`, отож образ будується саме для того користувача, якого обслуговує. Користувач `ubuntu` базових образів Ubuntu 24.04 звільняє свої ідентифікатори `1000`, а інші облікові записи та групи з тими ж ідентифікаторами поділяють їх з користувачем `darknetxx`. Змонтована тека YOLO4 має належати користувачу хост-системи: відсутню теку Docker створює від імені `root`, і користувач `darknetxx` не може в неї записувати. Рецепт оригінального Darknet [Dockerfile.cpu](/misc/Dockerfiles/Dockerfile.cpu), який не будує жодна CMake-ціль, приймає ті ж аргументи побудови для свого користувача `yolo`.
