# Migrating to Enhanced Command System with Parameter Extraction

This guide outlines the steps to migrate existing NexusLink CLI commands to use the new parameter extraction system.

## Overview

The enhanced command system adds parameter extraction capabilities, allowing commands to extract and use parameters from command input strings using pattern matching. This system makes command implementations more robust and simplifies code.

## Benefits of Migration

- **Cleaner command implementation**: No need to manually parse arguments.
- **Type-safe parameter access**: Access parameters by name rather than parsing argument arrays.
- **Automatic validation**: Pattern matching validates command syntax before execution.
- **Support for optional parameters**: Easily handle optional parameters and subcommands.
- **Reduced boilerplate code**: Eliminate repetitive argument parsing code.

## Migration Steps

### 1. Update Command Structure

Add the `handler_with_params` field to your command structure:

```c
// Before
NexusCommand old_command = {
    .name = "mycommand",
    .description = "My command description",
    .handler = my_command_handler,
    .execute = my_command_execute
};

// After
NexusCommand updated_command = {
    .name = "mycommand",
    .description = "My command description",
    .handler = my_command_handler,              // Keep for backward compatibility
    .handler_with_params = my_command_handler_with_params,  // Add parameter-aware handler
    .execute = my_command_execute               // Keep for backward compatibility
};
```

### 2. Implement Parameter-Aware Handler

Create a new handler function that accepts parameters:

```c
static NexusResult my_command_handler_with_params(NexusContext* ctx, NlinkCommandParams* params) {
    // Get required parameters
    const char* param1 = nlink_command_params_get(params, "param1");
    if (!param1) {
        nexus_log(ctx, NEXUS_LOG_ERROR, "Missing required parameter 'param1'");
        return NEXUS_INVALID_PARAMETER;
    }
    
    // Get optional parameters
    const char* param2 = nlink_command_params_get(params, "param2");
    
    // Implement command logic using parameters
    nexus_log(ctx, NEXUS_LOG_INFO, "Executing with param1='%s', param2='%s'", 
              param1, param2 ? param2 : "(not provided)");
    
    // ... command implementation ...
    
    return NEXUS_SUCCESS;
}
```

### 3. Register Command with Pattern Matching

In your initialization code, register the command with pattern matching:

```c
// Define pattern and parameter names
const char* pattern = "^mycommand ([a-zA-Z0-9_-]+)( with ([a-zA-Z0-9_-]+))?$";
const char* param_names[] = {"param1", NULL, "param2"};

// Register command with parameter names
NexusResult result = nlink_command_router_register_with_params(
    router,
    pattern,
    &updated_command,
    NLINK_PATTERN_FLAG_REGEX,
    param_names,
    3
);
```

The pattern above matches inputs like:
- `mycommand value1` - Sets param1="value1"
- `mycommand value1 with value2` - Sets param1="value1", param2="value2"

Note that the `NULL` entry corresponds to the capturing group that matches the entire " with value2" section, which we don't need separately.

### 4. Create Registration Function

For convenience, create a function to register all patterns for a command:

```c
NexusResult my_command_register(NlinkCommandRouter* router) {
    if (!router) {
        return NEXUS_INVALID_PARAMETER;
    }
    
    // Register basic pattern
    const char* basic_pattern = "^mycommand ([a-zA-Z0-9_-]+)$";
    const char* basic_params[] = {"param1"};
    
    NexusResult result = nlink_command_router_register_with_params(
        router,
        basic_pattern,
        &updated_command,
        NLINK_PATTERN_FLAG_REGEX,
        basic_params,
        1
    );
    
    if (result != NEXUS_SUCCESS) {
        return result;
    }
    
    // Register extended pattern
    const char* extended_pattern = "^mycommand ([a-zA-Z0-9_-]+) with ([a-zA-Z0-9_-]+)$";
    const char* extended_params[] = {"param1", "param2"};
    
    return nlink_command_router_register_with_params(
        router,
        extended_pattern,
        &updated_command,
        NLINK_PATTERN_FLAG_REGEX,
        extended_params,
        2
    );
}
```

### 5. Update Command Initialization

In your CLI system initialization:

```c
// Initialize the CLI with parameter-aware commands
NexusResult result = my_command_register(g_command_router);
if (result != NEXUS_SUCCESS) {
    // Handle error
}
```

## Best Practices

### Pattern Matching

1. **Use anchors**: Start patterns with `^` and end with `# Migrating to Enhanced Command System with Parameter Extraction

This guide outlines the steps to migrate existing NexusLink CLI commands to use the new parameter extraction system.

## Overview

The enhanced command system adds parameter extraction capabilities, allowing commands to extract and use parameters from command input strings using pattern matching. This system makes command implementations more robust and simplifies code.

## Benefits of Migration

- **Cleaner command implementation**: No need to manually parse arguments.
- **Type-safe parameter access**: Access parameters by name rather than parsing argument arrays.
- **Automatic validation**: Pattern matching validates command syntax before execution.
- **Support for optional parameters**: Easily handle optional parameters and subcommands.
- **Reduced boilerplate code**: Eliminate repetitive argument parsing code.

## Migration Steps

### 1. Update Command Structure

Add the `handler_with_params` field to your command structure:

```c
// Before
NexusCommand old_command = {
    .name = "mycommand",
    .description = "My command description",
    .handler = my_command_handler,
    .execute = my_command_execute
};

// After
NexusCommand updated_command = {
    .name = "mycommand",
    .description = "My command description",
    .handler = my_command_handler,              // Keep for backward compatibility
    .handler_with_params = my_command_handler_with_params,  // Add parameter-aware handler
    .execute = my_command_execute               // Keep for backward compatibility
};
```

### 2. Implement Parameter-Aware Handler

Create a new handler function that accepts parameters:

```c
static NexusResult my_command_handler_with_params(NexusContext* ctx, NlinkCommandParams* params) {
    // Get required parameters
    const char* param1 = nlink_command_params_get(params, "param1");
    if (!param1) {
        nexus_log(ctx, NEXUS_LOG_ERROR, "Missing required parameter 'param1'");
        return NEXUS_INVALID_PARAMETER;
    }
    
    // Get optional parameters
    const char* param2 = nlink_command_params_get(params, "param2");
    
    // Implement command logic using parameters
    nexus_log(ctx, NEXUS_LOG_INFO, "Executing with param1='%s', param2='%s'", 
              param1, param2 ? param2 : "(not provided)");
    
    // ... command implementation ...
    
    return NEXUS_SUCCESS;
}
```

 to match the entire input.
2. **Group parameters**: Use parentheses to capture parameter values: `([a-zA-Z0-9_-]+)`.
3. **Make groups optional when needed**: Use `(...)` for required parameters and `(...)?` for optional ones.
4. **Use character classes**: Specify allowed characters, like `[a-zA-Z0-9_-]` for alphanumeric + dash/underscore.
5. **Test patterns thoroughly**: Verify they match all expected inputs and reject invalid ones.

### Parameter Handling

1. **Always check parameters**: Verify required parameters exist before using them.
2. **Provide helpful error messages**: Be specific about missing or invalid parameters.
3. **Use descriptive parameter names**: Choose names that clearly indicate the parameter's purpose.
4. **Document patterns and parameters**: Add comments to explain the pattern structure.
5. **Clean up resources**: Always free parameter lists when done with them.

## Example: Converting the Load Command

Here's how we converted the `load` command to use parameter extraction:

### Before:

```c
static int load_command_handler(NexusContext* ctx, int argc, char** argv) {
    if (argc < 1) {
        fprintf(stderr, "Error: Component name required\n");
        return 1;
    }
    
    const char* component_name = argv[0];
    const char* version = NULL;
    
    // Manual argument parsing
    for (int i = 1; i < argc - 1; i++) {
        if (strcmp(argv[i], "version") == 0 && i + 1 < argc) {
            version = argv[i + 1];
            i++; // Skip the version value
        }
    }
    
    // Command implementation...
}
```

### After:

```c
static NexusResult load_with_params_handler(NexusContext* ctx, NlinkCommandParams* params) {
    // Simple parameter extraction by name
    const char* component = nlink_command_params_get(params, "component");
    if (!component) {
        nexus_log(ctx, NEXUS_LOG_ERROR, "Component name required");
        return NEXUS_INVALID_PARAMETER;
    }
    
    // Optional parameters
    const char* version = nlink_command_params_get(params, "version");
    
    // Command implementation...
}
```

## Moving Forward

1. **Gradual Migration**: Start by converting simple commands, then tackle more complex ones.
2. **Maintain Backward Compatibility**: Keep the existing handler for backward compatibility during migration.
3. **Test Thoroughly**: Create tests to verify parameter extraction works as expected.
4. **Document New Patterns**: Keep a reference of all command patterns for future maintenance.

By migrating to the enhanced command system, you'll improve code quality, reduce bugs from manual parsing, and make the CLI more robust and maintainable.