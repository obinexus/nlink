// src/core/semverx/semver.c
// OBINexus SemVerX: Semantic Versioning Coordination Protocol
#define _POSIX_C_SOURCE 200809L

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Constitutional version structure
typedef struct {
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
    char *prerelease;
    char *metadata;
} semver_t;

typedef enum {
    SEMVER_SUCCESS = 0,
    SEMVER_ERROR_INVALID_FORMAT = -1,
    SEMVER_ERROR_MEMORY_ALLOCATION = -2,
    SEMVER_ERROR_INVALID_PARAMETER = -3
} semver_result_t;

// Constitutional version parsing
semver_result_t semver_parse(const char *version_string, semver_t *version) {
    if (!version_string || !version) {
        return SEMVER_ERROR_INVALID_PARAMETER;
    }
    
    // Simple constitutional parsing implementation
    int result = sscanf(version_string, "%u.%u.%u", 
                       &version->major, &version->minor, &version->patch);
    
    if (result != 3) {
        return SEMVER_ERROR_INVALID_FORMAT;
    }
    
    version->prerelease = NULL;
    version->metadata = NULL;
    
    return SEMVER_SUCCESS;
}

// Constitutional version comparison
int semver_compare(const semver_t *a, const semver_t *b) {
    if (!a || !b) return 0;
    
    if (a->major != b->major) {
        return (a->major > b->major) ? 1 : -1;
    }
    
    if (a->minor != b->minor) {
        return (a->minor > b->minor) ? 1 : -1;
    }
    
    if (a->patch != b->patch) {
        return (a->patch > b->patch) ? 1 : -1;
    }
    
    return 0; // Equal versions
}

// Constitutional version string generation
semver_result_t semver_to_string(const semver_t *version, char *buffer, size_t buffer_size) {
    if (!version || !buffer || buffer_size == 0) {
        return SEMVER_ERROR_INVALID_PARAMETER;
    }
    
    int result = snprintf(buffer, buffer_size, "%u.%u.%u", 
                         version->major, version->minor, version->patch);
    
    if (result < 0 || (size_t)result >= buffer_size) {
        return SEMVER_ERROR_INVALID_FORMAT;
    }
    
    return SEMVER_SUCCESS;
}
