/**
 * @file test_semver.c
 * @brief Unit tests for semantic versioning
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nexus_semver.h"
 #include "../common/test_common.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 
 void test_semver_parsing() {
     // Test basic version parsing
     SemVer* ver = semver_parse("1.2.3");
     test_assert("Basic version parsing", 
                 ver != NULL && 
                 ver->major == 1 && 
                 ver->minor == 2 && 
                 ver->patch == 3);
     semver_free(ver);
     
     // Test with pre-release
     ver = semver_parse("1.2.3-alpha.1");
     test_assert("Version with pre-release", 
                 ver != NULL && 
                 ver->major == 1 && 
                 ver->minor == 2 && 
                 ver->patch == 3 && 
                 strcmp(ver->prerelease, "alpha.1") == 0);
     semver_free(ver);
     
     // Test with build metadata
     ver = semver_parse("1.2.3+build.123");
     test_assert("Version with build metadata", 
                 ver != NULL && 
                 ver->major == 1 && 
                 ver->minor == 2 && 
                 ver->patch == 3 && 
                 strcmp(ver->build, "build.123") == 0);
     semver_free(ver);
     
     // Test with pre-release and build metadata
     ver = semver_parse("1.2.3-alpha.1+build.123");
     test_assert("Version with pre-release and build", 
                 ver != NULL && 
                 ver->major == 1 && 
                 ver->minor == 2 && 
                 ver->patch == 3 && 
                 strcmp(ver->prerelease, "alpha.1") == 0 && 
                 strcmp(ver->build, "build.123") == 0);
     semver_free(ver);
     
     // Test wildcard
     ver = semver_parse("*");
     test_assert("Wildcard version", 
                 ver != NULL && 
                 ver->is_wildcard);
     semver_free(ver);
     
     // Test invalid versions
     test_assert("Invalid version 'a.b.c'", 
                 semver_parse("a.b.c") == NULL);
     
     test_assert("Invalid version '1.2'", 
                 semver_parse("1.2") == NULL);
     
     test_assert("Invalid version '1.2.3.'", 
                 semver_parse("1.2.3.") == NULL);
 }
 
 void test_semver_comparison() {
     // Test equal versions
     SemVer* ver1 = semver_parse("1.2.3");
     SemVer* ver2 = semver_parse("1.2.3");
     test_assert("Equal versions", semver_compare(ver1, ver2) == 0);
     semver_free(ver1);
     semver_free(ver2);
     
     // Test major version difference
     ver1 = semver_parse("2.0.0");
     ver2 = semver_parse("1.0.0");
     test_assert("Major version difference", semver_compare(ver1, ver2) > 0);
     test_assert("Major version difference (reversed)", semver_compare(ver2, ver1) < 0);
     semver_free(ver1);
     semver_free(ver2);
     
     // Test minor version difference
     ver1 = semver_parse("1.2.0");
     ver2 = semver_parse("1.1.0");
     test_assert("Minor version difference", semver_compare(ver1, ver2) > 0);
     test_assert("Minor version difference (reversed)", semver_compare(ver2, ver1) < 0);
     semver_free(ver1);
     semver_free(ver2);
     
     // Test patch version difference
     ver1 = semver_parse("1.0.1");
     ver2 = semver_parse("1.0.0");
     test_assert("Patch version difference", semver_compare(ver1, ver2) > 0);
     test_assert("Patch version difference (reversed)", semver_compare(ver2, ver1) < 0);
     semver_free(ver1);
     semver_free(ver2);
     
     // Test pre-release versions
     ver1 = semver_parse("1.0.0-alpha");
     ver2 = semver_parse("1.0.0");
     test_assert("Pre-release vs normal", semver_compare(ver1, ver2) < 0);
     test_assert("Pre-release vs normal (reversed)", semver_compare(ver2, ver1) > 0);
     semver_free(ver1);
     semver_free(ver2);
     
     // Test pre-release ordering
     ver1 = semver_parse("1.0.0-alpha.2");
     ver2 = semver_parse("1.0.0-alpha.1");
     test_assert("Pre-release ordering", semver_compare(ver1, ver2) > 0);
     semver_free(ver1);
     semver_free(ver2);
     
     // Test wildcard versions
     ver1 = semver_parse("*");
     ver2 = semver_parse("1.0.0");
     test_assert("Wildcard vs specific", semver_compare(ver1, ver2) < 0);
     test_assert("Wildcard vs specific (reversed)", semver_compare(ver2, ver1) > 0);
     semver_free(ver1);
     semver_free(ver2);
     
     // Test two wildcards
     ver1 = semver_parse("*");
     ver2 = semver_parse("*");
     test_assert("Wildcard vs wildcard", semver_compare(ver1, ver2) == 0);
     semver_free(ver1);
     semver_free(ver2);
 }
 
 void test_semver_satisfies() {
     // Test exact match
     test_assert("Exact match '1.2.3' satisfies '1.2.3'", 
                 semver_satisfies("1.2.3", "1.2.3"));
     
     // Test wildcard
     test_assert("Wildcard '*' satisfies any version", 
                 semver_satisfies("2.0.0", "*"));
     
     // Test greater than
     test_assert("Version '2.0.0' satisfies '>1.0.0'", 
                 semver_satisfies("2.0.0", ">1.0.0"));
     
     test_assert("Version '1.0.0' does not satisfy '>1.0.0'", 
                 !semver_satisfies("1.0.0", ">1.0.0"));
     
     // Test greater than or equal
     test_assert("Version '2.0.0' satisfies '>=1.0.0'", 
                 semver_satisfies("2.0.0", ">=1.0.0"));
     
     test_assert("Version '1.0.0' satisfies '>=1.0.0'", 
                 semver_satisfies("1.0.0", ">=1.0.0"));
     
     test_assert("Version '0.9.0' does not satisfy '>=1.0.0'", 
                 !semver_satisfies("0.9.0", ">=1.0.0"));
     
     // Test less than
     test_assert("Version '0.9.0' satisfies '<1.0.0'", 
                 semver_satisfies("0.9.0", "<1.0.0"));
     
     test_assert("Version '1.0.0' does not satisfy '<1.0.0'", 
                 !semver_satisfies("1.0.0", "<1.0.0"));
     
     // Test less than or equal
     test_assert("Version '0.9.0' satisfies '<=1.0.0'", 
                 semver_satisfies("0.9.0", "<=1.0.0"));
     
     test_assert("Version '1.0.0' satisfies '<=1.0.0'", 
                 semver_satisfies("1.0.0", "<=1.0.0"));
     
     test_assert("Version '1.0.1' does not satisfy '<=1.0.0'", 
                 !semver_satisfies("1.0.1", "<=1.0.0"));
     
     // Test caret ranges (compatible with: same major version, >= specific version)
     test_assert("Version '1.2.3' satisfies '^1.2.0'", 
                 semver_satisfies("1.2.3", "^1.2.0"));
     
     test_assert("Version '1.3.0' satisfies '^1.2.0'", 
                 semver_satisfies("1.3.0", "^1.2.0"));
     
     test_assert("Version '2.0.0' does not satisfy '^1.2.0'", 
                 !semver_satisfies("2.0.0", "^1.2.0"));
     
     test_assert("Version '1.1.0' does not satisfy '^1.2.0'", 
                 !semver_satisfies("1.1.0", "^1.2.0"));
     
     // Test tilde ranges (compatible with: same major.minor version, >= specific version)
     test_assert("Version '1.2.3' satisfies '~1.2.0'", 
                 semver_satisfies("1.2.3", "~1.2.0"));
     
     test_assert("Version '1.2.9' satisfies '~1.2.0'", 
                 semver_satisfies("1.2.9", "~1.2.0"));
     
     test_assert("Version '1.3.0' does not satisfy '~1.2.0'", 
                 !semver_satisfies("1.3.0", "~1.2.0"));
 }
 
 int main() {
     test_init("SemVer");
     
     test_semver_parsing();
     test_semver_comparison();
     test_semver_satisfies();
     
     return test_finalize();
 }