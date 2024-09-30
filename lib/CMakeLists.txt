add_subdirectory(sdl3)
set(SDLTTF_VENDORED ON)
add_subdirectory(sdl_ttf)

add_library(lib INTERFACE)
target_include_directories(lib PRIVATE)

target_link_libraries(lib INTERFACE SDL3::SDL3)
target_link_libraries(lib INTERFACE SDL3_ttf::SDL3_ttf) 
