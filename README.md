# Tetris AI
Making a computer play Tetris better than most people can.

## How to build
(so I remember)

1. Clone repo
2. Install git modules
```
git submodule update --init --recursive
```
3. Create build directory and run CMake / ninja
```
mkdir build && cd build
cmake -S .. -B . -G "Ninja"
ninja
```
4. Copy dlls into root build (Windows only)
```
cp lib\sdl3\SDL3.dll .
cp lib\sdl_ttf\SDL3_ttf.dll .
```
5. Copy font file into build
```
cp '..\Retro Gaming.ttf' .
```
6. Run one of the executables

```
# Using human game as an example
.\src\Human
```
