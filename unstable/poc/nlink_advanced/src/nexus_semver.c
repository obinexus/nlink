// src/nexus_semver.c
// Semantic versioning implementation for NexusLink
// Author: Nnamdi Michael Okpala

#include "../include/nexus_semver.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Parse a semantic version string into components
SemVer* semver_parse(const char* version_str) {
    if (!version_str) return NULL;
    
    SemVer* ver = (SemVer*)malloc(sizeof(SemVer));
    memset(ver, 0, sizeof(SemVer));
    
    // Handle wildcards
    if (strcmp(version_str, "*") == 0 || 
        strcmp(version_str, "latest") == 0) {
        ver->is_wildcard = true;
        return ver;
    }
    
    // Copy the string for manipulation
    char* str = strdup(version_str);
    char* p = str;
    
    // Parse major version
    char* end;
    ver->major = strtol(p, &end, 10);
    
    if (p == end || *end != '.') {
        free(str);
        free(ver);
        return NULL;
    }
    
    // Move to minor version
    p = end + 1;
    ver->minor = strtol(p, &end, 10);
    
    if (p == end || *end != '.') {
        free(str);
        free(ver);
        return NULL;
    }
    
    // Move to patch version
    p = end + 1;
    ver->patch = strtol(p, &end, 10);
    
    // Parse pre-release if present
    if (*end == '-') {
        p = end + 1;
        // Find the end of the pre-release
        end = p;
        while (*end && *end != '+') end++;
        
        // Copy pre-release string
        size_t len = end - p;
        ver->prerelease = (char*)malloc(len + 1);
        strncpy(ver->prerelease, p, len);
        ver->prerelease[len] = '\0';
    }
    
    // Parse build metadata if present
    if (*end == '+') {
        p = end + 1;
        ver->build = strdup(p);
    }
    
    free(str);
    return ver;
}

// Compare two semantic versions
// Returns: -1 if a < b, 0 if a == b, 1 if a > b
int semver_compare(const SemVer* a, const SemVer* b) {
    // Handle wildcard cases
    if (a->is_wildcard) return b->is_wildcard ? 0 : -1;
    if (b->is_wildcard) return 1;
    
    // Compare major.minor.patch
    if (a->major != b->major) return a->major > b->major ? 1 : -1;
    if (a->minor != b->minor) return a->minor > b->minor ? 1 : -1;
    if (a->patch != b->patch) return a->patch > b->patch ? 1 : -1;
    
    // If we get here, the core versions are the same
    
    // Pre-release versions have lower precedence than the associated normal version
    if (a->prerelease && !b->prerelease) return -1;
    if (!a->prerelease && b->prerelease) return 1;
    
    // If both have pre-release versions, compare them
    if (a->prerelease && b->prerelease) {
        return strcmp(a->prerelease, b->prerelease);
    }
    
    // Build metadata doesn't affect version precedence
    return 0;
}

// Check if a version satisfies a constraint
bool semver_satisfies(const char* version, const char* constraint) {
    // Parse both versions
    SemVer* ver = semver_parse(version);
    if (!ver) return false;
    
    // Handle special constraints
    if (strcmp(constraint, "*") == 0 || 
        strcmp(constraint, "latest") == 0) {
        semver_free(ver);
        return true;
    }
    
    // Handle range constraints (simplified)
    char* constraint_copy = strdup(constraint);
    char* operator = constraint_copy;
    
    // Find the version part
    while (*operator && !isdigit(*operator) && *operator != '*') operator++;
    
    // Extract the operator
    char op_type[3] = {0};
    if (operator > constraint_copy) {
        size_t op_len = operator - constraint_copy;
        if (op_len > 2) op_len = 2;
        strncpy(op_type, constraint_copy, op_len);
    }
    
    // Parse the constraint version
    SemVer* cons = semver_parse(operator);
    if (!cons) {
        free(constraint_copy);
        semver_free(ver);
        return false;
    }
    
    // Do the comparison based on operator
    bool result = false;
    int cmp = semver_compare(ver, cons);
    
    if (op_type[0] == 0 || strcmp(op_type, "=") == 0) {
        // Exact match
        result = (cmp == 0);
    } else if (strcmp(op_type, ">") == 0) {
        result = (cmp > 0);
    } else if (strcmp(op_type, ">=") == 0) {
        result = (cmp >= 0);
    } else if (strcmp(op_type, "<") == 0) {
        result = (cmp < 0);
    } else if (strcmp(op_type, "<=") == 0) {
        result = (cmp <= 0);
    } else if (strcmp(op_type, "^") == 0) {
        // Compatible with: same major version, greater than or equal to specified version
        result = (ver->major == cons->major && cmp >= 0);
    } else if (strcmp(op_type, "~") == 0) {
        // Compatible with: same major.minor version, greater than or equal to specified version
        result = (ver->major == cons->major && ver->minor == cons->minor && cmp >= 0);
    }
    
    free(constraint_copy);
    semver_free(ver);
    semver_free(cons);
    
    return result;
}

// Free a semantic version structure
void semver_free(SemVer* ver) {
    if (!ver) return;
    
    free(ver->prerelease);
    free(ver->build);
    free(ver);
}