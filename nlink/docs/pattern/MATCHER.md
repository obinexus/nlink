# NexusLink Pattern Matcher Documentation

## Overview

The NexusLink Pattern Matcher provides a flexible and powerful pattern matching system that supports:

1. **Literal string matching** - Exact string comparison
2. **Glob pattern matching** - Unix-style wildcard patterns (`*`, `?`, `[abc]`)
3. **Regular expression matching** - Full POSIX extended regular expressions

This component is designed for use throughout the NexusLink system, with a focus on command routing in the CLI but applicable to any pattern matching needs.

## Pattern Types

### Literal String Matching

The simplest form of matching, literal string matching compares strings exactly (or case-insensitively if configured).

```c
// Case-sensitive literal match
NlinkPatternMatcher* matcher = nlink_pattern_create("hello", NLINK_PATTERN_FLAG_NONE);
bool match = nlink_pattern_match(matcher, "hello");  // true
match = nlink_pattern_match(matcher, "Hello");       // false

// Case-insensitive literal match
matcher = nlink_pattern_create("hello", NLINK_PATTERN_FLAG_CASE_INSENSITIVE);
match = nlink_pattern_match(matcher, "Hello");       // true
```

### Glob Pattern Matching

Glob patterns use wildcards to match patterns:

- `*` matches any number of characters (including zero)
- `?` matches exactly one character
- `[abc]` matches any character within the brackets
- `[a-z]` matches any character in the range a through z
- `[^abc]` matches any character not in the brackets

```c
// Glob pattern with wildcard
NlinkPatternMatcher* matcher = nlink_pattern_create("hello*", NLINK_PATTERN_FLAG_GLOB);
bool match = nlink_pattern_match(matcher, "hello world");  // true
match = nlink_pattern_match(matcher, "goodbye");           // false

// Glob pattern with character class
matcher = nlink_pattern_create("h[ae]llo", NLINK_PATTERN_FLAG_GLOB);
match = nlink_pattern_match(matcher, "hello");             // true
match = nlink_pattern_match(matcher, "hallo");             // true
match = nlink_pattern_match(matcher, "hillo");             // false
```

### Regular Expression Matching

For more complex pattern matching needs, the matcher supports full POSIX extended regular expressions:

```c
// Simple regex with EXTENDED flag
NlinkPatternMatcher* matcher = nlink_pattern_create("^hello.*world$", NLINK_PATTERN_FLAG_EXTENDED);
bool match = nlink_pattern_match(matcher, "hello wonderful world");  // true
match = nlink_pattern_match(matcher, "goodbye world");               // false

// Alternation
matcher = nlink_pattern_create("cat|dog", NLINK_PATTERN_FLAG_REGEX);
match = nlink_pattern_match(matcher, "cat");                         // true
match = nlink_pattern_match(matcher, "dog");                         // true
match = nlink_pattern_match(matcher, "bird");                        // false

// Complex regex
matcher = nlink_pattern_create("^(https?|ftp)://[^\\s/$.?#].[^\\s]*$", NLINK_PATTERN_FLAG_REGEX);
match = nlink_pattern_match(matcher, "https://example.com/path");    // true
match = nlink_pattern_match(matcher, "not a url");                   // false
```

## Pattern Type Detection

The matcher can automatically detect the appropriate pattern type based on the pattern string:

- If the pattern contains `*`, `?`, or `[`, it's treated as a glob pattern
- If the pattern contains regex-specific characters like `^`, `$`, `(`, `)`, `+`, `.`, `|`, or `\`, it's treated as a regex
- Otherwise, it's treated as a literal string

You can override this detection by specifying the `NLINK_PATTERN_FLAG_GLOB`, `NLINK_PATTERN_FLAG_REGEX`, or `NLINK_PATTERN_FLAG_EXTENDED` flags.

## API Reference

### Creation and Destruction

```c
// Create a new pattern matcher
NlinkPatternMatcher* nlink_pattern_create(const char* pattern, NlinkPatternFlags flags);

// Free pattern matcher resources
void nlink_pattern_destroy(NlinkPatternMatcher* matcher);
```

### Pattern Matching

```c
// Match a string against the pattern
bool nlink_pattern_match(NlinkPatternMatcher* matcher, const char* string);
```

### Inspection Functions

```c
// Get the pattern string
const char* nlink_pattern_get_pattern(const NlinkPatternMatcher* matcher);

// Get the pattern flags
NlinkPatternFlags nlink_pattern_get_flags(const NlinkPatternMatcher* matcher);

// Check if the pattern is a regex pattern
bool nlink_pattern_is_regex(const NlinkPatternMatcher* matcher);

// Check if the pattern is a glob pattern
bool nlink_pattern_is_glob(const NlinkPatternMatcher* matcher);
```

### Pattern Flags

Available flags for pattern matching:

```c
typedef enum {
    NLINK_PATTERN_FLAG_NONE              = 0x00,  // No special handling
    NLINK_PATTERN_FLAG_CASE_INSENSITIVE  = 0x01,  // Case insensitive matching
    NLINK_PATTERN_FLAG_GLOB              = 0x02,  // Force glob-style pattern matching
    NLINK_PATTERN_FLAG_REGEX             = 0x04,  // Force regex pattern matching
    NLINK_PATTERN_FLAG_EXTENDED          = 0x08,  // Use extended regex syntax
    NLINK_PATTERN_FLAG_MULTILINE         = 0x10,  // Multi-line mode (^ and $ match line boundaries)
    NLINK_PATTERN_FLAG_DOT_ALL           = 0x20,  // Dot matches newlines too
} NlinkPatternFlags;
```

## Integration with Command Router

The enhanced pattern matcher enables more powerful command routing in the CLI system:

```c
// Register a command with a regex pattern
NexusResult result = nlink_command_router_register(
    router,
    "^stats (memory|components|all)$",  // Matches "stats memory", "stats components", or "stats all"
    stats_command,
    NLINK_PATTERN_FLAG_EXTENDED
);

// Register a command with a glob pattern
result = nlink_command_router_register(
    router,
    "load lib*.so",  // Matches "load libmath.so", "load libregex.so", etc.
    load_command,
    NLINK_PATTERN_FLAG_GLOB
);
```

## Performance Considerations

Different pattern types have different performance characteristics:

- **Literal matching** is the fastest (O(n) time complexity)
- **Glob matching** is moderately fast (O(m*n) in worst case, where m is pattern length and n is string length)
- **Regex matching** can be more expensive, especially for complex patterns

Choose the simplest pattern type that meets your needs for best performance.

## Thread Safety

The pattern matcher functions are not inherently thread-safe. If you need to use the same matcher from multiple threads, you should:

1. Create the matcher once
2. Use appropriate synchronization when calling `nlink_pattern_match()`
3. Only destroy the matcher when no threads are using it

Alternatively, create a separate matcher instance for each thread.