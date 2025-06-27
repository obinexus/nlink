# NexusLink Pattern Matching System

## Overview

The NexusLink Pattern Matching System provides robust facilities for matching file paths, strings, and other patterns within the NexusLink scripting system. It implements a declarative approach to pattern resolution with support for wildcards, regular expressions, and extended glob syntax.

## Components

The system consists of the following core components:

### Pattern Matcher

The Pattern Matcher provides high-level pattern matching operations for file path resolution:

- Pattern-based file discovery
- Wildcard path matching
- Multi-pattern resolution

### Regular Expression Matcher

The Regular Expression Matcher implements standard regular expression processing:

- Regex compilation and matching
- Capture group extraction
- Pattern optimization

### Wildcard Matcher

The Wildcard Matcher provides specialized wildcard pattern matching optimized for file paths:

- Glob pattern matching (*, ?, [abc])
- Extended glob syntax (**, ?(pat), *(pat))
- Path-specific pattern optimization

## Usage Examples

### Basic Pattern Matching

```c
// Match files using a glob pattern
nlink_pattern_options options = nlink_pattern_default_options();
nlink_pattern_result* result = nlink_match_pattern("src/*.c", ".", options);

// Process matched files
for (size_t i = 0; i < result->count; i++) {
    printf("Matched: %s\n", result->matches[i]);
}

// Free result when done
nlink_pattern_result_free(result);
```

### Regular Expression Matching

```c
// Compile a regular expression
nlink_regex_options options = nlink_regex_default_options();
nlink_regex* regex = nlink_regex_compile("pattern_(\\d+)\\.c", options);

// Match a string
nlink_regex_match match;
if (nlink_regex_match(regex, "pattern_123.c", &match)) {
    // Extract captured group
    char* group = nlink_regex_get_group("pattern_123.c", &match, 1);
    printf("Captured: %s\n", group);
    free(group);
}

// Free resources
nlink_regex_match_free(&match);
nlink_regex_free(regex);
```

### Wildcard Path Matching

```c
// Match a file path using wildcards
nlink_wildcard_options options = nlink_wildcard_default_options();
if (nlink_wildcard_match_path("src/**/*.c", "src/core/file.c", 
                             NLINK_WILDCARD_EXTENDED, options)) {
    printf("Path matches pattern\n");
}

// Get base directory from pattern
char* base_dir = nlink_wildcard_get_base_dir("src/**/*.c");
printf("Base directory: %s\n", base_dir);
free(base_dir);
```

## Implementation Details

The pattern matching system is implemented with the following design considerations:

1. **Efficiency**: Optimized for rapid path matching with minimal memory overhead
2. **Flexibility**: Support for multiple pattern syntaxes and matching modes
3. **Robustness**: Comprehensive error handling and edge case management
4. **Extensibility**: Plugin architecture for custom pattern matchers

## API Reference

See the following header files for detailed API documentation:

- `pattern_matcher.h`: High-level pattern matching API
- `regex_matcher.h`: Regular expression processing
- `wildcard_matcher.h`: Specialized wildcard pattern matching
