#include "nlink/core/versioning/nexus_version.h"

/**
 * Parse a version string into a NexusVersion structure
 * 
 * @param version_str The version string to parse (e.g., "1.2.3-alpha.1+build.42")
 * @return A pointer to a newly allocated NexusVersion structure, or NULL on error
 */
NexusVersion* nexus_version_parse(const char* version_str) {
    if (!version_str || !*version_str) return NULL;
    
    // Special case for wildcard version
    if (strcmp(version_str, "*") == 0 || strcmp(version_str, "latest") == 0) {
        NexusVersion* ver = (NexusVersion*)malloc(sizeof(NexusVersion));
        ver->major = -1;  // -1 indicates wildcard
        ver->minor = -1;
        ver->patch = -1;
        ver->prerelease = NULL;
        ver->build = NULL;
        return ver;
    }
    
    NexusVersion* ver = (NexusVersion*)malloc(sizeof(NexusVersion));
    if (!ver) return NULL;
    
    // Initialize fields
    ver->major = 0;
    ver->minor = 0;
    ver->patch = 0;
    ver->prerelease = NULL;
    ver->build = NULL;
    
    // Make a copy of the string for parsing
    char* str = strdup(version_str);
    if (!str) {
        free(ver);
        return NULL;
    }
    
    // Separate build metadata
    char* build_sep = strchr(str, '+');
    if (build_sep) {
        *build_sep = '\0';
        ver->build = strdup(build_sep + 1);
    }
    
    // Separate prerelease info
    char* pre_sep = strchr(str, '-');
    if (pre_sep) {
        *pre_sep = '\0';
        ver->prerelease = strdup(pre_sep + 1);
    }
    
    // Parse major.minor.patch
    char* major_str = str;
    char* minor_str = strchr(major_str, '.');
    if (!minor_str) {
        // Only major version specified
        ver->major = atoi(major_str);
    } else {
        *minor_str = '\0';
        minor_str++;
        ver->major = atoi(major_str);
        
        char* patch_str = strchr(minor_str, '.');
        if (!patch_str) {
            // Only major.minor specified
            ver->minor = atoi(minor_str);
        } else {
            *patch_str = '\0';
            patch_str++;
            ver->minor = atoi(minor_str);
            ver->patch = atoi(patch_str);
        }
    }
    
    free(str);
    return ver;
}

/**
 * Compare two versions
 * 
 * @param a First version
 * @param b Second version
 * @return -1 if a < b, 0 if a == b, 1 if a > b
 */
int nexus_version_compare(const NexusVersion* a, const NexusVersion* b) {
    if (!a || !b) return 0;
    
    // Handle wildcard versions
    if (a->major < 0) return (b->major < 0) ? 0 : -1;
    if (b->major < 0) return 1;
    
    // Compare major.minor.patch
    if (a->major != b->major) return (a->major > b->major) ? 1 : -1;
    if (a->minor != b->minor) return (a->minor > b->minor) ? 1 : -1;
    if (a->patch != b->patch) return (a->patch > b->patch) ? 1 : -1;
    
    // Equal versions, check prerelease
    if (!a->prerelease && !b->prerelease) return 0;
    if (a->prerelease && !b->prerelease) return -1;  // Prerelease comes before release
    if (!a->prerelease && b->prerelease) return 1;   // Release comes after prerelease
    
    // Both have prerelease, compare them lexically
    return strcmp(a->prerelease, b->prerelease);
}

/**
 * Parse a version constraint string
 * 
 * @param constraint_str The constraint string (e.g., ">=1.0.0", "^2.3.4")
 * @return A pointer to a newly allocated NexusVersionConstraint, or NULL on error
 */
NexusVersionConstraint* nexus_version_constraint_parse(const char* constraint_str) {
    if (!constraint_str || !*constraint_str) return NULL;
    
    NexusVersionConstraint* constraint = (NexusVersionConstraint*)malloc(sizeof(NexusVersionConstraint));
    if (!constraint) return NULL;
    
    // Default to equality
    constraint->op = NEXUS_VERSION_OP_EQ;
    
    // Check for wildcard
    if (strcmp(constraint_str, "*") == 0 || strcmp(constraint_str, "latest") == 0) {
        constraint->op = NEXUS_VERSION_OP_ANY;
        constraint->version = nexus_version_parse("0.0.0");
        return constraint;
    }
    
    // Extract operator and version
    const char* version_start = constraint_str;
    
    // Check for operator prefix
    if (constraint_str[0] == '^') {
        constraint->op = NEXUS_VERSION_OP_CARET;
        version_start = constraint_str + 1;
    } else if (constraint_str[0] == '~') {
        constraint->op = NEXUS_VERSION_OP_TILDE;
        version_start = constraint_str + 1;
    } else if (constraint_str[0] == '>' && constraint_str[1] == '=') {
        constraint->op = NEXUS_VERSION_OP_GE;
        version_start = constraint_str + 2;
    } else if (constraint_str[0] == '<' && constraint_str[1] == '=') {
        constraint->op = NEXUS_VERSION_OP_LE;
        version_start = constraint_str + 2;
    } else if (constraint_str[0] == '>') {
        constraint->op = NEXUS_VERSION_OP_GT;
        version_start = constraint_str + 1;
    } else if (constraint_str[0] == '<') {
        constraint->op = NEXUS_VERSION_OP_LT;
        version_start = constraint_str + 1;
    } else if (constraint_str[0] == '=') {
        constraint->op = NEXUS_VERSION_OP_EQ;
        version_start = constraint_str + 1;
    }
    
    // Skip whitespace
    while (isspace(*version_start)) version_start++;
    
    // Parse the version
    constraint->version = nexus_version_parse(version_start);
    if (!constraint->version) {
        free(constraint);
        return NULL;
    }
    
    return constraint;
}

/**
 * Check if a version satisfies a constraint
 * 
 * @param version The version to check
 * @param constraint The constraint to check against
 * @return true if the version satisfies the constraint, false otherwise
 */
bool nexus_version_satisfies(const NexusVersion* version, const NexusVersionConstraint* constraint) {
    if (!version || !constraint || !constraint->version) return false;
    
    // Handle wildcard constraint
    if (constraint->op == NEXUS_VERSION_OP_ANY) return true;
    
    // Compare according to operator
    int comparison = nexus_version_compare(version, constraint->version);
    
    switch (constraint->op) {
        case NEXUS_VERSION_OP_EQ:
            return comparison == 0;
        
        case NEXUS_VERSION_OP_GT:
            return comparison > 0;
        
        case NEXUS_VERSION_OP_GE:
            return comparison >= 0;
        
        case NEXUS_VERSION_OP_LT:
            return comparison < 0;
        
        case NEXUS_VERSION_OP_LE:
            return comparison <= 0;
        
        case NEXUS_VERSION_OP_CARET:
            // ^1.2.3 means >=1.2.3 <2.0.0
            if (version->major != constraint->version->major) return false;
            if (constraint->version->major == 0) {
                // For 0.x.y, only allow exact minor version
                if (version->minor != constraint->version->minor) return false;
                // For 0.0.z, only allow exact patch version
                if (constraint->version->minor == 0) {
                    return version->patch >= constraint->version->patch;
                } else {
                    return version->patch >= constraint->version->patch;
                }
            }
            return (comparison >= 0);
        
        case NEXUS_VERSION_OP_TILDE:
            // ~1.2.3 means >=1.2.3 <1.3.0
            if (version->major != constraint->version->major) return false;
            if (version->minor != constraint->version->minor) return false;
            return version->patch >= constraint->version->patch;
        
        default:
            return false;
    }
}

/**
 * Check if a version string satisfies a constraint string
 * 
 * @param version_str The version string to check
 * @param constraint_str The constraint string to check against
 * @return true if the version satisfies the constraint, false otherwise
 */
bool nexus_version_string_satisfies(const char* version_str, const char* constraint_str) {
    NexusVersion* version = nexus_version_parse(version_str);
    if (!version) return false;
    
    NexusVersionConstraint* constraint = nexus_version_constraint_parse(constraint_str);
    if (!constraint) {
        nexus_version_free(version);
        return false;
    }
    
    bool result = nexus_version_satisfies(version, constraint);
    
    nexus_version_free(version);
    nexus_version_constraint_free(constraint);
    
    return result;
}

/**
 * Free a version structure
 * 
 * @param version The version to free
 */
void nexus_version_free(NexusVersion* version) {
    if (!version) return;
    
    free(version->prerelease);
    free(version->build);
    free(version);
}

/**
 * Free a version constraint structure
 * 
 * @param constraint The constraint to free
 */
void nexus_version_constraint_free(NexusVersionConstraint* constraint) {
    if (!constraint) return;
    
    nexus_version_free(constraint->version);
    free(constraint);
}

/**
 * Convert a version to a string
 * 
 * @param version The version to convert
 * @return A newly allocated string representation of the version, or NULL on error
 */
char* nexus_version_to_string(const NexusVersion* version) {
    if (!version) return NULL;
    
    // Handle wildcard version
    if (version->major < 0) {
        return strdup("*");
    }
    
    // Calculate size needed for string
    size_t size = 32;  // Base size for major.minor.patch
    if (version->prerelease) size += strlen(version->prerelease) + 1;  // +1 for '-'
    if (version->build) size += strlen(version->build) + 1;  // +1 for '+'
    
    char* str = (char*)malloc(size);
    if (!str) return NULL;
    
    // Format major.minor.patch
    int pos = sprintf(str, "%d.%d.%d", version->major, version->minor, version->patch);
    
    // Add prerelease if present
    if (version->prerelease) {
        str[pos++] = '-';
        strcpy(str + pos, version->prerelease);
        pos += strlen(version->prerelease);
    }
    
    // Add build if present
    if (version->build) {
        str[pos++] = '+';
        strcpy(str + pos, version->build);
    }
    
    return str;
}