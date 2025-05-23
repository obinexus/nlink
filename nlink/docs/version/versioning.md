# Implementation Guide for NexusLink Version Management

## Overview

This guide outlines the proper structure for implementing version management in the NexusLink system to avoid header conflicts and maintain a clean code structure.

## File Structure

```
include/
├── nlink/
│   ├── core/
│   │   ├── versioning/
│   │   │   └── nexus_version.h      # Core version definitions
│   │   └── common/
│   │       ├── types.h
│   │       ├── result.h
│   │       └── nexus_core.h
│   └── cli/
│       ├── commands/
│       │   ├── version.h            # CLI command interface
│       │   └── version_utils.h      # CLI utilities for version
│       ├── command.h
│       ├── command_params.h
│       └── command_router.h
src/
├── core/
│   └── versioning/
│       └── nexus_version.c          # Implementation of core version functions
└── cli/
    └── commands/
        ├── version.c                # Implementation of version command
        └── version_utils.c          # Implementation of CLI version utilities
```

## Header Hierarchy

1. **`nexus_version.h`**: Contains the primary definitions of version structures and functions.
2. **`version_utils.h`**: Includes `nexus_version.h` and provides CLI-specific utilities.
3. **`version.h`**: Includes `version_utils.h` and defines the version command interface.

## Inclusion Pattern

Files should include headers in the following order:

1. System headers (e.g., `<stdio.h>`)
2. Project-level headers (e.g., `"nlink/core/common/types.h"`)
3. Component-specific headers

## Best Practices

### Forward Declarations

- Use forward declarations ONLY when necessary to break circular dependencies
- Don't forward declare structures that are fully defined in included headers

### Type Definitions

- Define types only once, in the most appropriate header
- Use typedefs consistently (e.g., `typedef struct NexusVersion NexusVersion;`)

### Interface Separation

- Core library (nexus_version.h): Provides direct version manipulation
- CLI utilities (version_utils.h): Provides CLI-friendly wrappers
- Command interface (version.h): Defines command structure and registration

## Implementation Notes

The updated structure resolves several issues:

1. Eliminates duplicate type definitions by ensuring a clear header hierarchy
2. Removes conflicting function declarations by using proper includes
3. Separates CLI-specific utilities from core version functionality
4. Maintains backward compatibility with existing code

## Build System Integration

Add the following components to the CMake build:

```cmake
# Core versioning library
add_library(nexus_versioning
    src/core/versioning/nexus_version.c
)

target_include_directories(nexus_versioning
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

# CLI versioning utilities
add_library(nexus_cli_versioning
    src/cli/commands/version_utils.c
)

target_link_libraries(nexus_cli_versioning
    PUBLIC
        nexus_versioning
)
```

## Testing

Test the version management system thoroughly, focusing on:

1. Version parsing edge cases
2. Constraint validation
3. Version comparison
4. CLI integration