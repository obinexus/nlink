/**
 * @file nexus_core.h
 * @brief Core functionality for NexusLink
 * 
 * This header defines the core structures and functions for NexusLink.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #ifndef NLINK_CORE_H
 #define NLINK_CORE_H
 
 #include "nlink/core/common/types.h"
 #include "nlink/core/common/result.h"
 #include "nlink/core/common/types.h"
 #include "nlink/core/common/result.h"
 

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdarg.h>
    #include <errno.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 
 #ifdef __cplusplus
 extern "C" {
 #endif



/* Config structure for context creation */
typedef struct NexusConfig NexusConfig;
struct NexusConfig {
    NexusFlags flags;
    NexusLogLevel log_level;
    NexusLogCallback log_callback;
    const char* component_path;
};


 /**
  * @brief Context for NexusLink operations
  */
    typedef struct NexusContext NexusContext;
 struct NexusContext {
     NexusFlags flags;                   /**< Configuration flags */
     NexusLogLevel log_level;            /**< Log level */
     NexusLogCallback log_callback;      /**< Log callback function */
     char* component_path;               /**< Path to components */
     NexusSymbolRegistry* symbols;       /**< Symbol registry */
 };
 
 /**
  * @brief Create a new NexusLink context
  * 
  * @param config Configuration for the context (can be NULL for defaults)
  * @return NexusContext* The new context, or NULL on failure
  */
 NexusContext* nexus_create_context(const NexusConfig* config);
 
 /**
  * @brief Destroy a NexusLink context
  * 
  * @param ctx The context to destroy
  */
 void nexus_destroy_context(NexusContext* ctx);
 
 /**
  * @brief Get the global NexusLink context
  * 
  * @return NexusContext* The global context, or NULL if none exists
  */
 NexusContext* nexus_get_global_context(void);
 
 /**
  * @brief Set the global NexusLink context
  * 
  * @param ctx The context to set as global
  */
 void nexus_set_global_context(NexusContext* ctx);
 
 /**
  * @brief Set the log level for a context
  * 
  * @param ctx The context to modify (NULL for global context)
  * @param level The new log level
  * @return NexusResult Result code
  */
 NexusResult nexus_set_log_level(NexusContext* ctx, NexusLogLevel level);
 
 /**
  * @brief Log a message
  * 
  * @param ctx The context to use (NULL for global context)
  * @param level The log level
  * @param format The format string
  * @param ... Format arguments
  */
 void nexus_log(NexusContext* ctx, NexusLogLevel level, const char* format, ...);
 
 /**
  * @brief Default log callback
  * 
  * @param level The log level
  * @param format The format string
  * @param args Format arguments
  */
 void nexus_default_log_callback(NexusLogLevel level, const char* format, va_list args);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* NLINK_CORE_H */