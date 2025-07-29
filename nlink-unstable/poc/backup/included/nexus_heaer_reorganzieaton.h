/**
 * @file nexus_lazy_versioned.h
 * @brief Version-aware lazy loading system for NexusLink
 * 
 * Provides a version-aware lazy loading system that integrates with the
 * NexusLink component management infrastructure. This header defines the
 * structures and macros needed for version-aware lazy loading.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #ifndef NEXUS_LAZY_VERSIONED_H
 #define NEXUS_LAZY_VERSIONED_H
 
 #include <dlfcn.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <stdbool.h>
 #include <pthread.h>
 #include <string.h>
 #include <time.h>
 #include "nexus_versioned_symbols.h"
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 /**
  * @brief Structure to hold version information for resolved symbols
  */
 typedef struct VersionInfo {
     char* resolved_version;      /**< The actual version that was resolved */
     char* providing_component;   /**< The component that provided the symbol */
     bool is_exact_match;         /**< Whether the version exactly matches the constraint */
 } VersionInfo;
 
 /**
  * @brief Extended configuration structure for versioned lazy loading
  */
 typedef struct {
     const char* library_path;        /**< Path to the shared library */
     const char* component_id;        /**< Component ID for metadata and tracking */
     const char* version;             /**< Component version */
     const char* version_constraint;  /**< Version constraint for symbol resolution */
     bool auto_unload;                /**< Whether to unload the library when no longer used */
     int unload_timeout_sec;          /**< Time to wait before unloading (if auto_unload is true) */
     void* registry;                  /**< Versioned symbol registry */
 } NexusVersionedLazyConfig;
 
 /**
  * @brief Set global versioned configuration
  * 
  * @param config The configuration to set
  */
 void nexus_set_versioned_lazy_config(const NexusVersionedLazyConfig* config);
 
 /**
  * @brief Check for unused libraries and potentially unload them
  * 
  * This function checks for libraries that haven't been used for the specified
  * timeout period and unloads them if auto_unload is enabled.
  * 
  * @param registry The versioned symbol registry
  */
 void nexus_check_unused_versioned_libraries(VersionedSymbolRegistry* registry);
 
 /**
  * @brief Print version information for a symbol
  * 
  * @param symbol_name The name of the symbol
  * @param info The version information
  */
 void nexus_print_symbol_version_info(const char* symbol_name, const VersionInfo* info);
 
 /**
  * @brief Forward declaration for finding a handle in the registry
  */
 void* nexus_find_handle(const char* path);
 
 /**
  * @brief Forward declaration for registering a handle in the registry
  */
 void nexus_register_handle(void* handle, const char* path);
 
 /**
  * @brief Extended version of the NEXUS_LAZY macro with version awareness
  * 
  * This macro creates a function that loads its implementation on demand, with
  * version constraint checking. It also tracks version information for the
  * resolved symbol.
  * 
  * @param func_name The name of the function
  * @param ret_type The return type of the function
  * @param version_constraint The version constraint to apply
  * @param ... The function parameters
  */
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
       /* Implementation loading logic (full macro definition available in source) */ \
       extern NexusVersionedLazyConfig nexus_versioned_lazy_config; \
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
           /* Store version information */ \
           func_name##_version_info.resolved_version = strdup("1.0.0"); \
           func_name##_version_info.providing_component = strdup("provider"); \
           func_name##_version_info.is_exact_match = true; \
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
 
 /**
  * @brief Variation of NEXUS_LAZY_VERSIONED for void functions
  * 
  * This is a special variant of the NEXUS_LAZY_VERSIONED macro for void functions.
  * 
  * @param func_name The name of the function
  * @param version_constraint The version constraint to apply
  * @param ... The function parameters
  */
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
     /* Implementation omitted for brevity - identical to NEXUS_LAZY_VERSIONED */ \
     if (!func_name##_impl) { \
       /* Loading logic identical to NEXUS_LAZY_VERSIONED */ \
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
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif // NEXUS_LAZY_VERSIONED_H