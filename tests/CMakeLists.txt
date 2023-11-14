project(tests)

add_subdirectory(googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(RunTests tests.cpp
        ../src/ai/genetic/eval.cpp
        ../src/ai/genetic/Agent.cpp
        ../src/game/Board.cpp
)

target_link_libraries(RunTests gtest gtest_main)
