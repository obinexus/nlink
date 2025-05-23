// include/nexus_semver.h
// Semantic versioning implementation for NexusLink
// Author: Nnamdi Michael Okpala

#ifndef NEXUS_SEMVER_H
#define NEXUS_SEMVER_H

#include <stdbool.h>

// Semantic version structure
typedef struct {
    int major;              // Major version component
    int minor;              // Minor version component
    int patch;              // Patch version component
    char* prerelease;       // Pre-release identifier (e.g., "alpha.1")
    char* build;            // Build metadata
    bool is_wildcard;       // Indicates wildcard version (e.g., "*" or "latest")
} SemVer;

// Parse a version string into a semantic version structure
// Returns NULL if parsing fails
SemVer* semver_parse(const char* version_str);

// Compare two semantic versions
// Returns: -1 if a < b, 0 if a == b, 1 if a > b
int semver_compare(const SemVer* a, const SemVer* b);

// Check if a version satisfies a constraint
// Examples of constraints:
//   - "1.2.3"    : Exact match
//   - ">1.2.3"   : Greater than
//   - ">=1.2.3"  : Greater than or equal
//   - "<1.2.3"   : Less than
//   - "<=1.2.3"  : Less than or equal
//   - "^1.2.3"   : Compatible with (same major version)
//   - "~1.2.3"   : Compatible with (same major.minor version)
//   - "*"        : Any version
bool semver_satisfies(const char* version, const char* constraint);

// Free a semantic version structure
void semver_free(SemVer* ver);

#endif // NEXUS_SEMVER_H