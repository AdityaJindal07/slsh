cmake_minimum_required(VERSION 3.16)

project(generic_cmd_engine VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(cmd_engine_core STATIC
    core/src/cmd_engine.c
    core/src/builtin_cmds.c
    parser/src/shell_parser.c
    platform/linux/src/linux_io.c
    platform/linux/src/linux_console.c
)

target_include_directories(cmd_engine_core
    PUBLIC core/include
    PUBLIC parser/include
    PUBLIC platform/include
    PUBLIC platform/linux/include
    PUBLIC platform/freertos/include
    PUBLIC subsystems/include
)

# Compiler-specific warning options
if (MSVC)
    target_compile_options(cmd_engine_core PRIVATE /W4)
else()
    target_compile_options(cmd_engine_core PRIVATE -Wall -Wextra -Wpedantic)
endif()


# Linux Console Example
add_executable(cmd_engine_example
    platform/linux/src/example_main.c
    platform/linux/src/linux_console.c
)
target_link_libraries(cmd_engine_example PRIVATE cmd_engine_core)

# FreeRTOS Adapter Target
add_executable(freertos_adapter_test
    platform/freertos/src/freertos_uart_adapter.c
    tests/test_freertos_adapter.c
)
target_link_libraries(freertos_adapter_test PRIVATE cmd_engine_core)

# FreeRTOS Interactive Shell
add_executable(freertos_shell
    platform/freertos/src/freertos_uart_adapter.c
    platform/freertos/src/freertos_shell_main.c
)
target_link_libraries(freertos_shell PRIVATE cmd_engine_core)

# Subsystem Simulation Target (OBDH, EPS, TTC)
add_executable(subsystem_sim
    subsystems/simulation/src/subsystem_sim.c
)
target_link_libraries(subsystem_sim PRIVATE cmd_engine_core)

enable_testing()

add_executable(test_cmd_engine
    tests/test_cmd_engine.c
)
target_link_libraries(test_cmd_engine PRIVATE cmd_engine_core)

add_test(NAME cmd_engine_dispatch COMMAND test_cmd_engine)
add_test(NAME freertos_adapter_test COMMAND freertos_adapter_test)
add_test(NAME subsystem_sim COMMAND subsystem_sim)
