# [Chip-8](https://en.wikipedia.org/wiki/CHIP-8) emulator implementation in C++
Bare bones emulator capable of passing test from [chip8-test-suite](https://github.com/Timendus/chip8-test-suite) and running some game roms


## Dependencies
This project uses [raylib](https://github.com/raysan5/raylib) for handling graphics and keyboard input so you'll need to have it installed if you intend to build the emulator from source

## How to build (with provided shell script)
Command provided below should create an executable called `chip8` in the project directory (requires g++)

```
./comp.sh
```

## How to build (manually)
Take your favorite C++ compiler and compile the project from the 2 source files
- `main.cpp`
- `chip8.cpp`

Also remember to link against raylib.

example compilation command for g++

```
g++ main.cpp chip8.cpp -lraylib -o chip8 
```

## How to run
Call the generated executable and pass the rom file you want to emulate as the first argument

```
./chip8 ./test-roms/danm8ku.ch
```

## Notes
Quirks tested by [5-quirks.ch8](test-roms/5-quirks.ch8) are set up to correctly emulate [danm8ku.ch8](test-roms/danm8ku.ch8)
Other games might not necessarily work correctly

## Attributions
[Timendus](https://github.com/Timendus) - Author of the Chip8 test suite that was used throughout the development

[buffis](https://github.com/buffis) - Author of the game [Danm8ku](https://github.com/buffis/danm8ku) which was used to gauge the completeness of this project
