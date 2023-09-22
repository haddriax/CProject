cmake_minimum_required(VERSION 3.26)
project(ProjectC_oleil C)

set(CMAKE_C_STANDARD 99)

add_executable(ProjectC_oleil source_code/main.c)

target_link_libraries(ProjectC_oleil SDL2main SDL2)