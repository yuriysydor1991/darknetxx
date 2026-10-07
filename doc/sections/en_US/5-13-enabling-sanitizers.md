## Enabling sanitizers

In order to enable the sanitizers to check application at the runtime, configure project with the command:

```
cmake -S . -B build -DENABLE_SANITIZERS=ON
```

For a general purpose sanitizers. See the `cmake/darknetxx-sanitizers.cmake` to examine available list of the sanitizers.

Alternatively, enable the threads sanitizers with the command:


```
cmake -S . -B build -DENABLE_SANITIZERS_THREADS=ON
```

to check the possible data rises of the newly introduced multithreaded application code.

Do not combine the `ENABLE_SANITIZERS` and `ENABLE_SANITIZERS_THREADS`.

The threads sanitizer build turns the `ENABLE_OPENMP` option off: the ThreadSanitizer can't see the synchronization inside the uninstrumented OpenMP runtime (`libgomp`), so it would report the OpenMP worker threads of every parallel loop as the data races. The application threads - the stop watcher, the data loading and the training ones - are still checked.

Any findings will be printed into the application' stdout/stderr after the build and run.
