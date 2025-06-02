/**
 * @file test_stub_manager.c
 * @brief Implementation of the test stub manager
 */

#include "../include/nlink_test.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
    char **components;
    int count;
    void **stubs;
} StubManager;

static StubManager *manager = NULL;

void* test_stub_manager_init(const char* components[], int count) {
    if (manager != NULL) {
        // Already initialized
        return manager;
    }
    
    manager = (StubManager*)malloc(sizeof(StubManager));
    if (!manager) {
        return NULL;
    }
    
    manager->count = count;
    manager->components = (char**)malloc(count * sizeof(char*));
    manager->stubs = (void**)malloc(count * sizeof(void*));
    
    if (!manager->components || !manager->stubs) {
        free(manager->components);
        free(manager->stubs);
        free(manager);
        manager = NULL;
        return NULL;
    }
    
    for (int i = 0; i < count; i++) {
        manager->components[i] = strdup(components[i]);
        manager->stubs[i] = NULL;
    }
    
    return manager;
}

void test_stub_manager_cleanup(void) {
    if (manager == NULL) {
        return;
    }
    
    for (int i = 0; i < manager->count; i++) {
        free(manager->components[i]);
        // Free any allocated stubs if needed
        // free(manager->stubs[i]);
    }
    
    free(manager->components);
    free(manager->stubs);
    free(manager);
    manager = NULL;
}
