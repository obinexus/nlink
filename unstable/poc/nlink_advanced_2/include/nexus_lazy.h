// nexus_lazy.h – Enhanced header for lazy loading system
#ifndef NEXUS_LAZY_H
#define NEXUS_LAZY_H

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include "./nexus_symbols.h"


// Forward declaration for symbol registry
struct NexusSymbolRegistry;

// Global handle registry to track loaded libraries
typedef struct {
    void** handles;
    char** paths;
    size_t count;
    size_t capacity;
    pthread_mutex_t mutex;  // For thread safety
} NexusHandleRegistry;

// Initialize the global handle registry
static NexusHandleRegistry* nexus_handle_registry = NULL;

// Initialize the handle registry
void nexus_init_handle_registry() {
    if (nexus_handle_registry) return;  // Already initialized
    
    nexus_handle_registry = (NexusHandleRegistry*)malloc(sizeof(NexusHandleRegistry));
    nexus_handle_registry->capacity = 16;  // Initial capacity
    nexus_handle_registry->count = 0;
    nexus_handle_registry->handles = (void**)malloc(nexus_handle_registry->capacity * sizeof(void*));
    nexus_handle_registry->paths = (char**)malloc(nexus_handle_registry->capacity * sizeof(char*));
    pthread_mutex_init(&nexus_handle_registry->mutex, NULL);
}

// Add a handle to the registry
void nexus_register_handle(void* handle, const char* path) {
    if (!nexus_handle_registry) {
        nexus_init_handle_registry();
    }
    
    pthread_mutex_lock(&nexus_handle_registry->mutex);
    
    // Check if we need to resize
    if (nexus_handle_registry->count >= nexus_handle_registry->capacity) {
        nexus_handle_registry->capacity *= 2;
        nexus_handle_registry->handles = (void**)realloc(
            nexus_handle_registry->handles, 
            nexus_handle_registry->capacity * sizeof(void*)
        );
        nexus_handle_registry->paths = (char**)realloc(
            nexus_handle_registry->paths, 
            nexus_handle_registry->capacity * sizeof(char*)
        );
    }
    
    // Add the handle
    nexus_handle_registry->handles[nexus_handle_registry->count] = handle;
    nexus_handle_registry->paths[nexus_handle_registry->count] = strdup(path);
    nexus_handle_registry->count++;
    
    pthread_mutex_unlock(&nexus_handle_registry->mutex);
}

// Find a handle in the registry
void* nexus_find_handle(const char* path) {
    if (!nexus_handle_registry) return NULL;
    
    pthread_mutex_lock(&nexus_handle_registry->mutex);
    
    for (size_t i = 0; i < nexus_handle_registry->count; i++) {
        if (strcmp(nexus_handle_registry->paths[i], path) == 0) {
            void* handle = nexus_handle_registry->handles[i];
            pthread_mutex_unlock(&nexus_handle_registry->mutex);
            return handle;
        }
    }
    
    pthread_mutex_unlock(&nexus_handle_registry->mutex);
    return NULL;
}

// Cleanup the handle registry
void nexus_cleanup_handle_registry() {
    if (!nexus_handle_registry) return;
    
    pthread_mutex_lock(&nexus_handle_registry->mutex);
    
    // Close all handles
    for (size_t i = 0; i < nexus_handle_registry->count; i++) {
        dlclose(nexus_handle_registry->handles[i]);
        free(nexus_handle_registry->paths[i]);
    }
    
    // Free arrays
    free(nexus_handle_registry->handles);
    free(nexus_handle_registry->paths);
    
    pthread_mutex_unlock(&nexus_handle_registry->mutex);
    
    // Destroy mutex
    pthread_mutex_destroy(&nexus_handle_registry->mutex);
    
    // Free registry
    free(nexus_handle_registry);
    nexus_handle_registry = NULL;
}

// Configuration structure for lazy loading
typedef struct {
    const char* library_path;     // Path to the shared library
    const char* component_id;     // Component ID for metadata and tracking
    bool auto_unload;             // Whether to unload the library when no longer used
    int unload_timeout_sec;       // Time to wait before unloading (if auto_unload is true)
    void* registry;               // Optional symbol registry for tracking
} NexusLazyConfig;

// Default configuration
static NexusLazyConfig nexus_default_config = {
    .library_path = "./libcold.so",
    .component_id = "default_component",
    .auto_unload = false,
    .unload_timeout_sec = 300,  // 5 minutes
    .registry = NULL
};

// Set global configuration
void nexus_set_lazy_config(const NexusLazyConfig* config) {
    if (config) {
        nexus_default_config.library_path = config->library_path ? 
            strdup(config->library_path) : nexus_default_config.library_path;
            
        nexus_default_config.component_id = config->component_id ? 
            strdup(config->component_id) : nexus_default_config.component_id;
            
        nexus_default_config.auto_unload = config->auto_unload;
        nexus_default_config.unload_timeout_sec = config->unload_timeout_sec;
        nexus_default_config.registry = config->registry;
    }
}

// Enhanced macro for lazy-loading functions
#define NEXUS_LAZY(func_name, ret_type, ...) \
  /* Function type */ \
  typedef ret_type (*func_name##_t)(__VA_ARGS__); \
  /* Implementation pointer */ \
  static func_name##_t func_name##_impl = NULL; \
  /* Library handle */ \
  static void* func_name##_handle = NULL; \
  /* Last usage timestamp */ \
  static time_t func_name##_last_used = 0; \
  /* Function prototype */ \
  static ret_type func_name(__VA_ARGS__); \
  /* Load function implementation */ \
  static void load_##func_name() { \
    if (!func_name##_impl) { \
      const char* lib_path = nexus_default_config.library_path; \
      /* Check if library is already loaded */ \
      func_name##_handle = nexus_find_handle(lib_path); \
      if (!func_name##_handle) { \
        /* Load the library */ \
        func_name##_handle = dlopen(lib_path, RTLD_LAZY); \
        if (!func_name##_handle) { \
          fprintf(stderr, "NexusLink error: %s\n", dlerror()); \
          exit(1); \
        } \
        /* Register the handle */ \
        nexus_register_handle(func_name##_handle, lib_path); \
      } \
      /* Get the symbol */ \
      func_name##_impl = (func_name##_t)dlsym(func_name##_handle, #func_name); \
      if (!func_name##_impl) { \
        fprintf(stderr, "NexusLink symbol error: %s\n", dlerror()); \
        exit(1); \
      } \
      if (nexus_default_config.registry) { \
        /* This is a simplified version. In a real implementation, you would */ \
        /* update the symbol registry with more details. */ \
        /* Here we just call a function that prints a message. */ \
        nexus_track_symbol_usage((NexusSymbolRegistry*)nexus_default_config.registry, #func_name, nexus_default_config.component_id); \
      } \
    } \
    /* Update last used timestamp */ \
    func_name##_last_used = time(NULL); \
  } \
  /* Actual function implementation */ \
  static ret_type func_name(__VA_ARGS__) { \
    load_##func_name(); \
    return func_name##_impl(__VA_ARGS__); \
  }


// Check for unused libraries and unload them if needed
void nexus_check_unused_libraries() {
    if (!nexus_handle_registry || !nexus_default_config.auto_unload) return;
    
    time_t now = time(NULL);
    
    pthread_mutex_lock(&nexus_handle_registry->mutex);
    
    // This is a simplified version. In a real implementation, you would
    // need to check if any functions from each library are still being used.
    // Here we just print a message.
    printf("Checking for unused libraries (not implemented in this demo)\n");
    
    pthread_mutex_unlock(&nexus_handle_registry->mutex);
}

// Declare a lazy-loaded function
#define NEXUS_LAZY_DECLARE(name, rettype, argtype) \
    rettype name##_impl(argtype); \
    void load_##name(void); \
    static int name##_loaded = 0;

// Define implementation for a lazy-loaded function
#define NEXUS_LAZY_DEFINE(name, rettype, argtype) \
    rettype name##_impl(argtype x) { \
        printf("Executing %s with argument %d\n", #name, x); \
        return; \
    } \
    \
    void load_##name(void) { \
        if (!name##_loaded) { \
            printf("Loading %s implementation...\n", #name); \
            name##_loaded = 1; \
        } \
    }

// Implement a lazy-loaded function
#define NEXUS_LAZY_IMPLEMENT(name, rettype, argtype) \
    rettype name##_impl(argtype x) { \
        printf("Executing %s with argument %d\n", #name, x); \
        return; \
    }

#endif // NEXUS_LAZY_H