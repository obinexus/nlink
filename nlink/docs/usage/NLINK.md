# NexusLink CLI

## Overview

NexusLink CLI (`nlink.exe`) is a command-line interface for the NexusLink dynamic component linkage system. It provides a modular, extensible architecture for efficiently managing binary sizes and runtime loading of components.

## Core Features

- **Dynamic Component Loading**: Load components at runtime with version constraints
- **Component Minimization**: Reduce binary size through state machine minimization
- **Pipeline Processing**: Create and manage multi-pass processing pipelines
- **Symbol Management**: Three-tier symbol registry with reference counting
- **Minimal Syntax Mode**: Concise command syntax for common operations

## Architecture

NexusLink CLI is built on a modular architecture with the following key components:

### Command System

The command system uses a pattern-based routing mechanism that supports:

- Regular expression pattern matching
- Parameter extraction from command inputs
- Command handler delegation
- Help text generation

### Command Registry

Commands are registered in a central registry that handles:

- Command lookup by name
- Command execution
- Parameter passing
- Help text display

### Core Modules

| Module | Description |
|--------|-------------|
| `nexus_cli` | Main CLI interface and command processing |
| `nexus_commands` | Command implementations |
| `nexus_common` | Common utilities and structures |
| `nexus_loader` | Dynamic library loading functionality |
| `nexus_minimizer` | Binary size optimization |
| `nexus_symbols` | Symbol registry and management |
| `nexus_versioning` | Semantic versioning support |

## Command Reference

### Core Commands

- `load`: Load a component dynamically
  ```
  nlink load <component> [version <version>] [path <path>]
  ```

- `minimal`: Execute commands in minimal syntax format
  ```
  nlink minimal <component>[@version][:function][=args]
  ```

- `minimize`: Minimize a component using state machine minimization
  ```
  nlink minimize <component> [level <1-3>] [with|without boolean] [output <file>]
  ```

- `pipeline`: Create and manage processing pipelines
  ```
  nlink pipeline create [mode=<auto|single|multi>] [optimization=<enabled|disabled>]
  nlink pipeline add-stage <name>
  nlink pipeline execute
  ```

- `version`: Display version information
  ```
  nlink version [--detailed] [--json]
  ```

### Utility Commands

- `help`: Display help information
  ```
  nlink help [command]
  ```

- `list`: List available components
  ```
  nlink list [category]
  ```

- `stats`: Display system statistics
  ```
  nlink stats [memory|components]
  ```

- `parse`: Parse input file through lexer-parser-AST pipeline
  ```
  nlink parse <input_file> [to <output_file>]
  ```

## Minimal Syntax Mode

NexusLink supports a concise syntax for common operations:

```
nlink component[@version][:function][=args]
```

Examples:
- `nlink logger` - Load the logger component
- `nlink logger@1.2.3` - Load logger version 1.2.3
- `nlink logger:log` - Load logger and call log function
- `nlink logger@1.2.3:log=Hello World` - Load logger 1.2.3, call log with "Hello World"

Minimal mode can be enabled using the `--minimal` flag or by setting the `NEXUS_MINIMAL` environment variable.

## Multi-Pass Pipeline System

The multi-pass pipeline system allows creation of complex processing workflows:

1. Create a pipeline with specific execution mode
2. Add processing stages to the pipeline
3. Execute the pipeline with input data
4. Analyze results and statistics

Pipeline execution modes:
- `single-pass`: Execute each stage once in sequence
- `multi-pass`: Execute stages iteratively with feedback loops
- `auto`: Automatically determine the best execution strategy

## Symbol Management

NexusLink provides a sophisticated symbol management system:

- **Global Symbols**: Core system symbols available to all components
- **Imported Symbols**: Symbols imported by a component
- **Exported Symbols**: Symbols exported by a component for others to use

Symbol types supported include functions, variables, types, constants, macros, structures, enumerations, and unions.

## Usage Examples

### Loading Components

```
nlink load core
nlink load minimizer version 1.2.3
nlink load logger path /custom/path
```

### Pipeline Processing

```
nlink pipeline create mode=multi optimization=enabled
nlink pipeline add-stage tokenizer
nlink pipeline add-stage parser
nlink pipeline add-stage optimizer
nlink pipeline execute
```

### Component Minimization

```
nlink minimize mycomponent.so
nlink minimize mycomponent.so level 3
nlink minimize mycomponent.so with boolean output minimized.so
```

### Interactive Mode

```
nlink --interactive
nexus> load core
nexus> pipeline create
nexus> pipeline add-stage tokenizer
nexus> pipeline execute
nexus> exit
```

## Script Execution

NexusLink supports executing commands from script files:

```
nlink --execute script.nlink
```

Example script file:
```
# Load components
load core
load parser version 2.1.0

# Create and configure pipeline
pipeline create mode=multi
pipeline add-stage tokenizer
pipeline add-stage parser
pipeline add-stage optimizer

# Execute pipeline
pipeline execute
```

## Building from Source

```
mkdir build && cd build
cmake ..
make
```

## Environment Variables

- `NEXUS_MINIMAL`: Enable minimal syntax mode when set to any value
- `NEXUS_LOG_LEVEL`: Set logging level (debug, info, warning, error)
- `NEXUS_COMPONENT_PATH`: Additional paths to search for components

## License

Copyright © 2025 OBINexus Computing - All rights reserved