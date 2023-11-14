cmake_minimum_required (VERSION 3.8)

project ("TetrisAI")

# So clangd in neovim can see libraries
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Add SDL Main Library
find_package(SDL2 REQUIRED)
# Add SDL Text Library
find_package(SDL2_ttf REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)

# Sources that every target needs
set(COMMON_SOURCES
        src/app/App.cpp
        src/game/Board.cpp
        src/app/gfx/Window.cpp
        src/app/HumanPlayer.cpp
)
# Human Game
add_executable(Human src/main_human.cpp ${COMMON_SOURCES})
# Genetic Algorithm Game
add_executable(GeneticAlgo
        src/main_genetic.cpp
        src/ai/genetic/eval.cpp
        src/ai/genetic/Agent.cpp
        src/ai/genetic/train.cpp
        ${COMMON_SOURCES}
)

# Include all headers
include_directories(PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
# SDL headers
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIR})

# Link SDL to targets
target_link_libraries(Human ${SDL2_LIBRARIES} SDL2_ttf::SDL2_ttf)
target_link_libraries(GeneticAlgo ${SDL2_LIBRARIES} SDL2_ttf::SDL2_ttf)

# Unit tests
add_subdirectory(tests)