# Lain-8
The first of a family of emulators I'll be making.  This is the CHIP 8 emulator `Lain-8`

## Build

You’ll need Git, CMake 3.31 or newer, a build tool such as Make or Ninja, and a C++23 compiler and standard library that support `<print>` (for example, GCC 14 with libstdc++). SDL3 is built from the Git submodule. CMake fetches GoogleTest from GitHub during configuration.

Run these commands from the repository root. The SDL submodule uses GitHub SSH, so you’ll need SSH access to GitHub to initialize it with the first command.

```sh
git submodule update --init --recursive
cmake -S . -B build
cmake --build build
```

## Run the tests

After building, run the CHIP-8 instruction tests with CTest.

```sh
ctest --test-dir build --output-on-failure --no-tests=error
```

A passing run reports `100% tests passed`. Use `cmake --build build --target lain8-tester` to rebuild just the test executable.

## Load a ROM from the interactive shell

Start the emulator from the repository root in a graphical desktop session.

```sh
./build/bin/lain8
```

1. At the `>` prompt, type `LOAD` and press Enter. Shell commands are case-sensitive.
2. At the file path prompt, enter `roms/ibm.ch8` to try the included IBM logo ROM. For another ROM, enter its absolute path or a path relative to the directory where you launched Lain-8. Enter the path without quotes, even if it contains spaces.
3. Lain-8 prints a hex dump of the ROM and opens the emulator window. For ROMs that use the CHIP-8 keypad, focus the emulator window and use the keys below.
4. Close the emulator window to return to the shell. Enter `LOAD` to start another ROM or `QUIT` to exit Lain-8.

| CHIP-8 keys | Keyboard keys |
| --- | --- |
| `0` `1` `2` `3` | `5` `6` `7` `8` |
| `4` `5` `6` `7` | `T` `Y` `U` `I` |
| `8` `9` `A` `B` | `G` `H` `J` `K` |
| `C` `D` `E` `F` | `B` `N` `M` `,` |

`PAUSE` and `DEBUG` are currently placeholders.

