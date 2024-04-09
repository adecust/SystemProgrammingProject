cmake_minimum_required(VERSION 3.26)
project(proj C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_FLAGS "-lncurses")

add_executable(proj main.c)
