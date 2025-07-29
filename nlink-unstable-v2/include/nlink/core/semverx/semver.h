// include/nlink/core/semverx/semver.h
// OBINexus SemVerX: Semantic Versioning Coordination Interface
#ifndef OBINEXUS_NLINK_CORE_SEMVERX_SEMVER_H
#define OBINEXUS_NLINK_CORE_SEMVERX_SEMVER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Constitutional Version Structure
typedef struct {
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
    char *prerelease;
    char *metadata;
} semver_t;

// Constitutional SemVer Result System
typedef enum {
    SEMVER_SUCCESS = 0,
    SEMVER_ERROR_INVALID_FORMAT = -1,
    SEMVER_ERROR_MEMORY_ALLOCATION = -2,
    SEMVER_ERROR_INVALID_PARAMETER = -3,
    SEMVER_ERROR_VERSION_CONFLICT = -4
} semver_result_t;

// Constitutional Version Operations
semver_result_t semver_parse(const char *version_string, semver_t *version);
semver_result_t semver_to_string(const semver_t *version, char *buffer, size_t buffer_size);
int semver_compare(const semver_t *a, const semver_t *b);

// Advanced Version Coordination
semver_result_t semver_increment_major(semver_t *version);
semver_result_t semver_increment_minor(semver_t *version);
semver_result_t semver_increment_patch(semver_t *version);

// Constitutional Version Validation
bool semver_is_compatible(const semver_t *required, const semver_t *available);
bool semver_satisfies_range(const semver_t *version, const char *range_spec);

// Memory Management
void semver_free(semver_t *version);
semver_result_t semver_clone(const semver_t *source, semver_t *destination);

#ifdef __cplusplus
}
#endif

#endif // OBINEXUS_NLINK_CORE_SEMVERX_SEMVER_H
