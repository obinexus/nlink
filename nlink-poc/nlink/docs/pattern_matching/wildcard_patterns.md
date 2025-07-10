# NexusLink Wildcard Pattern Syntax

## Glob Patterns

Glob patterns are the simplest form of wildcard patterns, providing basic path matching capabilities:

| Pattern | Description | Example |
|---------|-------------|---------|
| `*` | Matches any sequence of characters except `/` | `*.c` matches all C files in current directory |
| `?` | Matches any single character except `/` | `file?.c` matches `file1.c` but not `file10.c` |
| `[abc]` | Matches any character in the set | `file[123].c` matches `file1.c`, `file2.c`, `file3.c` |
| `[a-z]` | Matches any character in the range | `file[a-c].c` matches `filea.c`, `fileb.c`, `filec.c` |
| `[!abc]` | Matches any character not in the set | `file[!123].c` matches `filed.c` but not `file1.c` |

## Extended Glob Patterns

Extended glob patterns add more powerful matching capabilities:

| Pattern | Description | Example |
|---------|-------------|---------|
| `**` | Matches any number of directories | `src/**/*.c` matches all C files in src and its subdirectories |
| `?(pat)` | Matches zero or one occurrence of pat | `file?(1).c` matches `file.c` and `file1.c` |
| `*(pat)` | Matches zero or more occurrences of pat | `file*(1).c` matches `file.c`, `file1.c`, `file11.c` |
| `+(pat)` | Matches one or more occurrences of pat | `file+(1).c` matches `file1.c`, `file11.c` but not `file.c` |
| `@(pat)` | Matches exactly one occurrence of pat | `file@(1).c` matches `file1.c` but not `file.c` or `file11.c` |
| `!(pat)` | Matches anything except pat | `file!(1).c` matches `file2.c` but not `file1.c` |

## Regular Expression Patterns

For more complex matching needs, regular expressions provide the most flexibility:

| Pattern | Description | Example |
|---------|-------------|---------|
| `.` | Matches any single character | `file.\.c` matches `file1.c`, `fileA.c` |
| `^` | Matches the start of the string | `^src/` matches strings that start with "src/" |
| `$` | Matches the end of the string | `\.c$` matches strings that end with ".c" |
| `*` | Matches 0 or more of the previous character | `a*b` matches "b", "ab", "aab", etc. |
| `+` | Matches 1 or more of the previous character | `a+b` matches "ab", "aab", but not "b" |
| `?` | Matches 0 or 1 of the previous character | `a?b` matches "ab" and "b", but not "aab" |
| `{n}` | Matches exactly n of the previous character | `a{3}b` matches "aaab" only |
| `{n,m}` | Matches between n and m of the previous character | `a{2,3}b` matches "aab" and "aaab" |
| `[abc]` | Matches any character in the set | `[abc]` matches "a", "b", or "c" |
| `[^abc]` | Matches any character not in the set | `[^abc]` matches any character except "a", "b", or "c" |
| `\d` | Matches any digit | `\d+` matches one or more digits |
| `\w` | Matches any word character | `\w+` matches one or more word characters |
| `\s` | Matches any whitespace character | `\s+` matches one or more whitespace characters |
| `(pat)` | Captures the matched pattern | `(file_\d+)\.c` captures "file_123" from "file_123.c" |
| `pat1|pat2` | Matches either pat1 or pat2 | `a|b` matches "a" or "b" |

## Usage in NexusLink

Wildcard patterns are used extensively in the NexusLink scripting system for:

1. **Source Resolution**: Identifying source files to process
2. **Include Binding**: Matching header files for dependency management
3. **Output Target**: Specifying output file patterns
4. **Resource Discovery**: Finding and processing resource files

Example in NexusLink manifest:

```
// NexusLink manifest using wildcard patterns
sources {
    include: [
        "src/**/*.c",        // All C source files in src and subdirectories
        "!src/**/*_test.c"   // Exclude test files
    ],
    headers: [
        "include/**/*.h"     // All header files in include and subdirectories
    ]
}
```
