/**
 * @file test_semver.c
 * @brief Unit tests for semantic versioning
 * 
 * Copyright © 2025 OBINexus Computing
 */

#include "nexus/semver.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// Test semantic version parsing
void test_semver_parse() {
    SemVer* ver = semver_parse("1.2.3");
    assert(ver != NULL);
    assert(ver->major == 1);
    assert(ver->minor == 2);
    assert(ver->patch == 3);
    assert(ver->prerelease == NULL);
    assert(ver->build == NULL);
    assert(ver->is_wildcard == false);
    semver_free(ver);
    
    printf("test_semver_parse passed\n");
}

// Test semantic version comparison
void test_semver_compare() {
    SemVer* v1 = semver_parse("1.2.3");
    SemVer* v2 = semver_parse("1.3.0");
    
    assert(semver_compare(v1, v2) < 0);  // 1.2.3 < 1.3.0
    assert(semver_compare(v2, v1) > 0);  // 1.3.0 > 1.2.3
    
    SemVer* v3 = semver_parse("1.2.3");
    assert(semver_compare(v1, v3) == 0); // 1.2.3 == 1.2.3
    
    semver_free(v1);
    semver_free(v2);
    semver_free(v3);
    
    printf("test_semver_compare passed\n");
}

// Test semantic version constraint satisfaction
void test_semver_satisfies() {
    assert(semver_satisfies("1.2.3", "1.2.3") == true);    // Exact match
    assert(semver_satisfies("1.2.3", "^1.0.0") == true);   // ^1.0.0 means >= 1.0.0 and < 2.0.0
    assert(semver_satisfies("2.0.0", "^1.0.0") == false);  // 2.0.0 is not < 2.0.0
    assert(semver_satisfies("1.3.0", "~1.2.0") == false);  // ~1.2.0 means >= 1.2.0 and < 1.3.0
    assert(semver_satisfies("1.2.9", "~1.2.0") == true);   // 1.2.9 is >= 1.2.0 and < 1.3.0
    
    printf("test_semver_satisfies passed\n");
}

int main() {
    test_semver_parse();
    test_semver_compare();
    test_semver_satisfies();
    
    printf("All semantic versioning tests passed!\n");
    return 0;
}
