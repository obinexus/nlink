/**
 * @file version_utils.h
 * @brief Version utilities for NexusLink CLI
 * 
 * This file provides utility wrapper functions around the core versioning
 * functions from nexus_version.h to be used by CLI components.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #ifndef NEXUS_CLI_VERSION_UTILS_H
 #define NEXUS_CLI_VERSION_UTILS_H
 
 #include "nlink/core/common/types.h"
 #include "nlink/core/common/result.h"
 #include "nlink/core/versioning/nexus_version.h"  /* Include the canonical version definitions */
 #include <stdbool.h>
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 /**
  * @brief Check if a version string satisfies a constraint string
  * 
  * @param version_str The version string to check
  * @param constraint_str The constraint string to check against
  * @return true if the version satisfies the constraint, false otherwise
  */
 bool nexus_cli_version_check(const char* version_str, const char* constraint_str);
 
 /**
  * @brief Get a formatted string describing a version
  * 
  * @param version_str The version string to format
  * @return A newly allocated formatted string, or NULL on error
  */
 char* nexus_cli_format_version(const char* version_str);
 
 /**
  * @brief Compare two version strings
  * 
  * @param v1_str First version string
  * @param v2_str Second version string
  * @return -1 if v1 < v2, 0 if v1 == v2, 1 if v1 > v2
  */
 int nexus_cli_compare_versions(const char* v1_str, const char* v2_str);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* NEXUS_CLI_VERSION_UTILS_H */