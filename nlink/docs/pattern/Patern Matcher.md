# NexusLink Pattern Matcher with Parameter Extraction

## Overview

The NexusLink Pattern Matcher provides a flexible and powerful pattern matching system that supports:

1. **Literal string matching** - Exact string comparison
2. **Glob pattern matching** - Unix-style wildcard patterns (`*`, `?`, `[abc]`)
3. **Regular expression matching** - Full POSIX extended regular expressions

This component has been enhanced with parameter extraction capabilities, allowing command patterns to capture and extract parameters from matched strings. This feature is particularly useful for CLI commands, allowing for more sophisticated command parsing.

## Parameter Extraction

The pattern matcher can now extract parameters from matched patterns using:

- **Regex capturing groups** - Extract matched substrings within parentheses
- **Glob wildcards** - Capture text matched by wildcard patterns
- **Named parameters** - Associate meaningful names with captured values

### Basic Parameter Extraction

```c
// Create a pattern matcher with a regex pattern
NlinkPatternMatcher* matcher = nlink_pattern_create(
    "load ([a-zA-Z0-9_-]+)( version ([0-9.]+))?",  // Pattern with capturing groups
    NLINK_PATTERN_FLAG_REGEX
);

// Match with parameter extraction
NlinkMatchInfo* match_info = NULL;
if (nlink_pattern_match_with_params(matcher, "load core version 1.2.3", &match_info)) {
    // Get the number of groups (including the full match as group 0)
    size_t group_count = nlink_match_info_get_group_count(match_info);
    
    // Access the captured groups
    const char* full_match = nlink_match_info_get_group(match_info, 0);  // "load core version 1.2.3"
    const char* component = nlink_match_info_get_group(match_info, 1);   // "core"
    const char* version = nlink_match_info_get_group(match_info, 3);     // "1.2.3"
    
    // Use the extracted parameters
    printf("Loading component '%s' version '%s'\n", component, version);
    
    // Clean up
    nlink_match_info_destroy(match_info);
}

// Clean up
nlink_pattern_destroy(matcher);
```

### Command Router Integration

The pattern matcher's parameter extraction capabilities are integrated with the command router system, allowing for sophisticated command parsing:

```c
// Register a command with a regex pattern and parameter names
const char* pattern = "^load ([a-zA-Z0-9_-]+)( version ([0-9.]+))?$";
const char* param_names[] = {"component", NULL, "version"};  // Names for capturing groups

nlink_command_router_register_with_params(
    router,
    pattern,
    load_command,
    NLINK_PATTERN_FLAG_REGEX,
    param_names,
    3
);

// Command handler with parameter extraction
NexusResult load_component_handler(NexusContext* ctx, NlinkCommandParams* params) {
    // Extract parameters by name
    const char* component = nlink_command_params_get(params, "component");
    const char* version = nlink_command_params_get(params, "version");
    
    // Use the parameters
    printf("Loading component '%s'\n", component);
    if (version) {
        printf("Version: %s\n", version);
    }
    
    return NEXUS_SUCCESS;
}
```

## Pattern Replacement

The enhanced matcher also supports pattern replacement with backreferences to captured groups:

```c
// Replace pattern matches in a string
char result[256];
size_t replacements = nlink_pattern_replace(
    matcher,                 // Regex matcher
    "load core version 1.2.3", // Input string
    "component:$1 v$3",      // Replacement ($N refers to capturing group N)
    result,                  // Output buffer
    sizeof(result)           // Output buffer size
);

// result now contains: "component:core v1.2.3"
```

## API Reference

### Parameter Extraction

```c
/**
 * Match a string against the pattern, extracting parameters
 */
bool nlink_pattern_match_with_params(NlinkPatternMatcher* matcher, 
                                    const char* string, 
                                    NlinkMatchInfo** info);

/**
 * Create a new match information structure
 */
NlinkMatchInfo* nlink_match_info_create(void);

/**
 * Get the number of capturing groups in the match
 */
size_t nlink_match_info_get_group_count(const NlinkMatchInfo* info);

/**
 * Get a captured group string
 */
const char* nlink_match_info_get_group(const NlinkMatchInfo* info, size_t group_index);

/**
 * Get the start position of a captured group
 */
size_t nlink_match_info_get_group_start(const NlinkMatchInfo* info, size_t group_index);

/**
 * Get the end position of a captured group
 */
size_t nlink_match_info_get_group_end(const NlinkMatchInfo* info, size_t group_index);

/**
 * Free match information resources
 */
void nlink_match_info_destroy(NlinkMatchInfo* info);
```

### Pattern Replacement

```c
/**
 * Replace pattern matches in a string
 */
size_t nlink_pattern_replace(NlinkPatternMatcher* matcher,
                            const char* string,
                            const char* replacement,
                            char* result,
                            size_t result_size);
```

### Command Parameters

```c
/**
 * Create a new command parameter list
 */
NlinkCommandParams* nlink_command_params_create(void);

/**
 * Add a parameter to the list
 */
NexusResult nlink_command_params_add(NlinkCommandParams* params, 
                                   const char* name, 
                                   const char* value);

/**
 * Get a parameter value by name
 */
const char* nlink_command_params_get(const NlinkCommandParams* params, const char* name);

/**
 * Check if a parameter exists
 */
bool nlink_command_params_has(const NlinkCommandParams* params, const char* name);

/**
 * Get the number of parameters
 */
size_t nlink_command_params_count(const NlinkCommandParams* params);

/**
 * Get parameter at index
 */
bool nlink_command_params_get_at(const NlinkCommandParams* params, 
                               size_t index, 
                               const char** name, 
                               const char** value);

/**
 * Free command parameter list resources
 */
void nlink_command_params_destroy(NlinkCommandParams* params);
```

## Implementation Details

### Regular Expression Capturing Groups

For regex patterns, the implementation extracts capturing groups using the POSIX regex library:

- Group 0 is the full match
- Groups 1+ are the captured substrings in parentheses
- Non-capturing groups `(?:...)` are supported and not counted

### Glob Pattern Capture

For glob patterns, the implementation captures:

- Text matched by `*` wildcards
- Characters matched by `?` wildcards
- Characters matched by character classes `[abc]`

### Memory Management

All match information and extracted parameters are dynamically allocated:

- The caller is responsible for freeing match information with `nlink_match_info_destroy()`
- Command parameters are freed with `nlink_command_params_destroy()`
- Match information includes copies of the captured strings, not just pointers

## Best Practices

1. **Use meaningful parameter names** - When registering commands, use descriptive names for capturing groups to improve readability and maintainability.

2. **Check for NULL values** - Always check if parameters exist before using them, especially for optional parameters.

3. **Document patterns** - Comment your regex patterns to explain what they match and what parameters they extract.

4. **Limit complexity** - While regex can be powerful, overly complex patterns can be hard to maintain. Keep patterns as simple as possible.

5. **Provide examples** - When defining command patterns, include examples of valid inputs to help users understand the expected format.

## Example Patterns

### Simple Command with Required Parameter

```
^load ([a-zA-Z0-9_-]+)$
```
- Matches: `load core`
- Parameters: component = "core"

### Command with Optional Parameter

```
^stats( component ([a-zA-Z0-9_-]+))?$
```
- Matches: `stats` or `stats component core`
- Parameters: component = "core" (if provided)

### Command with Multiple Parameters

```
^config (set|get) ([a-zA-Z0-9_-]+)( (.+))?$
```
- Matches: `config set log-level debug` or `config get log-level`
- Parameters: action = "set", key = "log-level", value = "debug" (if provided)

### Complex Command with Multiple Optional Parameters

```
^find( in ([a-zA-Z0-9_-]+))?( type ([a-zA-Z0-9_-]+))?( limit ([0-9]+))?$
```
- Matches various combinations like:
  - `find`
  - `find in core`
  - `find type function`
  - `find in core type function`
  - `find in core type function limit 10`
- Parameters: directory, type, limit (all optional)

## Performance Considerations

Different pattern types and extraction methods have different performance characteristics:

- **Literal string matching** is the fastest (negligible overhead for extraction)
- **Glob matching with capture** is moderately expensive due to recursive nature
- **Regex matching with capturing groups** can be more expensive for complex patterns

Choose the simplest pattern type that meets your needs for optimal performance.