# FreeRTOS integration boundary

Keep FreeRTOS, TTC, EPS, shell, and OBDH-specific headers in this directory.
Each adapter should parse its input into a source-specific context, then call
the dependency-free core with `cmd_engine_execute(command_id, &context)`.

The core must not include files from this directory.
