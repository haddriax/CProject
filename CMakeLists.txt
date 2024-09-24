cmake_minimum_required(VERSION 3.11)

project(SDL2Project C)
set(CMAKE_C_STANDARD 99)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(SDL2_DIR "C:/libraries/SDL2-devel-2.30.7-mingw/x86_64-w64-mingw32")

if(MINGW)
    message(STATUS "Using MinGW for compilation")
else()
    message(FATAL_ERROR "This project is configured to use MinGW. Make sure you are compiling with MinGW.")
endif()

include_directories("${SDL2_DIR}/include/SDL2")

set(SDL2_LIBRARY_PATH "${SDL2_DIR}/lib")
set(SDL2_LIB "${SDL2_LIBRARY_PATH}/libSDL2.a")
set(SDL2_MAIN_LIB "${SDL2_LIBRARY_PATH}/libSDL2main.a")

if(NOT EXISTS ${SDL2_LIB})
    message(FATAL_ERROR "SDL2 library not found at: ${SDL2_LIB}")
endif()

if(NOT EXISTS ${SDL2_MAIN_LIB})
    message(FATAL_ERROR "SDL2 main library not found at: ${SDL2_MAIN_LIB}")
endif()

set(SOURCE_FILES
        src/main.c
        src/model/model.c
        src/vue_controller/vue_controller.c
        src/model/model.h
        src/vue_controller/vue_controller.h
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} "${SDL2_MAIN_LIB}" "${SDL2_LIB}" setupapi)

target_link_libraries(${PROJECT_NAME}
        kernel32
        user32
        gdi32
        winspool
        shell32
        ole32
        oleaut32
        uuid
        comdlg32
        advapi32
        Imm32
        Winmm
        Version
)


# Copy the SDL2.dll to the build directory after the executable is built.
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL2_DIR}/bin/SDL2.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Copying SDL2.dll to output directory."
)

# Set build options (warnings as errors).
if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
    # MinGW flags here ...
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

set(CMAKE_C_FLAGS_DEBUG "-g -O0")
set(CMAKE_C_FLAGS_RELEASE "-O3")

message(STATUS "CMake configuration completed. Ready to build ${PROJECT_NAME}.")
