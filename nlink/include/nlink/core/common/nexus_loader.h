/**
 * @file nexus_loader.h
 * @brief Dynamic component loader header for NexusLink
 * 
 * Defines the structures and functions for dynamically loading components.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #ifndef NLINK_CORE_LOADER_H
 #define NLINK_CORE_LOADER_H
 
 #include "nlink/core/common/types.h"
 #include "nlink/core/common/result.h"
 #include "nlink/core/common/nexus_core.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <dlfcn.h>
 #include <errno.h>
 
 #ifdef __cplusplus
 extern "C" {
 #endif


 
// Define the default registry size
#define NEXUS_DEFAULT_REGISTRY_SIZE 16

// Forward declaration of handle registry
struct NexusHandleRegistry;
/**
* @brief Handle registry structure
*/
typedef struct NexusHandleRegistry {
    void** handles;     /**< Array of library handles */
    char** paths;       /**< Array of library paths */
    char** components;  /**< Array of component identifiers */
    size_t count;       /**< Number of registered handles */
    size_t capacity;    /**< Capacity of the arrays */

    pthread_mutex_t mutex; /**< Mutex for thread safety */
} NexusHandleRegistry;
/**
 * @brief Component structure
 *  
 */
typedef struct NexusComponent NexusComponent;
struct NexusComponent {
     void* handle;      /**< Library handle */
     char* path;        /**< Path to the component library */
     char* id;          /**< Component identifier */
     int ref_count;     /**< Reference count */
};

 /**
  * @brief Initialize the handle registry
  * 
  * @return NexusHandleRegistry* Pointer to the handle registry, or NULL on failure
  */
 NexusHandleRegistry* nexus_init_handle_registry(void);
 
 /**
  * @brief Find a component handle by path
  * 
  * @param registry The handle registry
  * @param path Path to the component library
  * @return Handle, or NULL if not found
  */
 void* nexus_find_component_handle(NexusHandleRegistry* registry, const char* path);
 
 /**
  * @brief Register a component handle
  * 
  * @param registry The handle registry
  * @param handle Component handle
  * @param path Path to the component library
  * @param component_id Component identifier
  * @return Result code
  */
 NexusResult nexus_register_component_handle(NexusHandleRegistry* registry, void* handle, 
                                           const char* path, const char* component_id);
 
 /**
  * @brief Load a component
  * 
  * @param ctx The NexusLink context
  * @param path Path to the component library
  * @param component_id Component identifier
  * @return Pointer to the loaded component, or NULL on failure
  */
 NexusComponent* nexus_load_component(NexusContext* ctx, const char* path, const char* component_id);
 
 /**
  * @brief Unload a component
  * 
  * @param ctx The NexusLink context
  * @param component The component to unload
  * @return Result code
  */
 NexusResult nexus_unload_component(NexusContext* ctx, NexusComponent* component);
 
 /**
  * @brief Resolve a symbol from a component
  * 
  * @param ctx The NexusLink context
  * @param component The component to resolve from
  * @param symbol_name Name of the symbol to resolve
  * @return Symbol address, or NULL if not found
  */
 void* nexus_resolve_component_symbol(NexusContext* ctx, NexusComponent* component, const char* symbol_name);
 
 /**
  * @brief Cleanup the handle registry
  * 
  * @param registry The handle registry to cleanup
  */
 void nexus_cleanup_handle_registry(NexusHandleRegistry* registry);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif // NLINK_CORE_LOADER_H