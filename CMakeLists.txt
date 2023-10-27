# CMakeList.txt : CMake project for TetrisAI, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("TetrisAI")

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Add SDL Main Library
find_package(SDL2 REQUIRED)
# Add SDL Text Library
find_package(SDL2_ttf REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)

# Add source to this project's executable.
set(SOURCES src/main.cpp src/App.cpp src/game/Board.cpp  src/ai/eval.cpp src/gfx/Window.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES} SDL2_ttf::SDL2_ttf)
