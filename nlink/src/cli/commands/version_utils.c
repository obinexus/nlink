/**
 * @file version_utils.c
 * @brief Implementation of version utilities for NexusLink CLI
 * 
 * This file provides the implementation of utility functions
 * for semantic versioning in the NexusLink CLI system, which
 * wrap the core version functions.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nlink/cli/commands/version_utils.h"
 /* Core versioning functionality - ONLY INCLUDE, DO NOT REDEFINE */
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 
 /**
  * @brief Check if a version string satisfies a constraint string
  * 
  * @param version_str The version string to check
  * @param constraint_str The constraint string to check against
  * @return true if the version satisfies the constraint, false otherwise
  */
 bool nexus_cli_version_check(const char* version_str, const char* constraint_str) {
     // Use the core functionality
     return nexus_version_string_satisfies(version_str, constraint_str);
 }
 
 /**
  * @brief Get a formatted string describing a version
  * 
  * @param version_str The version string to format
  * @return A newly allocated formatted string, or NULL on error
  */
 char* nexus_cli_format_version(const char* version_str) {
     // Parse the version using core function
     NexusVersion* version = nexus_version_parse(version_str);
     if (!version) {
         return NULL;
     }
     
     // Get the version as a normalized string
     char* normal_version = nexus_version_to_string(version);
     if (!normal_version) {
         nexus_version_free(version);
         return NULL;
     }
     
     // Allocate space for the formatted string (with some extra room)
     char* formatted = (char*)malloc(strlen(normal_version) + 64);
     if (!formatted) {
         nexus_version_free(version);
         free(normal_version);
         return NULL;
     }
     
     // Format the string
     if (version->prerelease) {
         sprintf(formatted, "v%s (pre-release)", normal_version);
     } else if (version->build) {
         sprintf(formatted, "v%s (build: %s)", normal_version, version->build);
     } else {
         sprintf(formatted, "v%s", normal_version);
     }
     
     // Clean up
     nexus_version_free(version);
     free(normal_version);
     
     return formatted;
 }
 
 /**
  * @brief Compare two version strings
  * 
  * @param v1_str First version string
  * @param v2_str Second version string
  * @return -1 if v1 < v2, 0 if v1 == v2, 1 if v1 > v2
  */
 int nexus_cli_compare_versions(const char* v1_str, const char* v2_str) {
     // Parse both versions using core functions
     NexusVersion* v1 = nexus_version_parse(v1_str);
     NexusVersion* v2 = nexus_version_parse(v2_str);
     
     if (!v1 || !v2) {
         // Handle error: couldn't parse one or both versions
         if (v1) nexus_version_free(v1);
         if (v2) nexus_version_free(v2);
         return 0;  // Return equality on error
     }
     
     // Compare versions using core function
     int result = nexus_version_compare(v1, v2);
     
     // Clean up
     nexus_version_free(v1);
     nexus_version_free(v2);
     
     return result;
 }