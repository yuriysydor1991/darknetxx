## Optional for the code formatting

To auto format code using available `clang-format` target the `clang-format` code formatter must be installed (GNU/Linux based):

```
sudo apt install -y clang-format
```

The project code style needs the `clang-format` 16 or newer. Install the versioned package, for example `clang-format-16`, on the systems which ship an older default one - the `clang-format` target finds it by it's name.

Examine configuring details in the [Enabling and performing code formatting target](/doc/sections/en_US/5-6-enabling-and-performing-code-formatting-target.md) subsection.
