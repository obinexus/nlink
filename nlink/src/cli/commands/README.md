# NexusLink CLI Implementation

This directory contains the implementation of the command-line interface (CLI) for the NexusLink system, a dynamic component linkage system for efficient binary sizes and runtime loading.

## Architecture

The CLI follows a command pattern architecture where each command is implemented as a separate module. The main components are:

- `cli.h/cli.c`: Main CLI interface
- `command_registry.c`: Command registration and lookup
- `commands/*.h/commands/*.c`: Individual command implementations
- `main.c`: Entry point for the CLI

## Commands

The following commands are currently implemented:

- `load`: Load a component into the current context
- `version`: Display version information
- `minimal`: Execute commands in minimal syntax format

Additional commands planned for implementation:

- `build`: Build a component from source
- `stats`: Display statistics about loaded components

## Minimal Mode

NexusLink CLI supports a "minimal mode" which provides a more concise syntax for common operations. In minimal mode, you can use patterns like:

```
nlink component@version:function=args
```

For example:
- `nlink logger`: Load the logger component
- `nlink logger@1.2.3`: Load logger version 1.2.3
- `nlink logger:log`: Load logger and call log function
- `nlink logger@1.2.3:log=Hello World`: Load logger 1.2.3, call log with "Hello World"

Minimal mode can be enabled:
- Using the `--minimal` or `-m` command-line flag
- Setting the `NEXUS_MINIMAL` environment variable

## Building

To build the CLI, run:

```bash
make
```

This will create the `nlink` executable in the `../build/cli` directory.

To install the CLI to the lib/bin directory:

```bash
make install
```

## Usage

```bash
# Show help
./nlink --help

# Show version information
./nlink version

# Load a component
./nlink load liblogger.so

# Load a component with version constraint
./nlink load liblogger.so --version "^1.0.0"

# Load using minimal syntax
./nlink --minimal "logger@1.0.0:log=Hello World"

# Or in minimal mode
./nlink logger@1.0.0:log=Hello World
```

## Adding New Commands

To add a new command:

1. Create `commands/your_command.h` and `commands/your_command.c` files
2. Define a `NexusCommand` structure in your source file
3. Implement the required functions (execute, print_help, parse_args)
4. Register the command in `main.c`

Example command structure:

```c
NexusCommand your_command = {
    .name = "your_command",
    .short_name = "y",
    .description = "Description of your command",
    .help = "Detailed help for your command.",
    .execute = your_command_execute,
    .print_help = your_command_print_help,
    .parse_args = your_command_parse_args,
    .data = NULL
};
```

## License

Copyright © 2025 OBINexus Computing - All rights reserved.