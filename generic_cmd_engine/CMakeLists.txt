cmake_minimum_required(VERSION 3.16)

project(generic_cmd_engine VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(cmd_engine_core STATIC
    core/src/cmd_engine.c
    core/src/builtin_cmds.c
)

target_include_directories(cmd_engine_core
    PUBLIC core/include
    PUBLIC parser/include
    PUBLIC platform/linux/include
)

target_compile_options(cmd_engine_core PRIVATE
    -Wall -Wextra -Wpedantic
)

target_sources(cmd_engine_core PRIVATE
    parser/src/shell_parser.c
)

add_executable(cmd_engine_example
    platform/linux/src/example_main.c
)
target_link_libraries(cmd_engine_example PRIVATE cmd_engine_core)

enable_testing()

add_executable(test_cmd_engine
    tests/test_cmd_engine.c
)
target_link_libraries(test_cmd_engine PRIVATE cmd_engine_core)
add_test(NAME cmd_engine_dispatch COMMAND test_cmd_engine)
