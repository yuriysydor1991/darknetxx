# Handling the OS signals

The OS signals which are asking the process to stop or to pause are covered by the `src/app/signals-handlers` subcomponent of the `src/app` component, adopted from the [C++ application template](https://github.com/yuriysydor1991/cpp-app-template) project:

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

Every class of the subcomponent implements the very same `ISignalsHandler` interface, so the rest of the application depends on the interface only and knows neither how many signals categories exist nor which platform declares them:

```
ISignalsHandler                          the only type the application depends on
├── SignalsHandler                       the facade over every category
└── ASignalsHandler                      the std::signal install and uninstall mechanics
    └── PosixSignalsHandler              the POSIX platforms family
        ├── StopSignalsHandler           SIGINT, SIGTERM, SIGQUIT, SIGHUP
        ├── PauseSignalsHandler          SIGTSTP, SIGTTIN, SIGTTOU
        ├── ContinueSignalsHandler       SIGCONT
        ├── ReloadSignalsHandler         SIGHUP
        ├── UserSignalsHandler           SIGUSR1, SIGUSR2
        └── IgnoredSignalsHandler        SIGPIPE
```

The standalone source of the class diagram below is [`doc/diagrams/signals-handlers-classes.puml`](/doc/diagrams/signals-handlers-classes.puml).

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
  the POSIX platforms family base
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
  SIGPIPE, the SIG_IGN disposition
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

SignalsHandler o--> ISignalsHandler : creates and forwards to

StopSignalsHandler --> ApplicationContext : raises stop
PauseSignalsHandler --> ApplicationContext : raises pause
ContinueSignalsHandler --> ApplicationContext : lowers pause
ReloadSignalsHandler --> ApplicationContext : raises reload
UserSignalsHandler --> ApplicationContext : raises the user requests

note bottom of IgnoredSignalsHandler
  Touches no context at all: the OS
  drops the ignored signals itself
end note

@enduml

1. The `SignalsHandler` class is the **facade** of the whole subcomponent. It creates a subhandler of every covered signals category in it's `SignalsHandler::create_handlers` method and forwards the `ISignalsHandler::install` and the `ISignalsHandler::uninstall` calls to each of them.
1. The `ASignalsHandler` class is the **common std::signal based skeleton** of a single category handler. It knows how to subscribe the process to a set of the OS signals and how to give them back the dispositions they had before, while a descendant only tells which signals it's category consists of, which routine reacts on them and where the handled context pointer is kept.
1. The `PosixSignalsHandler` class is the **platform family base**, which makes the platform of a handler a part of it's type and not a directory name only. It adds no behaviour of it's own on purpose - it classifies the descendants instead, holds the lock free atomic assertion the whole family shares and keeps a place for the helpers it may share one day, so a handler of another family, a MS Windows console control one for example, becomes an obviously different type rather than a look alike neighbour.
1. The `handlers/posix` directory holds the **category handlers** themselves - one per reaction the application may have. They rest on the standard `std::signal` call, so they stay compilable everywhere, while the signals which are declared by the POSIX platforms only are guarded with the `ifdef` and are simply not covered elsewhere.

The `ApplicationFactory::run` method creates the default `ISignalsHandler` interface implementation - the `SignalsHandler` facade - right before the created application start and hands the application context over to it's `ISignalsHandler::install` method. Every handled signal raises one of the [`ApplicationContext`](/src/app/ApplicationContext.h) atomic flags from that moment, and the handled signals are given back the dispositions they had before as soon as the `ApplicationFactory::run` returns.

**A signal the process inherited ignored stays ignored** and is not covered at all: the `nohup` tool leaves the `SIGHUP` ignored and a non interactive shell starts its background jobs with the `SIGINT` and the `SIGQUIT` ignored, so the signals do not reach the work they were kept away from. The POSIX handlers query the current disposition with the `sigaction` call before installing anything (the `PosixSignalsHandler::is_ignored` method), while the portable `ASignalsHandler` code recognizes the inherited ignore by the disposition it's `std::signal` call replaced.

A handled signal loses its OS default disposition, so the `SignalsHandler::create_handlers` method of the darknetxx creates the categories the application acts on only - the stop and the ignored ones. The rest of the categories stay available as the classes of the subcomponent, while their signals keep the OS default dispositions.

| Category handler | Covered signals | The raised `ApplicationContext` flag | Created by the darknetxx |
| --- | --- | --- | --- |
| `StopSignalsHandler` | `SIGINT`, `SIGTERM`, `SIGQUIT`, `SIGHUP` | `stop` and `stop_signal` | yes |
| `PauseSignalsHandler` | `SIGTSTP`, `SIGTTIN`, `SIGTTOU` | `pause` | no |
| `ContinueSignalsHandler` | `SIGCONT` | `pause` (lowered) | no |
| `ReloadSignalsHandler` | `SIGHUP` | `reload` | no |
| `UserSignalsHandler` | `SIGUSR1`, `SIGUSR2` | `first_user_request`, `second_user_request` | no |
| `IgnoredSignalsHandler` | `SIGPIPE` | none, the signal is ignored | yes |

### The application stop signals

The `StopSignalsHandler` class covers the `SIGINT` one of the Ctrl+C keys combination, the `SIGTERM` one of the `kill` command and the POSIX only `SIGQUIT` and `SIGHUP` ones (silently skipped on the platforms which are not declaring them, like the MS Windows one). The darknetxx has no configuration to reload, so the `SIGHUP` of a closed terminal is a stop signal here, see the configuration reload requests below. The routine keeps the arrived signal number in the `ApplicationContext::stop_signal` field too.

So the application implementation is expected to poll the `ApplicationContext::stop` flag inside every long running routine of it's `IApplication::run` implementation and to return as soon as the flag is raised. The flag is an atomic one, so it is safe to poll it from any thread of the application.

**The first arrival of a stop signal is a graceful stop request only.** The copies of the signal arriving within the `StopSignalsHandler::GRACE_PERIOD_MS` period (one second) belong to the very same request - the `timeout` tool, for example, signals the process and then it's whole process group, and a closed terminal sends the `SIGHUP` twice - while a repeated signal arriving later - the second Ctrl+C, for example - terminates the process right away with the OS default disposition of the signal. The stop watcher described below logs a warning as soon as it notices the request.

A stopped training saves the current weights as the `<base>_last.weights` file (the reached `--stop-less-avg-loss` limit saves the `<base>_final.weights` one instead) and a stopped detection skips its outputs. Once the work is released, the `ApplicationFactory::execute` method ends the process by the OS default disposition of the very stop signal, so the parent process sees a process the signal terminated rather than a normal exit: a shell reports the `128 + signal number` status - `130` for the `SIGINT`, for example - and a shell script interrupted by the Ctrl+C stops as well. The `SIGQUIT` signal is the exception, since it's default disposition adds a core dump of the already stopped process, so the application exits with the `131` status instead.

The whole Darknet work happens inside a single blocking call of the original C code - the `train_detector_ctx` one for the training, for example - so nothing of the C++ side is reached while it runs. The `darknet_adaptor::DarknetContext` class therefore starts an own **stop watcher** thread in it's `DarknetContext::init` method, which polls the very same `ApplicationContext::stop` flag and mirrors it into the `detector_context::stop` field of the original computation context the C loops check every iteration. The watcher thread is joined by the `DarknetContext` destructor.

The watcher exists because **the routine which the OS calls on a signal arrival is allowed to use the async signal safe calls only**, so the handlers of the subcomponent touch the lock free atomic flags and nothing else - the earlier `Application::init_signals` and `Application::handle_sigint` pair used to walk a subscribers vector and to log from the signal context instead, which none of the two is allowed to do there.

### The application pause signals

The `PauseSignalsHandler` class covers the POSIX signals which are asking the process to suspend itself - the `SIGTSTP` one of the Ctrl+Z keys combination, the `SIGTTIN` one of a background process reading from the terminal and the `SIGTTOU` one of a background process writing to it. All the three raise the `ApplicationContext::pause` flag. The `ContinueSignalsHandler` class covers the `SIGCONT` signal of the `fg` and `bg` shell job control commands and lowers the very same flag back.

So the application implementation is expected to poll the `ApplicationContext::pause` flag the same way the stop one is polled and to hold it's work while the flag is raised, instead of leaving the `IApplication::run` method. The flag is an atomic one too.

**Keep in mind that handling the pause signals replaces the OS default disposition of them, so the process is not suspended by the Ctrl+Z keys combination any more** - it is up to the application to react on the raised pause flag. Drop the `PauseSignalsHandler` class from the `SignalsHandler::create_handlers` set whenever the usual shell job control behaviour is preferred, or make the application call `std::raise(SIGSTOP)` on it's own once it is ready to be suspended. Never cover the `SIGTTIN` and the `SIGTTOU` signals without suspending the process: the terminal access of a background job raises them again and again otherwise.

The darknetxx polls no pause flag, so it's `SignalsHandler::create_handlers` method creates neither the `PauseSignalsHandler` nor the `ContinueSignalsHandler` category, and the Ctrl+Z keys combination with the rest of the shell job control keeps working.

The `SIGSTOP` signal is deliberately absent in the covered sets, since **no process is allowed to handle, to block or to ignore it at all** - the OS suspends the process on it's arrival without any notification, so such a pause stays unnoticed by the application.

The pause and the continue categories are the empty ones on the platforms which are not declaring those POSIX signals, like the MS Windows one, so their `ISignalsHandler::install` calls report a failure there, which the facade tolerates as long as any other category is covered.

### The configuration reload signals

The `ReloadSignalsHandler` class covers the `SIGHUP` signal, which the OS sends on a closed terminal and which the daemon control tools send on purpose. It raises the `ApplicationContext::reload` flag, so the application is expected to poll it, to reread it's configuration and to lower the flag back on it's own.

The signal is the conventional configuration reload request of the long living processes, which is why it belongs here and not to the stop category. **The OS default reaction on it terminates the process, so handling it means a closed terminal does not end a long training run any more.** Drop the class from the `SignalsHandler::create_handlers` set, or move the `SIGHUP` number back into the `StopSignalsHandler::get_handled_signals` one, whenever the termination is the expected reaction.

The darknetxx has no configuration to reload, so it does the latter: the `ReloadSignalsHandler` category is not created, and a closed terminal stops the training gracefully, saving the last weights. Start the training with the `nohup` tool to keep it running after the terminal is closed.

### The user defined signals

The `UserSignalsHandler` class covers the `SIGUSR1` and the `SIGUSR2` signals, which the OS reserves for the application own needs and gives no meaning of it's own to. The first one raises the `ApplicationContext::first_user_request` flag and the second one raises the `ApplicationContext::second_user_request` one, so it is the only handler of the component which tells it's signals apart by the number the OS gives to the routine.

**The meaning of both requests is the application own one** - reopening the log files after a rotation and dumping the current training state are the classic ones. The application is expected to poll the flags, to serve the pending request and to lower the flag back on it's own. The flags coalesce, so a burst of the very same signal is a single pending request - count the arrivals inside the routine instead whenever every single one of them has to be served.

The OS default reaction on both signals terminates the process, so handling them keeps the application alive on a `kill -USR1 <pid>` command. The darknetxx serves no user request, so it does not create the `UserSignalsHandler` category and both signals keep terminating the process.

### The ignored signals

The `IgnoredSignalsHandler` class covers the `SIGPIPE` signal, which arrives on a write into a pipe or a socket nobody reads any more. It is the only handler of the component which installs the `SIG_IGN` disposition instead of a routine, so it touches no application context at all and it's `ASignalsHandler::store_context` override is an empty one.

**The OS default reaction on the signal terminates the process silently**, which turns a disconnected peer into an unexplained application death. Ignoring the signal makes the failing write return an `EPIPE` error instead, so the calling code gets the chance to notice the broken connection and to report it, which is why virtually every application which writes into a pipe ignores it.

### The computation traps

The `SIGFPE` signal is **not** a part of the subcomponent, since it reports an invalid computation of a dying process and not an application lifecycle request. The `Application::init_computation_traps` method enables the floating point exceptions with the `feenableexcept` call and traps the signal they arrive as, so an invalid computation becomes noticeable instead of spreading through the network as a `NaN` value. The trap routine reports the exception with the async signal safe `write` call and terminates the process with the `_exit` one.

The Darknet computations rely on the IEEE semantics of the floating point overflows and invalid operations here and there - the logistic activation of a confident logit overflows the `expf` call and the mAP statistics divide zero by zero for a class without detections, for example - so **the traps are a diagnostic which is off by default**. Configure the project with the `-DENABLE_FP_TRAPS=ON` CMake option to enable them.

### Extending the signals handling

1. Add or remove the covered OS signals numbers in the `ASignalsHandler::get_handled_signals` override of the appropriate category handler.
1. Add a brand new signals category by putting an own `PosixSignalsHandler` descendant into the `src/app/signals-handlers/handlers/posix` directory, listing it's new `.cpp` file in the `target_sources` call of that directory `CMakeLists.txt` build file and creating it in the `SignalsHandler::create_handlers` method. The descendant is expected to override the `ASignalsHandler::get_handled_signals`, the `ASignalsHandler::get_signal_disposition` and the `ASignalsHandler::store_context` methods, to keep it's own `std::atomic<ApplicationContext*>` field and **to call the `ISignalsHandler::uninstall` method from it's own destructor**, since neither the `ASignalsHandler` nor the `PosixSignalsHandler` one is able to reach the pure virtual `ASignalsHandler::store_context` method.
1. Cover a platform family the POSIX signals do not fit by putting an own `ASignalsHandler` descendant - a family base of the `PosixSignalsHandler` kind - into a new `src/app/signals-handlers/handlers/<platform>` directory, registering that directory with an `add_subdirectory` call in the `src/app/signals-handlers/handlers/CMakeLists.txt` build file and deriving the categories of that family from it. The facade tolerates a category which covers nothing on the current platform, so the families of every supported platform may be created side by side in the `SignalsHandler::create_handlers` method.
1. Implement an own `ISignalsHandler` descendant whenever a completely different reaction is needed, put it into the `src/app/signals-handlers` directory, list it's new `.cpp` file in the `target_sources` call of the `src/app/signals-handlers/CMakeLists.txt` build file and return it from an overridden `ApplicationFactory::create_default_signals_handler` method.
1. Add a new context flag of a new category into the [`ApplicationContext`](/src/app/ApplicationContext.h) structure as an `std::atomic_bool` one next to the `stop`, the `pause`, the `reload` and the user request ones, since **a signal routine touches the lock free atomics only** - a plain `bool` field is not one of them.
1. Put the new class' unit test into the `src/app/signals-handlers/tests/unit/<YourSignalsHandler>` directory, or into the `src/app/signals-handlers/handlers/<platform>/tests/unit/<YourSignalsHandler>` one for a category handler, and register that directory with an `add_subdirectory` call in the appropriate `tests/unit/CMakeLists.txt` build file. The subcomponent mocks directories are exposed to the whole `src/app` component by it's `src/app/CMakeLists.txt` build file as the `${SIGNALS_HANDLERS_MOCKS_ROOT}` and the `${POSIX_SIGNALS_HANDLERS_MOCKS_ROOT}` CMake variables, the same way the component own mocks are exposed as the `${APP_MOCKS_ROOT}` one.

**Keep in mind that the routine which the OS calls on a signal arrival is allowed to use the async signal safe calls only**, so every one of them - the `StopSignalsHandler::stop_context`, the `PauseSignalsHandler::pause_context`, the `ContinueSignalsHandler::continue_context`, the `ReloadSignalsHandler::reload_context` and the `UserSignalsHandler::request_context` ones - touches the lock free atomics and does no logging, no memory allocation and no locking at all. The `IgnoredSignalsHandler` class installs no routine at all, since the `SIG_IGN` value is a valid `ASignalsHandler::get_signal_disposition` answer of its own.
