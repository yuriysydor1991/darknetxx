## Вмикання навчання на CUDA GPU

Нейромережі навчаються і розпізнають на GPU NVIDIA, якщо проект побудовано з набором інструментів CUDA (і CMake версії 3.23 або новішої):

```
cmake -S . -B build -DENABLE_CUDA=ON
```

Модуль [cmake/enablers/darknetxx-CUDA-enabler.cmake](/cmake/enablers/darknetxx-CUDA-enabler.cmake) компілює ядра CUDA оригінального Darknet (директорія [`src/detector-cmake-adaptor/darknet/src/cuda`](/src/detector-cmake-adaptor/darknet/src/cuda)) разом з його середовищем виконання CUDA (файл `dark_cuda.c`), визначає `GPU` (а також `CUDNN` і `CUDNN_HALF`) для кожної цілі і приєднує до них бібліотеки cuBLAS, cuRAND і cuDNN. Бібліотека cuDNN використовується, якщо її знайдено, а змінна середовища `CUDNN_ROOT_DIR` вказує конфігурації на cuDNN поза набором інструментів CUDA:

```
CUDNN_ROOT_DIR=/opt/cudnn cmake -S . -B build -DENABLE_CUDA=ON
```

За замовчуванням ядра компілюються для кожної основної архітектури GPU набору інструментів, а архітектура GPU самої машини скорочує побудову:

```
cmake -S . -B build -DENABLE_CUDA=ON -DCMAKE_CUDA_ARCHITECTURES=native
```

| Опція CMake | За замовчуванням | Дія |
| --- | --- | --- |
| `ENABLE_CUDA` | `OFF` | Навчання і розпізнавання на GPU NVIDIA. |
| `ENABLE_CUDNN` | `ON`, якщо cuDNN знайдено | Згорткові, повнозв'язні шари, шари пакетної нормалізації і пулінгу обчислює cuDNN. |
| `ENABLE_CUDNN_HALF` | значення `ENABLE_CUDNN` | Згортки половинної точності на тензорних ядрах GPU з обчислювальною здатністю 7.0 і вище. |
| `CMAKE_CUDA_ARCHITECTURES` | `all-major` | Архітектури GPU, для яких компілюються ядра. |

Бінарні файли побудови CUDA під час виконання використовують спільні бібліотеки середовища виконання CUDA, cuBLAS, cuRAND (і cuDNN) набору інструментів.

### Навчання на GPU

Бінарні файли побудови CUDA обчислюють на першому GPU машини. Список індексів GPU `--gpus` навчає копію нейромережі на кожному GPU списку, так само як `-gpus` оригінального Darknet, а розпізнавання обчислює на першому GPU списку:

```
darknetxx --train-cfg project.dxxwjz4p --gpus 0,1
```

Пристрій компонента [`src/darknet-adaptor/adaptors/Devices`](/src/darknet-adaptor/adaptors/Devices) розміщує створені завантажувачем нейромережі на їхньому GPU: кожен шар отримує свої масиви пристрою так, як їх виділяли оригінальні функції `make_*_layer()`, зміна розміру випадкових розмірів входу і зміна пакета розміщують їх знову, завантажені ваги копіюються на GPU, а збережені - назад з нього. Машина без CUDA GPU (або без драйвера NVIDIA) обчислює на CPU, попереджаючи про це.

Опція `optimized_memory` секції `[net]` не підтримується: шари зберігають всю свою пам'ять пристрою.
