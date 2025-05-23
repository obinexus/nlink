/**
 * @file nexus_loader.c
 * @brief Dynamic component loader implementation for NexusLink
 * 
 * Provides functionality for dynamically loading components and their symbols
 * on demand, implementing the Load-By-Need principle of NexusLink.
 * 
 * Copyright © 2025 OBINexus Computing
 */

    
 // Global handle registry
 static NexusHandleRegistry* g_handle_registry = NULL;
 
 // Initialize the handle registry
 NexusHandleRegistry* nexus_init_handle_registry(void) {
     if (g_handle_registry) {
         return g_handle_registry;
     }
     
     NexusHandleRegistry* registry = (NexusHandleRegistry*)malloc(sizeof(NexusHandleRegistry));
     if (!registry) {
         return NULL;
     }
     
     // Initialize with default values
     registry->handles = NULL;
     registry->paths = NULL;
     registry->components = NULL;
     registry->count = 0;
     registry->capacity = 0;
     
     g_handle_registry = registry;
     return registry;
 }
 
 // Find a component handle by path
 void* nexus_find_component_handle(NexusHandleRegistry* registry, const char* path) {
     if (!registry || !path) {
         return NULL;
     }
     
     for (size_t i = 0; i < registry->count; i++) {
         if (strcmp(registry->paths[i], path) == 0) {
             return registry->handles[i];
         }
     }
     
     return NULL;
 }
 
 // Register a component handle
 NexusResult nexus_register_component_handle(NexusHandleRegistry* registry, void* handle, 
                                           const char* path, const char* component_id) {
     if (!registry || !handle || !path || !component_id) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     // Check if we need to resize
     if (registry->count >= registry->capacity) {
         size_t new_capacity = registry->capacity * 2;
         if (new_capacity == 0) {
             new_capacity = NEXUS_DEFAULT_REGISTRY_SIZE;
         }
         
         void** new_handles = (void**)realloc(registry->handles, new_capacity * sizeof(void*));
         if (!new_handles) {
             return NEXUS_OUT_OF_MEMORY;
         }
         
         char** new_paths = (char**)realloc(registry->paths, new_capacity * sizeof(char*));
         if (!new_paths) {
             registry->handles = new_handles; // Keep this update even if the next one fails
             return NEXUS_OUT_OF_MEMORY;
         }
         
         char** new_components = (char**)realloc(registry->components, new_capacity * sizeof(char*));
         if (!new_components) {
             registry->handles = new_handles;
             registry->paths = new_paths;
             return NEXUS_OUT_OF_MEMORY;
         }
         
         registry->handles = new_handles;
         registry->paths = new_paths;
         registry->components = new_components;
         registry->capacity = new_capacity;
     }
     
     // Add the new handle
     registry->handles[registry->count] = handle;
     registry->paths[registry->count] = strdup(path);
     registry->components[registry->count] = strdup(component_id);
     
     if (!registry->paths[registry->count] || !registry->components[registry->count]) {
         // Cleanup on error
         free(registry->paths[registry->count]);
         free(registry->components[registry->count]);
         return NEXUS_OUT_OF_MEMORY;
     }
     
     registry->count++;
     return NEXUS_SUCCESS;
 }
 
 // Load a component
 NexusComponent* nexus_load_component(NexusContext* ctx, const char* path, const char* component_id) {
     if (!ctx || !path || !component_id) {
         return NULL;
     }
     
     // Ensure we have a handle registry
     NexusHandleRegistry* registry = nexus_init_handle_registry();
     if (!registry) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to initialize handle registry");
         return NULL;
     }
     
     // Check if the component is already loaded
     void* handle = nexus_find_component_handle(registry, path);
     if (!handle) {
         // Load the component
         handle = dlopen(path, RTLD_LAZY);
         if (!handle) {
             nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to load component: %s", dlerror());
             return NULL;
         }
         
         // Register the handle
         NexusResult result = nexus_register_component_handle(registry, handle, path, component_id);
         if (result != NEXUS_SUCCESS) {
             dlclose(handle);
             nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to register component handle: %s", 
                     nexus_result_to_string(result));
             return NULL;
         }
     }
     
     // Create the component structure
     NexusComponent* component = (NexusComponent*)malloc(sizeof(NexusComponent));
     if (!component) {
         // Don't close the handle here, as it might be used by other components
         nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to allocate component structure");
         return NULL;
     }
     
     // Initialize the component
     component->handle = handle;
     component->path = strdup(path);
     component->id = strdup(component_id);
     component->ref_count = 1;
     
     if (!component->path || !component->id) {
         free(component->path);
         free(component->id);
         free(component);
         nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to allocate component strings");
         return NULL;
     }
     
     // Load the initialization function
     NexusComponentInit init_func = (NexusComponentInit)dlsym(handle, "nexus_component_init");
     if (init_func) {
         // Call the initialization function
         if (!init_func(ctx)) {
             nexus_log(ctx, NEXUS_LOG_ERROR, "Component initialization failed");
             free(component->path);
             free(component->id);
             free(component);
             return NULL;
         }
     }
     
     nexus_log(ctx, NEXUS_LOG_INFO, "Loaded component: %s", component_id);
     return component;
 }
 
 // Unload a component
 NexusResult nexus_unload_component(NexusContext* ctx, NexusComponent* component) {
     if (!ctx || !component) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     // Decrement reference count
     component->ref_count--;
     
     // If still in use, don't unload
     if (component->ref_count > 0) {
         return NEXUS_SUCCESS;
     }
     
     // Load the cleanup function
     NexusComponentCleanup cleanup_func = (NexusComponentCleanup)dlsym(component->handle, 
                                                                     "nexus_component_cleanup");
     if (cleanup_func) {
         // Call the cleanup function
         cleanup_func(ctx);
     }
     
     // Free component resources
     free(component->path);
     free(component->id);
     free(component);
     
     // Note: We don't call dlclose here because other components might still be using the library
     
     return NEXUS_SUCCESS;
 }
 
 // Resolve a symbol from a component
 void* nexus_resolve_component_symbol(NexusContext* ctx, NexusComponent* component, const char* symbol_name) {
     if (!ctx || !component || !symbol_name) {
         return NULL;
     }
     
     // Use dlsym to find the symbol
     void* symbol_address = dlsym(component->handle, symbol_name);
     if (!symbol_address) {
         nexus_log(ctx, NEXUS_LOG_DEBUG, "Symbol not found in component: %s", symbol_name);
         return NULL;
     }
     
     // Add the symbol to the exported table
     NexusResult result = nexus_symbol_table_add(&ctx->symbols->exported, symbol_name, 
                                                symbol_address, NEXUS_SYMBOL_UNKNOWN, component->id);
     if (result != NEXUS_SUCCESS) {
         nexus_log(ctx, NEXUS_LOG_WARNING, "Failed to register exported symbol: %s", symbol_name);
     }
     
     return symbol_address;
 }
 
 // Cleanup the handle registry
 void nexus_cleanup_handle_registry(NexusHandleRegistry* registry) {
     if (!registry) {
         return;
     }
     
     // Close all handles
     for (size_t i = 0; i < registry->count; i++) {
         dlclose(registry->handles[i]);
         free(registry->paths[i]);
         free(registry->components[i]);
     }
     
     free(registry->handles);
     free(registry->paths);
     free(registry->components);
     
     if (registry == g_handle_registry) {
         g_handle_registry = NULL;
     }
     
     free(registry);
 }