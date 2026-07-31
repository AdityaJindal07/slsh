# Generic Command Engine (SAT-OS / OBDH)

A lightweight command engine written in **C99** for embedded systems. The project provides a generic command dispatcher, a character-by-character shell parser, and platform-independent interfaces that can be used on Linux terminals as well as FreeRTOS-based systems.

---

# Features

- Generic command registration using callback functions
- Character-by-character command parsing
- Built-in commands (`help`, `ping`, `echo`)
- Linux terminal support
- FreeRTOS UART adapter
- Multi-subsystem simulation (OBDH, EPS, TTC)
- Cross-platform design (Linux & Windows compatible)

---

# Architecture

```text
                 +------------------------------+
                 |    User / UART / Terminal    |
                 +--------------+---------------+
                                |
                                v
                    Character-by-Character Input
                                |
                                v
                   +---------------------------+
                   |      Shell Parser         |
                   |  - Buffer characters      |
                   |  - Handle backspace       |
                   |  - Tokenize input         |
                   +-------------+-------------+
                                 |
                                 v
                   +---------------------------+
                   | Generic Command Engine    |
                   |  Command Lookup           |
                   |  Callback Dispatcher      |
                   +-------------+-------------+
                                 |
        ----------------------------------------------------
        |                      |                          |
        v                      v                          v
   Built-in Commands      OBDH Commands            User Commands
(help, ping, echo)    (telemetry, beacon...)   (Custom callbacks)
```

---

# Building the Project

## Linux

```bash
cmake -S . -B build
cmake --build build
```

Run:

```bash
./build/cmd_engine_example
```

or

```bash
./build/subsystem_sim
```

## Windows

```bash
cmake -S . -B build
cmake --build build --config Debug
```

Executables are generated inside:

```
build/Debug/
```

---

# Adding a New Command

Adding a new command requires four simple steps:

1. Create a command handler function.

```c
void my_command(void *context)
{
    shell_printf("Hello from my command!\n");
}
```

2. Register the command.

```c
cmd_register("my_command", my_command);
```

3. Call the registration function during initialization.

4. Execute it from the shell.

```
slsh> my_command
Hello from my command!
```

---

# Current Limitations

- Maximum of 32 registered commands.
- Maximum command length is 31 characters.
- Input buffer size is 128 bytes.
- Maximum of 16 command arguments.
- Command lookup uses linear search.
- Commands execute synchronously.
- Quoted string arguments are not supported.

---

# Demo Video

Demo Video:

(https://drive.google.com/file/d/1os_iNwc2MbHalnLwTvJ95h-hjNm9aCp-/view?usp=drive_link)

The demonstration includes:

- Character-by-character input
- Typo correction using Backspace
- `help` command
- Built-in commands (`ping`, `echo`)
- Subsystem commands (`obdh_status`, `telemetry_dump`, `vbat`, etc.)
- Invalid command handling

---

# Repository Structure

```
generic_cmd_engine/
│
├── core/           Command engine
├── parser/         Character parser
├── platform/       Linux & FreeRTOS adapters
├── subsystems/     Satellite subsystem simulation
├── tests/          Unit tests
└── README.md
```

---

# Future Improvements

- Faster command lookup using hash tables.
- Dynamic command registration.
- Command history support.
- Auto-completion.
- Quoted string parsing.
- RTOS integration with actual hardware UART.