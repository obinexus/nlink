#ifndef NEXUS_VERSION_H
#define NEXUS_VERSION_H

#include "nlink/core/common/types.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>


#ifdef __cplusplus
extern "C" {
#endif



/**
 * @brief Semantic version structure
 */
typedef struct NexusVersion {
    int major;           /**< Major version number */
    int minor;           /**< Minor version number */
    int patch;           /**< Patch version number */
    char* prerelease;    /**< Pre-release identifier (can be NULL) */
    char* build;         /**< Build metadata (can be NULL) */
} NexusVersion;

/**
 * @brief Version constraint operator
 */
typedef enum {
    NEXUS_VERSION_OP_EQ,     /**< Equal (=) */
    NEXUS_VERSION_OP_LT,     /**< Less than (<) */
    NEXUS_VERSION_OP_LE,     /**< Less than or equal (<=) */
    NEXUS_VERSION_OP_GT,     /**< Greater than (>) */
    NEXUS_VERSION_OP_GE,     /**< Greater than or equal (>=) */
    NEXUS_VERSION_OP_TILDE,  /**< Tilde (~) - Compatible with minor */
    NEXUS_VERSION_OP_CARET,  /**< Caret (^) - Compatible with major */
    NEXUS_VERSION_OP_ANY     /**< Any version (*) */
} NexusVersionOperator;

/**
 * @brief Version constraint structure
 */
typedef struct NexusVersionConstraint {
    NexusVersionOperator op;    /**< Constraint operator */
    NexusVersion* version;      /**< Version to compare against */
} NexusVersionConstraint;

/* Function declarations only - definitions are in nexus_version.c */

/**
 * @brief Parse a version string into a version structure
 * 
 * @param version_str Version string (e.g., "1.2.3-alpha+build.123")
 * @return NexusVersion* Parsed version or NULL on error
 */
extern NexusVersion* nexus_version_parse(const char* version_str);

/**
 * @brief Compare two versions
 * 
NexusVersion* nexus_version_parse(const char* version_str);

/**
 * @brief Compare two versions
 * 
 * @param v1 First version
 * @param v2 Second version
 * @return int -1 if v1 < v2, 0 if v1 == v2, 1 if v1 > v2
 */
int nexus_version_compare(const NexusVersion* v1, const NexusVersion* v2);

/**
 * @brief Parse a version constraint string
 * 
 * @param constraint_str Constraint string (e.g., ">=1.2.3", "^2.0.0", "~1.2.0")
 * @return NexusVersionConstraint* Parsed constraint or NULL on error
 */
NexusVersionConstraint* nexus_version_constraint_parse(const char* constraint_str);

/**
 * @brief Check if a version satisfies a constraint
 * 
 * @param version Version to check
 * @param constraint Constraint to check against
 * @return bool True if version satisfies constraint, false otherwise
 */
bool nexus_version_satisfies(const NexusVersion* version, const NexusVersionConstraint* constraint);

/**
 * @brief Check if a version string satisfies a constraint string
 * 
 * @param version_str Version string
 * @param constraint_str Constraint string
 * @return bool True if version satisfies constraint, false otherwise
 */
bool nexus_version_string_satisfies(const char* version_str, const char* constraint_str);

/**
 * @brief Free version resources
 * 
 * @param version Version to free
 */
void nexus_version_free(NexusVersion* version);

/**
 * @brief Free constraint resources
 * 
 * @param constraint Constraint to free
 */
void nexus_version_constraint_free(NexusVersionConstraint* constraint);

/**
 * @brief Convert version to string
 * 
 * @param version Version to convert
 * @return char* Newly allocated string representation
 */
char* nexus_version_to_string(const NexusVersion* version);
#ifdef __cplusplus
}
#endif

#endif /* NEXUS_VERSION_H */
