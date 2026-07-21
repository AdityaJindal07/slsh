# Generic Command Engine

Small, dependency-free C command dispatcher intended to sit beneath TTC,
EPS, shell, or scheduler adapters.

## Layout

- `core/`: ISO C engine. It has no platform, RTOS, driver, or subsystem
  dependencies.
- `platform/linux/`: host example and test support.
- `platform/freertos/`: integration boundary for FreeRTOS firmware adapters.
- `tests/`: host-side unit tests.

An adapter parses its source input, prepares a caller-owned context, and calls
`cmd_engine_execute()`. The selected handler owns interpretation of that
context.

## Host build

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build
```
