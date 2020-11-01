cmake_minimum_required(VERSION 3.18)
project(TP3 C)

set(CMAKE_C_STANDARD 11)

include_directories(include)

add_executable(ultra-cp src/main.c src/ultra-ls.c src/ultra-cp.c)
target_link_libraries(ultra-cp m)