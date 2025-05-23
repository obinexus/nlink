# NexusLink Enhanced Command System Implementation Guide

This guide explains how to implement and use the enhanced command system with parameter extraction capabilities for the NexusLink project.

## Overview

The enhanced command system adds parameter extraction capabilities to the NexusLink CLI, allowing commands to extract and use parameters from command input strings using pattern matching. This feature is particularly useful for implementing more sophisticated command-line interfaces.

## Implementation Steps

### 1. Update Header Files

The following header files need to be created or updated:

- `command.h` - Updated with `NexusCommandEx` structure and new function prototypes
- `command_params.h` - New file for parameter handling functionality
- `command_router.h` - Updated with parameter-aware routing functions

### 2. Update Implementation Files

Implement or update the following source files:

- `command.c` - Updated with new function implementations
- `command_params.c` - New file implementing parameter handling
- `command_router.c` - Updated with parameter extraction capabilities

### 3. Update Existing Commands

Existing commands can be gradually migrated to use the new parameter extraction capabilities. This can be done by:

1. Converting `NexusCommand` instances to `NexusCommandEx`
2. Implementing parameter-aware handlers
3. Registering commands with parameter names

## Directory Structure

```
include/nlink/cli/
├── command.h               # Updated command structure definitions
├── command_params.h        # New parameter handling functionality
├── command_router.h        # Updated command routing with parameter support
└── commands/
    ├── load.h              # Example updated command header
    └── ...

src/cli/
├── command.c               # Updated command implementation
├── command_params.c        # New parameter handling implementation
├── command_router.c        # Updated command routing implementation
└── commands/
    ├── load.c              # Example updated command implementation
    └── ...
```

## Using the Enhanced Command System

### Defining Commands with Parameter Support

```c
// Define a command with parameter support
static NexusCommandEx my_command = {
    .name = "mycommand",
    .description = "My command with parameters",
    .handler = NULL,  // Basic handler (optional)
    .handler_with_params = my_command_handler  // Parameter-aware handler
};

// Define parameter-aware handler
static NexusResult my_command_handler(NexusContext* ctx, NlinkCommandParams* params) {
    // Extract parameters
    const char* param1 = nlink_command_params_get(params, "param1");
    const char* param2 = nlink_command_params_get(params, "param2");
    
    // Use parameters
    // ...
    
    return NEXUS_SUCCESS;
}
```

### Registering Commands with Parameter Names

```c
// Define pattern and parameter names
const char* pattern = "^mycommand ([a-zA-Z0-9_-]+)( with ([a-zA-Z0-9_-]+))?$";
const char* param_names[] = {"param1", NULL, "param2"};

// Register command with parameter names
NexusResult result = nlink_command_router_register_with_params(
    router,
    pattern,
    (NexusCommand*)&my_command,
    NLINK_PATTERN_FLAG_REGEX,
    param_names,
    3
);
```

### Executing Commands with Parameter Extraction

```c
// Execute command with parameter extraction
NlinkCommandParams* params = NULL;
NexusResult result = nlink_command_router_execute_with_params(
    router,
    "mycommand value1 with value2",
    ctx,
    &params
);

// Don't forget to clean up
if (params) {
    nlink_command_params_destroy(params);
}
```

## Pattern Matching and Parameter Extraction

The parameter extraction is based on capturing groups in the pattern. For regex patterns, these are defined using parentheses:

```
^load ([a-zA-Z0-9_-]+)( version ([0-9.]+))?$
      -------------- --------------------
           |                 |
     component name       version
     (required)          (optional)
```

When registering a command, you can provide parameter names that correspond to these capturing groups:

```c
const char* param_names[] = {"component", NULL, "version"};
```

- `"component"` maps to the first capturing group
- `NULL` for the second group (which matches the entire " version X.Y.Z" part)
- `"version"` maps to the third capturing group

## Compatibility with Existing Code

The enhanced command system is backward compatible with the existing system:

- `NexusCommand` is still supported
- `nlink_command_router_execute()` still works without parameter extraction
- Commands without parameter-aware handlers will still function as before

Existing commands can be migrated gradually to use the new parameter extraction capabilities.

## Best Practices

1. **Use descriptive parameter names** - Choose parameter names that clearly describe what the parameter represents.

2. **Document patterns** - Add comments to explain what each part of the pattern matches.

3. **Always check parameters** - Always check if parameters exist before using them, especially for optional parameters.

4. **Clean up resources** - Always free parameter lists when done with them.

5. **Provide helpful error messages** - When parameters are missing or invalid, provide clear error messages.

## Example Command Registration

Here's a complete example of registering a command with parameter extraction:

```c
// Define the command
static NexusCommandEx load_command = {
    .name = "load",
    .description = "Load a component by name and optionally specify a version",
    .handler = NULL,
    .handler_with_params = load_command_handler
};

// Define the registration function
NexusResult load_command_register(NlinkCommandRouter* router) {
    if (!router) {
        return NEXUS_INVALID_PARAMETER;
    }
    
    // Define pattern and parameter names
    const char* pattern = "^load ([a-zA-Z0-9_-]+)( version ([0-9.]+))?$";
    const char* param_names[] = {"component", NULL, "version"};
    
    // Register the command
    return nlink_command_router_register_with_params(
        router,
        pattern,
        (NexusCommand*)&load_command,
        NLINK_PATTERN_FLAG_REGEX,
        param_names,
        3
    );
}
```

This registration allows the command to be used in the following ways:

- `load core` - Loads the "core" component
- `load minimizer version 1.2.3` - Loads the "minimizer" component, version 1.2.3

The handler will receive parameters:
- `component` = "core" or "minimizer"
- `version` = NULL or "1.2.3"


