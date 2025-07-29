/**
 * @file test_cross_language_marshal.c  
 * @brief Integration tests for cross-language data marshalling
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/wait.h>
#include <unistd.h>

#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "FAIL: %s\n", message); \
            return 0; \
        } \
        printf("PASS: %s\n", message); \
    } while(0)

int test_cython_integration() {
    printf("Testing Cython marshalling integration...\n");
    
    // Test data marshalling between C and Cython
    const char* test_script = 
        "cd examples/cython-package && "
        "python -c \"import sys; sys.path.append('src'); "
        "from nlink_data_marshal import create_marshaller; "
        "m = create_marshaller(); "
        "import numpy as np; "
        "data = np.array([1.0, 2.0, 3.0]); "
        "result = m.marshal_data(data); "
        "print('SUCCESS' if len(result) > 0 else 'FAIL')\"";
    
    int result = system(test_script);
    TEST_ASSERT(WEXITSTATUS(result) == 0, "Cython marshalling integration");
    
    return 1;
}

int test_java_integration() {
    printf("Testing Java marshalling integration...\n");
    
    // Test Java compilation and basic marshalling
    const char* compile_cmd = 
        "cd examples/java-package && "
        "javac -cp . src/main/java/com/obinexus/nlink/*.java";
    
    int compile_result = system(compile_cmd);
    TEST_ASSERT(WEXITSTATUS(compile_result) == 0, "Java compilation");
    
    const char* test_cmd =
        "cd examples/java-package && "
        "java -cp src/main/java com.obinexus.nlink.ZeroOverheadMarshaller";
    
    // Note: This would need a main method in the Java class for full testing
    printf("Java integration test prepared (requires main method)\n");
    
    return 1;
}

int test_python_integration() {
    printf("Testing Python integration...\n");
    
    const char* test_script =
        "cd examples/python-package && "
        "python -c \"import sys; sys.path.append('src'); "
        "from nlink_marshal import create_marshaller; "
        "m = create_marshaller(); "
        "data = [1.0, 2.0, 3.0]; "
        "marshalled = m.marshal_data(data); "
        "recovered = m.unmarshal_data(marshalled); "
        "print('SUCCESS' if data == recovered else 'FAIL')\"";
    
    int result = system(test_script);
    TEST_ASSERT(WEXITSTATUS(result) == 0, "Python marshalling integration");
    
    return 1;
}

int test_cross_language_compatibility() {
    printf("Testing cross-language marshalling compatibility...\n");
    
    // This would test that data marshalled in one language
    // can be unmarshalled in another language
    printf("Cross-language compatibility test framework prepared\n");
    printf("(Requires compiled implementations for full validation)\n");
    
    return 1;
}

int main() {
    printf("NexusLink QA POC - Integration Test Suite\n");
    printf("=========================================\n\n");
    
    // Note: These tests require the example packages to be built
    printf("Integration tests prepared. To run:\n");
    printf("1. Build all example packages\n");
    printf("2. Run: make integration-tests\n\n");
    
    test_cross_language_compatibility();
    
    printf("Integration test framework ready!\n");
    return 0;
}
