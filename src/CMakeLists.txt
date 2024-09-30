# Sources that every target needs
set(COMMON_SOURCES
        app/App.cpp
        game/Board.cpp
        app/gfx/Window.cpp
        app/HumanPlayer.cpp
)
# Human Game
add_executable(
    Human 
    main_human.cpp 
    ${COMMON_SOURCES}
)
target_link_libraries(Human PRIVATE lib)

# Genetic Algorithm Game
add_executable(
    GeneticAlgo
    main_genetic.cpp
    ai/genetic/eval.cpp
    ai/genetic/Agent.cpp
    ${COMMON_SOURCES}
)
target_link_libraries(GeneticAlgo PRIVATE lib)

include_directories(PRIVATE)
