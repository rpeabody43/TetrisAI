cmake_minimum_required (VERSION 3.8)

project ("TetrisAI")

# So clangd in neovim can see libraries
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)

# Libraries
add_subdirectory(lib)
# Source code
add_subdirectory(src)
# Unit tests
add_subdirectory(tests)
