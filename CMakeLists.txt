cmake_minimum_required(VERSION 3.17)
project(bytepusher C)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/modules")
find_package(SDL2 REQUIRED)
INCLUDE_DIRECTORIES(SYSTEM ${SDL2_INCLUDE_DIR})

set(CMAKE_C_STANDARD 11)

add_executable(bytepusher main.c)
target_link_libraries(bytepusher ${SDL2_LIBRARY})