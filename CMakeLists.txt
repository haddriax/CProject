cmake_minimum_required(VERSION 3.11)
project(ProjetSDL2)

set(CMAKE_C_STANDARD 99)

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

add_executable(Application
        ProjetC_oleil/source_code/main.c
        ProjetC_oleil/source_code/vue_controller/vue_controller.c
        ProjetC_oleil/source_code/vue_controller/vue_controller.h
        ProjetC_oleil/source_code/model/model.c
        ProjetC_oleil/source_code/model/model.h)

target_link_libraries(Application ${SDL2_LIBRARIES} -lm)
