# CMakeList.txt : CMake project for TetrisAI, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("TetrisAI")


find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Add source to this project's executable.
add_executable (TetrisAI "src/main.cpp"  "include/Board.hpp")

target_link_libraries(TetrisAI SDL2::SDL2 SDL2::SDL2main)

# TODO: Add tests and install targets if needed.
