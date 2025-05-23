# NexusLink (nlink) Symbol Management System

## Overview
NexusLink is a dynamic symbol management system that provides lazy-loading functionality and JSON-based metadata handling for components. This tool allows for efficient runtime loading of components and functions when needed.

## Prerequisites
- Linux environment
- GCC compiler
- Bash shell

## Directory Structure
- `src/` - Source files
- `include/` - Header files
- `build/` - Output directory for compiled files
- `build/components/` - Component libraries

## Build System

### Requirements
The build system requires the following header files:
- `nexus_json.h` - JSON parsing and handling
- `nexus_metadata.h` - Component metadata management
- `nexus_symbols.h` - Symbol management
- `nexus_lazy.h` - Lazy loading functionality

### Build Script Usage
```
# Standard release build
./build.sh

# Clean build with debug symbols and run tests
./build.sh --clean --debug --test

# Build proof-of-concept only
./build.sh --poc

# Full build with components and run the demo
./build.sh --with-components --run-demo
```

