# NexusLink Regular Expression Pattern System

## Overview

The NexusLink Regular Expression System provides a comprehensive implementation of regular expression pattern matching for the NexusLink scripting system. This document describes the API, usage patterns, and implementation details.

## API Reference

### Compilation and Matching

```c
// Create default options
nlink_regex_options options = nlink_regex_default_options();

// Customize options
options.case_insensitive = true;
options.multiline = true;

// Compile a regular expression
nlink_regex* regex = nlink_regex_compile("pattern_(\\d+)\\.c", options);

// Match a string and get capture groups
nlink_regex_match match;
if (nlink_regex_match(regex, "pattern_123.c", &match)) {
    // Process match
    printf("Matched at positions %zu-%zu\n", match.start, match.end);
    
    // Extract captured group
    char* group = nlink_regex_get_group("pattern_123.c", &match, 1);
    printf("Captured: %s\n", group);
    free(group);
}

// Free resources
nlink_regex_match_free(&match);
nlink_regex_free(regex);
```

### Escaping Special Characters

```c
// Escape special regex characters
char* escaped = nlink_regex_escape("file[1].c");
printf("Escaped pattern: %s\n", escaped);  // Output: file\\[1\\]\\.c
free(escaped);
```

## Implementation Details

The NexusLink Regular Expression System is implemented with the following components:

1. **Regex Compiler**: Translates regex patterns into efficient internal representations
2. **Matcher Engine**: Performs pattern matching with capture group support
3. **Optimizer**: Improves matching performance for common patterns
4. **Character Class Handler**: Manages character sets and ranges

## Performance Considerations

Regular expression matching can be computationally expensive. The NexusLink implementation includes several optimizations:

1. **Pattern Caching**: Compiled patterns are cached to avoid repeated compilation
2. **Lazy Evaluation**: Capture groups are computed only when requested
3. **Early Termination**: Matching stops as soon as a result is determined
4. **Special Case Handling**: Common patterns use optimized implementations

## Examples

### Pattern Validation

```c
// Validate an email address
nlink_regex* email_regex = nlink_regex_compile(
    "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$",
    nlink_regex_default_options()
);

if (nlink_regex_match(email_regex, "user@example.com", NULL)) {
    printf("Valid email address\n");
}

nlink_regex_free(email_regex);
```

### Text Extraction

```c
// Extract version numbers from text
nlink_regex* version_regex = nlink_regex_compile(
    "version (\\d+)\\.(\\d+)\\.(\\d+)",
    nlink_regex_default_options()
);

const char* text = "Current version is version 1.2.3";
nlink_regex_match match;

if (nlink_regex_match(version_regex, text, &match)) {
    char* major = nlink_regex_get_group(text, &match, 1);
    char* minor = nlink_regex_get_group(text, &match, 2);
    char* patch = nlink_regex_get_group(text, &match, 3);
    
    printf("Version: %s.%s.%s\n", major, minor, patch);
    
    free(major);
    free(minor);
    free(patch);
}

nlink_regex_match_free(&match);
nlink_regex_free(version_regex);
```

### Pattern Replacement

```c
// Replace matched patterns
// Note: This is a conceptual example - the actual API does not include
// a replace function directly, but it can be implemented using the match API

const char* input = "Replace pattern_123 and pattern_456";
const char* pattern = "pattern_(\\d+)";
const char* replacement = "item_$1";

// Implementation would use nlink_regex_match in a loop to find all matches
// and build a new string with replacements
```

## Error Handling

All regex functions follow the NexusLink error handling conventions:

1. Functions that can fail return NULL or false on failure
2. Detailed error information can be retrieved from the error handling system
3. Memory is automatically cleaned up when freeing the regex object

## Limitations

The current implementation has the following limitations:

1. No support for look-ahead or look-behind assertions
2. Limited support for Unicode character properties
3. No support for recursive patterns
