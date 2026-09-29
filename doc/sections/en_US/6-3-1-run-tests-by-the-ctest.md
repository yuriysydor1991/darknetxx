### Run tests by the ctest

If enabled by the developer through the `ENABLE_UNIT_TESTS` variable and successfully build, the one may run all available test by the `ctest` command from a project build directory (GNU/Linux based):

```
# run from the project build directory

ctest
```

Alternatively, run the `ctest` command from any location by specifying the test dir (GNU/Linux based):

```
ctest --tests-dir /path/to/the/project/build/directory
```

The `ENABLE_COMPONENT_TESTS` and the `ENABLE_INTEGRATION_TESTS` variables make the component and the integration tests available the same way. A part of the tests may be selected by the regular expression of their names, for example the integration tests alone (see the [Enabling integration testing](/doc/sections/en_US/5-3-3-enabling-integration-testing.md) section):

```
# run from the project build directory

ctest -R "^ITEST_" --output-on-failure
```
