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
set(COMMON_SOURCES
        src/app/App.cpp
        src/game/Board.cpp
        src/app/gfx/Window.cpp
        src/app/HumanPlayer.cpp
)
add_executable(Human src/main_human.cpp ${COMMON_SOURCES})
add_executable(GeneticAlgo
        src/main_genetic.cpp
        src/ai/genetic/eval.cpp
        src/ai/genetic/Agent.cpp
        src/ai/genetic/train.cpp
        ${COMMON_SOURCES}
)

include_directories(PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIR})
target_link_libraries(Human ${SDL2_LIBRARIES} SDL2_ttf::SDL2_ttf)
target_link_libraries(GeneticAlgo ${SDL2_LIBRARIES} SDL2_ttf::SDL2_ttf)
