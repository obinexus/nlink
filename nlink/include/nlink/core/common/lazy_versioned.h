// include/nexus_lazy_versioned.h
// Version-aware lazy loading system for NexusLink
// Author: Implementation Team

#ifndef NEXUS_LAZY_VERSIONED_H
#define NEXUS_LAZY_VERSIONED_H

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include "nlink/core/common/symbols/nexus_symbols.h"
// Version information structure
typedef struct VersionInfo {
  char* resolved_version;
  char* providing_component;
  int is_exact_match;
} VersionInfo;

// Forward declarations
void* nexus_resolve_versioned_symbol(VersionedSymbolRegistry* registry,
                                    const char* name,
                                    const char* version_constraint,
                                    const char* requesting_component);
void* nexus_find_handle(const char* library_path);
void nexus_register_handle(void* handle, const char* library_path);
// External declaration for nexus_find_handle and nexus_register_handle
extern void* nexus_find_handle(const char* path);
extern void nexus_register_handle(void* handle, const char* path);


// Extended configuration structure for versioned lazy loading
typedef struct {
    const char* library_path;      // Path to the shared library
    const char* component_id;      // Component ID for metadata and tracking
    const char* version;           // Component version
    const char* version_constraint; // Version constraint for symbol resolution
    bool auto_unload;              // Whether to unload the library when no longer used
    int unload_timeout_sec;        // Time to wait before unloading (if auto_unload is true)
    void* registry;                // Versioned symbol registry
} NexusVersionedLazyConfig;

// Global configuration
static NexusVersionedLazyConfig nexus_versioned_lazy_config = {
    .library_path = "./libcold.so",
    .component_id = "default_component",
    .version = "1.0.0",
    .version_constraint = NULL,
    .auto_unload = false,
    .unload_timeout_sec = 300,  // 5 minutes
    .registry = NULL
};

// Set global versioned configuration
// Set global versioned configuration
void nexus_set_versioned_lazy_config(const NexusVersionedLazyConfig* config);

// Enhanced macro for version-aware lazy-loading functions
#define NEXUS_LAZY_VERSIONED(func_name, ret_type, version_constraint, ...) \
  /* Function type */ \
  typedef ret_type (*func_name##_t)(__VA_ARGS__); \
  /* Implementation pointer */ \
  static func_name##_t func_name##_impl = NULL; \
  /* Library handle */ \
  static void* func_name##_handle = NULL; \
  /* Last usage timestamp */ \
  static time_t func_name##_last_used = 0; \
  /* Version information */ \
  static VersionInfo func_name##_version_info = {NULL, NULL, false}; \
  /* Function prototype */ \
  static ret_type func_name(__VA_ARGS__); \
  /* Load function implementation with versioning */ \
  static void load_##func_name() { \
    if (!func_name##_impl) { \
      const char* lib_path = nexus_versioned_lazy_config.library_path; \
      const char* version_req = version_constraint ? version_constraint : \
                               nexus_versioned_lazy_config.version_constraint; \
      \
      /* Check if library is already loaded */ \
      func_name##_handle = nexus_find_handle(lib_path); \
      if (!func_name##_handle) { \
        /* Load the library */ \
        func_name##_handle = dlopen(lib_path, RTLD_LAZY); \
        if (!func_name##_handle) { \
          fprintf(stderr, "NexusLink error loading %s: %s\n", lib_path, dlerror()); \
          exit(1); \
        } \
        /* Register the handle */ \
        nexus_register_handle(func_name##_handle, lib_path); \
      } \
      \
      if (nexus_versioned_lazy_config.registry) { \
        /* Version-aware symbol resolution */ \
        void* symbol_address = nexus_resolve_versioned_symbol( \
          (VersionedSymbolRegistry*)nexus_versioned_lazy_config.registry, \
          #func_name, \
          version_req, \
          nexus_versioned_lazy_config.component_id \
        ); \
        \
        if (symbol_address) { \
          func_name##_impl = (func_name##_t)symbol_address; \
          \
          /* Store version information (simplified for this demo) */ \
          func_name##_version_info.resolved_version = strdup("1.0.0"); /* In a real impl, get this from the registry */ \
          func_name##_version_info.providing_component = strdup("provider"); /* In a real impl, get this from the registry */ \
          func_name##_version_info.is_exact_match = true; /* In a real impl, calculate this */ \
        } else { \
          /* Fall back to direct symbol lookup */ \
          func_name##_impl = (func_name##_t)dlsym(func_name##_handle, #func_name); \
          if (!func_name##_impl) { \
            fprintf(stderr, "NexusLink symbol error: %s\n", dlerror()); \
            exit(1); \
          } \
        } \
      } else { \
        /* Direct symbol lookup without versioning */ \
        func_name##_impl = (func_name##_t)dlsym(func_name##_handle, #func_name); \
        if (!func_name##_impl) { \
          fprintf(stderr, "NexusLink symbol error: %s\n", dlerror()); \
          exit(1); \
        } \
      } \
    } \
    /* Update last used timestamp */ \
    func_name##_last_used = time(NULL); \
  } \
  /* Get version information */ \
  static const VersionInfo* get_##func_name##_version_info() { \
    if (!func_name##_impl) { \
      load_##func_name(); \
    } \
    return &func_name##_version_info; \
  } \
  /* Actual function implementation */ \
  static ret_type func_name(__VA_ARGS__) { \
    load_##func_name(); \
    return func_name##_impl(__VA_ARGS__); \
  }

// Variation for void functions to avoid return statement errors
#define NEXUS_LAZY_VERSIONED_VOID(func_name, version_constraint, ...) \
  /* Function type */ \
  typedef void (*func_name##_t)(__VA_ARGS__); \
  /* Implementation pointer */ \
  static func_name##_t func_name##_impl = NULL; \
  /* Library handle */ \
  static void* func_name##_handle = NULL; \
  /* Last usage timestamp */ \
  static time_t func_name##_last_used = 0; \
  /* Version information */ \
  static VersionInfo func_name##_version_info = {NULL, NULL, false}; \
  /* Function prototype */ \
  static void func_name(__VA_ARGS__); \
  /* Load function implementation with versioning */ \
  static void load_##func_name() { \
    if (!func_name##_impl) { \
      const char* lib_path = nexus_versioned_lazy_config.library_path; \
      const char* version_req = version_constraint ? version_constraint : \
                               nexus_versioned_lazy_config.version_constraint; \
      \
      /* Check if library is already loaded */ \
      func_name##_handle = nexus_find_handle(lib_path); \
      if (!func_name##_handle) { \
        /* Load the library */ \
        func_name##_handle = dlopen(lib_path, RTLD_LAZY); \
        if (!func_name##_handle) { \
          fprintf(stderr, "NexusLink error loading %s: %s\n", lib_path, dlerror()); \
          exit(1); \
        } \
        /* Register the handle */ \
        nexus_register_handle(func_name##_handle, lib_path); \
      } \
      \
      if (nexus_versioned_lazy_config.registry) { \
        /* Version-aware symbol resolution */ \
        void* symbol_address = nexus_resolve_versioned_symbol( \
          (VersionedSymbolRegistry*)nexus_versioned_lazy_config.registry, \
          #func_name, \
          version_req, \
          nexus_versioned_lazy_config.component_id \
        ); \
        \
        if (symbol_address) { \
          func_name##_impl = (func_name##_t)symbol_address; \
          \
          /* Store version information (simplified for this demo) */ \
          func_name##_version_info.resolved_version = strdup("1.0.0"); /* In a real impl, get this from the registry */ \
          func_name##_version_info.providing_component = strdup("provider"); /* In a real impl, get this from the registry */ \
          func_name##_version_info.is_exact_match = true; /* In a real impl, calculate this */ \
        } else { \
          /* Fall back to direct symbol lookup */ \
          func_name##_impl = (func_name##_t)dlsym(func_name##_handle, #func_name); \
          if (!func_name##_impl) { \
            fprintf(stderr, "NexusLink symbol error: %s\n", dlerror()); \
            exit(1); \
          } \
        } \
      } else { \
        /* Direct symbol lookup without versioning */ \
        func_name##_impl = (func_name##_t)dlsym(func_name##_handle, #func_name); \
        if (!func_name##_impl) { \
          fprintf(stderr, "NexusLink symbol error: %s\n", dlerror()); \
          exit(1); \
        } \
      } \
    } \
    /* Update last used timestamp */ \
    func_name##_last_used = time(NULL); \
  } \
  /* Get version information */ \
  static const VersionInfo* get_##func_name##_version_info() { \
    if (!func_name##_impl) { \
      load_##func_name(); \
    } \
    return &func_name##_version_info; \
  } \
  /* Actual function implementation */ \
  static void func_name(__VA_ARGS__) { \
    load_##func_name(); \
    func_name##_impl(__VA_ARGS__); \
  }

// Check for unused libraries and potentially unload them
void nexus_check_unused_versioned_libraries(VersionedSymbolRegistry* registry);

// Utility to print version information for a symbol
void nexus_print_symbol_version_info(const char* symbol_name, const VersionInfo* info);

#endif // NEXUS_LAZY_VERSIONED_H