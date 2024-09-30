# Tetris AI
Making a computer play Tetris better than most people can.

## How to build
(so I remember)

**None of this is platform specific**
1. Clone repo
2. Install git modules
```
TetrisAI> git submodule update --init --recursive
```
3. Create build directory and run CMake / ninja
```
TetrisAI> mkdir build && cd build
TetrisAI\build> cmake -S .. -B . -G "Ninja"
TetrisAI\build> ninja
```
4. Copy dlls into root build
```
TetrisAI\build> cp lib\sdl3\SDL3.dll .
TetrisAI\build> cp lib\sdl_ttf\SDL3_ttf.dll .
```
5. Copy font file into build
```
TetrisAI\build> cp '..\Retro Gaming.ttf' .
```
6. Run one of the executables

```
# Using human game as an example
TetrisAI\build> .\src\Human
```
