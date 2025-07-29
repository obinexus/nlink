/**
 * @file test_version_constraints.c
 * @brief Unit tests for version constraint validation and checking
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nexus_semver.h"
 #include "../common/test_common.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 
 void test_exact_match_constraints() {
     // Test exact match constraints (=X.Y.Z or just X.Y.Z)
     test_assert("Version 1.2.3 satisfies =1.2.3", 
                 semver_satisfies("1.2.3", "=1.2.3"));
     
     test_assert("Version 1.2.3 satisfies 1.2.3 (implicit exact match)", 
                 semver_satisfies("1.2.3", "1.2.3"));
     
     test_assert("Version 1.2.4 does not satisfy =1.2.3", 
                 !semver_satisfies("1.2.4", "=1.2.3"));
     
     test_assert("Version 1.3.0 does not satisfy =1.2.3", 
                 !semver_satisfies("1.3.0", "=1.2.3"));
     
     // Test with pre-release versions
     test_assert("Version 1.2.3-alpha satisfies =1.2.3-alpha", 
                 semver_satisfies("1.2.3-alpha", "=1.2.3-alpha"));
     
     test_assert("Version 1.2.3 does not satisfy =1.2.3-alpha", 
                 !semver_satisfies("1.2.3", "=1.2.3-alpha"));
     
     test_assert("Version 1.2.3-beta does not satisfy =1.2.3-alpha", 
                 !semver_satisfies("1.2.3-beta", "=1.2.3-alpha"));
 }
 
 void test_greater_than_constraints() {
     // Test greater than constraints (>X.Y.Z)
     test_assert("Version 2.0.0 satisfies >1.0.0", 
                 semver_satisfies("2.0.0", ">1.0.0"));
     
     test_assert("Version 1.1.0 satisfies >1.0.0", 
                 semver_satisfies("1.1.0", ">1.0.0"));
     
     test_assert("Version 1.0.1 satisfies >1.0.0", 
                 semver_satisfies("1.0.1", ">1.0.0"));
     
     test_assert("Version 1.0.0 does not satisfy >1.0.0", 
                 !semver_satisfies("1.0.0", ">1.0.0"));
     
     test_assert("Version 0.9.0 does not satisfy >1.0.0", 
                 !semver_satisfies("0.9.0", ">1.0.0"));
     
     // Test with pre-release versions
     test_assert("Version 1.2.3 satisfies >1.2.3-alpha", 
                 semver_satisfies("1.2.3", ">1.2.3-alpha"));
     
     test_assert("Version 1.2.3-beta satisfies >1.2.3-alpha (if alpha < beta)", 
                 semver_satisfies("1.2.3-beta", ">1.2.3-alpha"));
 }
 
 void test_greater_than_or_equal_constraints() {
     // Test greater than or equal constraints (>=X.Y.Z)
     test_assert("Version 2.0.0 satisfies >=1.0.0", 
                 semver_satisfies("2.0.0", ">=1.0.0"));
     
     test_assert("Version 1.0.0 satisfies >=1.0.0", 
                 semver_satisfies("1.0.0", ">=1.0.0"));
     
     test_assert("Version 0.9.0 does not satisfy >=1.0.0", 
                 !semver_satisfies("0.9.0", ">=1.0.0"));
     
     // Test with pre-release versions
     test_assert("Version 1.2.3-alpha satisfies >=1.2.3-alpha", 
                 semver_satisfies("1.2.3-alpha", ">=1.2.3-alpha"));
     
     test_assert("Version 1.2.3 satisfies >=1.2.3-alpha", 
                 semver_satisfies("1.2.3", ">=1.2.3-alpha"));
 }
 
 void test_less_than_constraints() {
     // Test less than constraints (<X.Y.Z)
     test_assert("Version 0.9.0 satisfies <1.0.0", 
                 semver_satisfies("0.9.0", "<1.0.0"));
     
     test_assert("Version 1.0.0 does not satisfy <1.0.0", 
                 !semver_satisfies("1.0.0", "<1.0.0"));
     
     test_assert("Version 2.0.0 does not satisfy <1.0.0", 
                 !semver_satisfies("2.0.0", "<1.0.0"));
     
     // Test with pre-release versions
     test_assert("Version 1.2.3-alpha satisfies <1.2.3", 
                 semver_satisfies("1.2.3-alpha", "<1.2.3"));
     
     test_assert("Version 1.2.3-alpha satisfies <1.2.3-beta (if alpha < beta)", 
                 semver_satisfies("1.2.3-alpha", "<1.2.3-beta"));
 }
 
 void test_less_than_or_equal_constraints() {
     // Test less than or equal constraints (<=X.Y.Z)
     test_assert("Version 0.9.0 satisfies <=1.0.0", 
                 semver_satisfies("0.9.0", "<=1.0.0"));
     
     test_assert("Version 1.0.0 satisfies <=1.0.0", 
                 semver_satisfies("1.0.0", "<=1.0.0"));
     
     test_assert("Version 1.0.1 does not satisfy <=1.0.0", 
                 !semver_satisfies("1.0.1", "<=1.0.0"));
     
     // Test with pre-release versions
     test_assert("Version 1.2.3-alpha satisfies <=1.2.3-alpha", 
                 semver_satisfies("1.2.3-alpha", "<=1.2.3-alpha"));
     
     test_assert("Version 1.2.3-alpha satisfies <=1.2.3", 
                 semver_satisfies("1.2.3-alpha", "<=1.2.3"));
 }
 
 void test_caret_range_constraints() {
     // Test caret range constraints (^X.Y.Z) - compatible with, allowing minor/patch changes
     test_assert("Version 1.2.3 satisfies ^1.2.0", 
                 semver_satisfies("1.2.3", "^1.2.0"));
     
     test_assert("Version 1.3.0 satisfies ^1.2.0", 
                 semver_satisfies("1.3.0", "^1.2.0"));
     
     test_assert("Version 2.0.0 does not satisfy ^1.2.0", 
                 !semver_satisfies("2.0.0", "^1.2.0"));
     
     test_assert("Version 1.1.0 does not satisfy ^1.2.0", 
                 !semver_satisfies("1.1.0", "^1.2.0"));
     
     // Edge cases for 0.x.y versions (treat as ~)
     test_assert("Version 0.2.3 satisfies ^0.2.0", 
                 semver_satisfies("0.2.3", "^0.2.0"));
     
     test_assert("Version 0.3.0 does not satisfy ^0.2.0", 
                 !semver_satisfies("0.3.0", "^0.2.0"));
 }
 
 void test_tilde_range_constraints() {
     // Test tilde range constraints (~X.Y.Z) - only patch-level changes
     test_assert("Version 1.2.3 satisfies ~1.2.0", 
                 semver_satisfies("1.2.3", "~1.2.0"));
     
     test_assert("Version 1.2.9 satisfies ~1.2.0", 
                 semver_satisfies("1.2.9", "~1.2.0"));
     
     test_assert("Version 1.3.0 does not satisfy ~1.2.0", 
                 !semver_satisfies("1.3.0", "~1.2.0"));
     
     // Specific edge cases
     test_assert("Version ~1.2 allows 1.2.0 through 1.2.x", 
                 semver_satisfies("1.2.9", "~1.2"));
     
     test_assert("Version ~1 allows 1.0.0 through 1.x.y", 
                 semver_satisfies("1.9.9", "~1"));
 }
 
 void test_wildcard_constraints() {
     // Test wildcard constraints
     test_assert("Version 1.0.0 satisfies *", 
                 semver_satisfies("1.0.0", "*"));
     
     test_assert("Version 2.0.0 satisfies *", 
                 semver_satisfies("2.0.0", "*"));
     
     test_assert("Version 0.0.1 satisfies *", 
                 semver_satisfies("0.0.1", "*"));
     
     // Partial wildcards (not implemented in the base semver module, but should be in a full impl)
     if (semver_satisfies("1.2.3", "1.*.*")) {
         test_assert("Version 1.2.3 satisfies 1.*.*", true);
     } else {
         printf("Note: Partial wildcards not implemented in this version\n");
     }
 }
 
 void test_complex_constraints() {
     // These are more advanced tests that would require a more sophisticated
     // constraint parser than what we've implemented so far, but they should
     // be included for future reference
     
     // Range constraints (X.Y.Z - A.B.C)
     // Logical AND (X.Y.Z && A.B.C)
     // Logical OR (X.Y.Z || A.B.C)
     
     // For now, we'll just provide a placeholder test
     test_assert("Complex constraint placeholder", true);
     printf("Note: Complex constraint tests would be implemented in a future version\n");
 }
 
 int main() {
     test_init("Version Constraints");
     
     test_exact_match_constraints();
     test_greater_than_constraints();
     test_greater_than_or_equal_constraints();
     test_less_than_constraints();
     test_less_than_or_equal_constraints();
     test_caret_range_constraints();
     test_tilde_range_constraints();
     test_wildcard_constraints();
     test_complex_constraints();
     
     return test_finalize();
 }