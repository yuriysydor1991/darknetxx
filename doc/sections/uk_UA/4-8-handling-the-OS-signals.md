# Обробка сигналів операційної системи

Сигнали операційної системи, які просять процес зупинитись або призупинитись, обробляються підкомпонентом `src/app/signals-handlers` компонента `src/app`, запозиченим з проекту [шаблону програми C++](https://github.com/yuriysydor1991/cpp-app-template):

```
src/app/signals-handlers
├── CMakeLists.txt
├── ISignalsHandler.h
├── SignalsHandler.cpp
├── SignalsHandler.h
├── handlers
│   ├── ASignalsHandler.cpp
│   ├── ASignalsHandler.h
│   ├── CMakeLists.txt
│   ├── posix
│   │   ├── CMakeLists.txt
│   │   ├── ContinueSignalsHandler.cpp
│   │   ├── ContinueSignalsHandler.h
│   │   ├── IgnoredSignalsHandler.cpp
│   │   ├── IgnoredSignalsHandler.h
│   │   ├── PauseSignalsHandler.cpp
│   │   ├── PauseSignalsHandler.h
│   │   ├── PosixSignalsHandler.cpp
│   │   ├── PosixSignalsHandler.h
│   │   ├── ReloadSignalsHandler.cpp
│   │   ├── ReloadSignalsHandler.h
│   │   ├── StopSignalsHandler.cpp
│   │   ├── StopSignalsHandler.h
│   │   ├── UserSignalsHandler.cpp
│   │   ├── UserSignalsHandler.h
│   │   └── tests
│   │       ├── mocks
│   │       └── unit
│   └── tests
│       └── unit
└── tests
    ├── mocks
    │   └── SignalsHandler
    └── unit
        └── SignalsHandler
```

Кожен клас підкомпонента реалізує один і той самий інтерфейс `ISignalsHandler`, тому решта програми залежить лише від інтерфейсу і не знає ані кількості категорій сигналів, ані того, яка платформа їх оголошує:

```
ISignalsHandler                          єдиний тип, від якого залежить програма
├── SignalsHandler                       фасад над усіма категоріями
└── ASignalsHandler                      механіка встановлення та звільнення std::signal
    └── PosixSignalsHandler              родина платформ POSIX
        ├── StopSignalsHandler           SIGINT, SIGTERM, SIGQUIT, SIGHUP
        ├── PauseSignalsHandler          SIGTSTP, SIGTTIN, SIGTTOU
        ├── ContinueSignalsHandler       SIGCONT
        ├── ReloadSignalsHandler         SIGHUP
        ├── UserSignalsHandler           SIGUSR1, SIGUSR2
        └── IgnoredSignalsHandler        SIGPIPE
```

Окремий вихідний файл діаграми класів нижче - [`doc/diagrams/signals-handlers-classes.puml`](/doc/diagrams/signals-handlers-classes.puml).

@startuml
hide empty members

interface ISignalsHandler {
  +install(ctx) : bool
  +uninstall()
}

class SignalsHandler {
  +install(ctx) : bool
  +uninstall()
  #create_handlers() : vector<ISignalsHandler>
  -mhandlers : vector<ISignalsHandler>
}

abstract class ASignalsHandler {
  +install(ctx) : bool
  +uninstall()
  #{abstract} get_handled_signals() : set<int>
  #{abstract} get_signal_disposition() : SignalDisposition
  #{abstract} store_context(ctx)
  -minstalled_signals : set<int>
}

abstract class PosixSignalsHandler {
  базовий клас родини платформ POSIX
}

class StopSignalsHandler {
  SIGINT, SIGTERM, SIGQUIT, SIGHUP
  #{static} stop_context(signalNumber)
}

class PauseSignalsHandler {
  SIGTSTP, SIGTTIN, SIGTTOU
  #{static} pause_context(signalNumber)
}

class ContinueSignalsHandler {
  SIGCONT
  #{static} continue_context(signalNumber)
}

class ReloadSignalsHandler {
  SIGHUP
  #{static} reload_context(signalNumber)
}

class UserSignalsHandler {
  SIGUSR1, SIGUSR2
  #{static} request_context(signalNumber)
}

class IgnoredSignalsHandler {
  SIGPIPE, поведінка SIG_IGN
}

struct ApplicationContext {
  +stop : atomic_bool
  +stop_signal : atomic_int
  +pause : atomic_bool
  +reload : atomic_bool
  +first_user_request : atomic_bool
  +second_user_request : atomic_bool
}

ISignalsHandler <|.. SignalsHandler
ISignalsHandler <|.. ASignalsHandler
ASignalsHandler <|-- PosixSignalsHandler
PosixSignalsHandler <|-- StopSignalsHandler
PosixSignalsHandler <|-- PauseSignalsHandler
PosixSignalsHandler <|-- ContinueSignalsHandler
PosixSignalsHandler <|-- ReloadSignalsHandler
PosixSignalsHandler <|-- UserSignalsHandler
PosixSignalsHandler <|-- IgnoredSignalsHandler

SignalsHandler o--> ISignalsHandler : створює і передає виклики

StopSignalsHandler --> ApplicationContext : підіймає stop
PauseSignalsHandler --> ApplicationContext : підіймає pause
ContinueSignalsHandler --> ApplicationContext : опускає pause
ReloadSignalsHandler --> ApplicationContext : підіймає reload
UserSignalsHandler --> ApplicationContext : підіймає запити користувача

note bottom of IgnoredSignalsHandler
  Не звертається до контексту зовсім:
  ігноровані сигнали відкидає сама ОС
end note

@enduml

1. Клас `SignalsHandler` є **фасадом** усього підкомпонента. Він створює обробник кожної оброблюваної категорії сигналів у своєму методі `SignalsHandler::create_handlers` і передає кожному з них виклики `ISignalsHandler::install` та `ISignalsHandler::uninstall`.
1. Клас `ASignalsHandler` є **спільним каркасом** обробника однієї категорії, побудованим на виклику `std::signal`. Він знає, як підписати процес на набір сигналів операційної системи і як повернути їм попередню поведінку, тоді як нащадок лише повідомляє, з яких сигналів складається його категорія, яка процедура на них реагує і де зберігається вказівник на оброблюваний контекст.
1. Клас `PosixSignalsHandler` є **базовим класом родини платформ**, який робить платформу обробника частиною його типу, а не лише назвою директорії. Він навмисно не додає власної поведінки — натомість він класифікує нащадків, утримує спільне для всієї родини твердження щодо неблокуючих атомарних значень і зберігає місце для допоміжних засобів, які родина може розділити у майбутньому, тож обробник іншої родини, наприклад консольних подій MS Windows, стає очевидно іншим типом, а не схожим сусідом.
1. Директорія `handlers/posix` містить самі **обробники категорій** — по одному на кожну реакцію, яку може мати програма. Вони спираються на стандартний виклик `std::signal`, тому залишаються придатними до збирання будь-де, а сигнали, які оголошують лише POSIX-платформи, захищено директивою `ifdef` і на інших платформах вони просто не обробляються.

Метод `ApplicationFactory::run` створює реалізацію інтерфейсу `ISignalsHandler` за умовчанням — фасад `SignalsHandler` — безпосередньо перед запуском створеної програми і передає контекст програми до його методу `ISignalsHandler::install`. З цього моменту кожен оброблюваний сигнал підіймає один з атомарних прапорців структури [`ApplicationContext`](/src/app/ApplicationContext.h), а оброблювані сигнали отримують назад свою попередню поведінку одразу після завершення методу `ApplicationFactory::run`.

**Сигнал, який процес успадкував проігнорованим, залишається проігнорованим** і не обробляється взагалі: засіб `nohup` залишає проігнорованим сигнал `SIGHUP`, а неінтерактивна оболонка запускає свої фонові завдання з проігнорованими сигналами `SIGINT` та `SIGQUIT`, тож ці сигнали не досягають роботи, від якої їх навмисно відгородили. Обробники POSIX перевіряють поточну поведінку викликом `sigaction` ще до встановлення будь-чого (метод `PosixSignalsHandler::is_ignored`), тоді як переносний код `ASignalsHandler` розпізнає успадковане ігнорування за поведінкою, яку замінив його виклик `std::signal`.

Оброблюваний сигнал втрачає свою типову поведінку, тому метод `SignalsHandler::create_handlers` у darknetxx створює лише ті категорії, на які програма реагує, — категорії зупинки та ігнорування. Решта категорій залишаються доступними як класи підкомпонента, а їхні сигнали зберігають типову поведінку операційної системи.

| Обробник категорії | Оброблювані сигнали | Підіймутий прапорець `ApplicationContext` | Створюється у darknetxx |
| --- | --- | --- | --- |
| `StopSignalsHandler` | `SIGINT`, `SIGTERM`, `SIGQUIT`, `SIGHUP` | `stop` та `stop_signal` | так |
| `PauseSignalsHandler` | `SIGTSTP`, `SIGTTIN`, `SIGTTOU` | `pause` | ні |
| `ContinueSignalsHandler` | `SIGCONT` | `pause` (опускається) | ні |
| `ReloadSignalsHandler` | `SIGHUP` | `reload` | ні |
| `UserSignalsHandler` | `SIGUSR1`, `SIGUSR2` | `first_user_request`, `second_user_request` | ні |
| `IgnoredSignalsHandler` | `SIGPIPE` | жодного, сигнал ігнорується | так |

### Сигнали зупинки програми

Клас `StopSignalsHandler` обробляє сигнал `SIGINT` (комбінація клавіш Ctrl+C), `SIGTERM` (команда `kill`) та наявні лише у POSIX-системах `SIGQUIT` і `SIGHUP` (останні мовчки пропускаються на платформах, які їх не оголошують, наприклад на MS Windows). У darknetxx немає налаштувань для перезавантаження, тому сигнал `SIGHUP` закритого термінала належить тут до сигналів зупинки, див. запити перезавантаження налаштувань нижче. Процедура також зберігає номер отриманого сигналу у полі `ApplicationContext::stop_signal`.

Тому реалізація програми повинна перевіряти прапорець `ApplicationContext::stop` у кожній тривалій процедурі своєї реалізації `IApplication::run` і завершуватись одразу після того, як прапорець підіймуто. Прапорець є атомарним, тому його безпечно перевіряти з будь-якого потоку програми.

**Перше надходження сигналу зупинки є лише запитом на плавну зупинку.** Копії сигналу, що надходять упродовж періоду `StopSignalsHandler::GRACE_PERIOD_MS` (одна секунда), належать до того самого запиту — наприклад, засіб `timeout` надсилає сигнал процесу, а потім усій його групі процесів, а закритий термінал надсилає `SIGHUP` двічі, — тоді як повторний сигнал, що надходить пізніше, — наприклад, друге натискання Ctrl+C — одразу завершує процес типовою поведінкою операційної системи для цього сигналу. Описаний нижче спостерігач зупинки записує попередження до журналу, щойно помічає запит.

Зупинене навчання зберігає поточні ваги у файл `<base>_last.weights` (досягнута межа `--stop-less-avg-loss` натомість зберігає файл `<base>_final.weights`), а зупинене розпізнавання пропускає свої результати. Після звільнення всієї роботи метод `ApplicationFactory::execute` завершує процес типовою поведінкою операційної системи для того самого сигналу зупинки, тож батьківський процес бачить процес, завершений сигналом, а не звичайний вихід: оболонка повідомляє статус `128 + номер сигналу` — наприклад `130` для сигналу `SIGINT`, — а сценарій оболонки, перерваний натисканням Ctrl+C, також зупиняється. Винятком є сигнал `SIGQUIT`, типова поведінка якого додає дамп пам'яті вже зупиненого процесу, тож натомість програма завершується зі статусом `131`.

Уся робота Darknet відбувається всередині одного блокуючого виклику первісного коду мовою C — наприклад `train_detector_ctx` для навчання — тому під час її виконання жодний код C++ не отримує керування. Саме тому клас `darknet_adaptor::DarknetContext` запускає у своєму методі `DarknetContext::init` власний потік-**спостерігач зупинки**, який перевіряє той самий прапорець `ApplicationContext::stop` і переносить його до поля `detector_context::stop` первісного контексту обчислень, яке цикли мовою C перевіряють щоітерації. Деструктор класу `DarknetContext` очікує на завершення даного потоку.

Спостерігач існує тому, що **процедура, яку операційна система викликає при надходженні сигналу, може використовувати лише async signal safe виклики**, тож обробники підкомпонента працюють винятково з неблокуючими атомарними прапорцями — попередня пара `Application::init_signals` та `Application::handle_sigint` натомість обходила вектор підписників і виконувала журналювання просто з контексту сигналу, чого там не дозволено робити.

### Сигнали призупинення програми

Клас `PauseSignalsHandler` обробляє наявні лише у POSIX-системах сигнали, які просять процес призупинитись, — `SIGTSTP` (комбінація клавіш Ctrl+Z), `SIGTTIN` (фонове завдання читає з термінала) та `SIGTTOU` (фонове завдання пише до термінала). Усі три підіймають прапорець `ApplicationContext::pause`. Клас `ContinueSignalsHandler` обробляє сигнал `SIGCONT`, який надсилають команди керування завданнями `fg` та `bg`, і опускає той самий прапорець назад.

Тому реалізація програми повинна перевіряти прапорець `ApplicationContext::pause` так само, як і прапорець зупинки, і призупиняти свою роботу, доки прапорець підіймуто, замість того щоб завершувати метод `IApplication::run`. Даний прапорець також є атомарним.

**Слід пам'ятати, що обробка сигналів призупинення заміщує типову поведінку операційної системи, тому комбінація клавіш Ctrl+Z більше не призупиняє процес** — реагувати на підіймутий прапорець призупинення має сама програма. Приберіть клас `PauseSignalsHandler` з набору методу `SignalsHandler::create_handlers`, якщо бажаною є звична поведінка керування завданнями оболонки, або нехай програма самостійно викликає `std::raise(SIGSTOP)` тоді, коли вона готова бути призупиненою. Ніколи не обробляйте сигнали `SIGTTIN` та `SIGTTOU` без призупинення процесу: інакше доступ фонового завдання до термінала знову і знову їх породжує.

Програма darknetxx не перевіряє прапорця призупинення, тому її метод `SignalsHandler::create_handlers` не створює ані категорії `PauseSignalsHandler`, ані категорії `ContinueSignalsHandler`, і комбінація клавіш Ctrl+Z разом з рештою керування завданнями оболонки продовжують працювати.

Сигнал `SIGSTOP` навмисно відсутній у наборах оброблюваних сигналів, оскільки **жодному процесу не дозволено ані обробляти, ані блокувати, ані ігнорувати його** — операційна система призупиняє процес при його надходженні без жодного сповіщення, тому таке призупинення залишається непоміченим програмою.

Категорії призупинення та продовження є порожніми на платформах, які не оголошують дані POSIX-сигнали, наприклад на MS Windows, тому їхні виклики `ISignalsHandler::install` повідомляють там про невдачу, яку фасад сприймає спокійно, доки оброблюється хоча б одна інша категорія.

### Сигнали перезавантаження налаштувань

Клас `ReloadSignalsHandler` обробляє сигнал `SIGHUP`, який операційна система надсилає при закритті термінала і який навмисно надсилають засоби керування службами. Він підіймає прапорець `ApplicationContext::reload`, тому програма повинна перевіряти його, перечитувати свої налаштування і самостійно опускати прапорець назад.

Даний сигнал є усталеним запитом перезавантаження налаштувань для тривало працюючих процесів, тому він належить сюди, а не до категорії зупинки. **Типова реакція операційної системи на нього завершує процес, тож його обробка означає, що закриття термінала більше не припиняє тривале навчання.** Приберіть даний клас з набору методу `SignalsHandler::create_handlers` або поверніть номер `SIGHUP` до набору `StopSignalsHandler::get_handled_signals`, якщо очікуваною реакцією є саме завершення.

У darknetxx немає налаштувань для перезавантаження, тому вона робить саме останнє: категорія `ReloadSignalsHandler` не створюється, а закриття термінала плавно зупиняє навчання зі збереженням останніх ваг. Запускайте навчання засобом `nohup`, щоб воно продовжувалось і після закриття термінала.

### Сигнали, визначені програмою

Клас `UserSignalsHandler` обробляє сигнали `SIGUSR1` та `SIGUSR2`, які операційна система полишає для власних потреб програми і не надає їм жодного власного значення. Перший підіймає прапорець `ApplicationContext::first_user_request`, а другий — прапорець `ApplicationContext::second_user_request`, тож це єдиний обробник компонента, який розрізняє свої сигнали за номером, що його передає процедурі операційна система.

**Значення обох запитів визначає сама програма** — класичними є перевідкриття файлів журналу після ротації та вивід поточного стану навчання. Програма повинна перевіряти прапорці, обслуговувати запит і самостійно опускати прапорець назад. Прапорці об'єднують повторення, тому серія однакових сигналів є одним запитом — рахуйте надходження всередині процедури, якщо необхідно обслужити кожне з них.

Типова реакція операційної системи на обидва сигнали завершує процес, тож їх обробка залишає програму живою після команди `kill -USR1 <pid>`. Програма darknetxx не обслуговує запитів користувача, тому вона не створює категорії `UserSignalsHandler`, і обидва сигнали й надалі завершують процес.

### Сигнали, які ігноруються

Клас `IgnoredSignalsHandler` обробляє сигнал `SIGPIPE`, який надходить при записі до каналу або сокета, який більше ніхто не читає. Це єдиний обробник компонента, який встановлює значення `SIG_IGN` замість процедури, тому він взагалі не звертається до контексту програми, а його перевизначення методу `ASignalsHandler::store_context` є порожнім.

**Типова реакція операційної системи на даний сигнал мовчки завершує процес**, що перетворює від'єднання іншої сторони на незрозумілу смерть програми. Ігнорування сигналу змушує невдалий запис повернути помилку `EPIPE`, тож код, що викликає запис, має змогу помітити розірване з'єднання і повідомити про нього — саме тому практично кожна програма, яка пише до каналу, ігнорує даний сигнал.

### Перехоплення хибних обчислень

Сигнал `SIGFPE` **не** є частиною підкомпонента, оскільку він повідомляє про хибне обчислення процесу, який вже вмирає, а не про запит щодо життєвого циклу програми. Метод `Application::init_computation_traps` вмикає виключення обчислень з рухомою комою викликом `feenableexcept` і перехоплює сигнал, яким вони надходять, тож хибне обчислення стає помітним замість того, щоб розповзатись мережею як значення `NaN`. Процедура перехоплення повідомляє про виключення async signal safe викликом `write` і завершує процес викликом `_exit`.

Обчислення Darknet подекуди покладаються на семантику IEEE для переповнень та недійсних операцій з рухомою комою — наприклад, логістична активація впевненого логіта переповнює виклик `expf`, а статистика mAP ділить нуль на нуль для класу без розпізнавань — тому **перехоплення є діагностикою, вимкненою за замовчуванням**. Для його ввімкнення слід конфігурувати проект з опцією CMake `-DENABLE_FP_TRAPS=ON`.

### Розширення обробки сигналів

1. Додати або прибрати номери оброблюваних сигналів операційної системи можна у перевизначеному методі `ASignalsHandler::get_handled_signals` відповідного обробника категорії.
1. Цілком нову категорію сигналів можна додати, розмістивши власний клас-нащадок `PosixSignalsHandler` у директорії `src/app/signals-handlers/handlers/posix`, додавши новий файл `.cpp` до виклику `target_sources` у файлі побудови `CMakeLists.txt` даної директорії і створивши його у методі `SignalsHandler::create_handlers`. Нащадок повинен перевизначити методи `ASignalsHandler::get_handled_signals`, `ASignalsHandler::get_signal_disposition` та `ASignalsHandler::store_context`, утримувати власне поле типу `std::atomic<ApplicationContext*>` і **викликати метод `ISignalsHandler::uninstall` зі свого власного деструктора**, оскільки ані деструктор класу `ASignalsHandler`, ані деструктор класу `PosixSignalsHandler` не здатні дістатись до суто віртуального методу `ASignalsHandler::store_context`.
1. Родину платформ, до якої POSIX-сигнали не пасують, можна охопити, розмістивши власний клас-нащадок `ASignalsHandler` — базовий клас родини на кшталт `PosixSignalsHandler` — у новій директорії `src/app/signals-handlers/handlers/<платформа>`, зареєструвавши дану директорію за допомогою виклику `add_subdirectory` у файлі побудови `src/app/signals-handlers/handlers/CMakeLists.txt` і успадкувавши категорії даної родини від нього. Фасад спокійно сприймає категорію, яка нічого не обробляє на поточній платформі, тому родини усіх підтримуваних платформ можна створювати поруч у методі `SignalsHandler::create_handlers`.
1. Для цілком іншої реакції необхідно реалізувати власний клас-нащадок `ISignalsHandler`, розмістити його у директорії `src/app/signals-handlers`, додати новий файл `.cpp` до виклику `target_sources` у файлі побудови `src/app/signals-handlers/CMakeLists.txt` і повертати його з перевизначеного методу `ApplicationFactory::create_default_signals_handler`.
1. Новий прапорець нової категорії необхідно додати до структури [`ApplicationContext`](/src/app/ApplicationContext.h) як поле типу `std::atomic_bool` поруч з прапорцями `stop`, `pause`, `reload` та прапорцями запитів користувача, оскільки **процедура сигналу працює лише з неблокуючими атомарними значеннями** — звичайне поле типу `bool` таким не є.
1. Модульний тест нового класу необхідно розмістити у директорії `src/app/signals-handlers/tests/unit/<ВашОбробник>`, або у директорії `src/app/signals-handlers/handlers/<платформа>/tests/unit/<ВашОбробник>` для обробника категорії, і зареєструвати дану директорію за допомогою виклику `add_subdirectory` у відповідному файлі побудови `tests/unit/CMakeLists.txt`. Директорії макетів підкомпонента надаються усьому компоненту `src/app` файлом побудови `src/app/CMakeLists.txt` через змінні CMake `${SIGNALS_HANDLERS_MOCKS_ROOT}` та `${POSIX_SIGNALS_HANDLERS_MOCKS_ROOT}`, так само як власні макети компонента надаються через змінну `${APP_MOCKS_ROOT}`.

**Слід пам'ятати, що процедура, яку операційна система викликає при надходженні сигналу, може використовувати лише async signal safe виклики**, тому кожна з них — `StopSignalsHandler::stop_context`, `PauseSignalsHandler::pause_context`, `ContinueSignalsHandler::continue_context`, `ReloadSignalsHandler::reload_context` та `UserSignalsHandler::request_context` — працює винятково з неблокуючими (lock free) атомарними значеннями і не виконує ані журналювання, ані виділення пам'яті, ані блокувань. Клас `IgnoredSignalsHandler` взагалі не встановлює процедури, оскільки значення `SIG_IGN` є самостійною припустимою відповіддю методу `ASignalsHandler::get_signal_disposition`.
