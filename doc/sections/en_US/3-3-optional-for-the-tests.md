## Optional for the tests

If necessary to build and execute available tests install GTest or ensure available Internet access in order to project make it available by itself (GNU/Linux based):

```
sudo apt install -y googletest
```

The tests of the network weights files read the gzip compressed JSON documents the [darknetxx-datadrivers](https://github.com/yuriysydor1991/darknetxx-datadrivers) library writes on their own, so they require the nlohmann JSON library (the system one or the one the configure fetches, see the [Enabling the nlohmann JSON library](/doc/sections/en_US/5-18-enabling-the-nlohmann-json-library.md) section) and the zlib development files of the system (see [The zlib library](/doc/sections/en_US/5-23-enabling-the-zlib-library.md) section):

```
sudo apt install -y nlohmann-json3-dev zlib1g-dev
```

Examine the [Project tests](/doc/sections/en_US/4-4-project-tests.md) for more info.
