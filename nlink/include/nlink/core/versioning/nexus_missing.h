/**
 * @file nexus_missing.h
 * @brief Declarations for functions referenced but not defined
 * 
 * This header provides declarations for functions that are referenced
 * in the codebase but currently lack definitions.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #ifndef NLINK_MISSING_H
 #define NLINK_MISSING_H
 
 #include "nlink/core/common/types.h"
 #include "nlink/core/common/result.h"
 #include "nlink/core/common/nexus_core.h"
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 /**
  * @brief Configuration for versioned lazy loading
  */
 typedef struct {
     bool auto_unload;                  /**< Whether to automatically unload unused libraries */
     time_t unload_timeout_sec;         /**< Timeout in seconds for unloading unused libraries */
     bool track_usage;                  /**< Whether to track usage of symbols */
 } NexusVersionedLazyConfig;
 
 /**
  * @brief Get the versioned lazy loading configuration
  * 
  * @return Pointer to the configuration structure
  */
 NexusVersionedLazyConfig* nexus_versioned_lazy_config(void);
 
 /**
  * @brief Handle registry for dynamically loaded components
  * 
  * @param ctx The NexusLink context
  * @return NexusResult Result code
  */
 NexusResult nexus_handle_registry(NexusContext* ctx);
 
 /**
  * @brief Command implementation for stats command
  * 
  * @param ctx The NexusLink context
  * @return NexusResult Result code
  */
 NexusResult stats_command(NexusContext* ctx);
 
 /**
  * @brief Command implementation for build command
  * 
  * @param ctx The NexusLink context
  * @return NexusResult Result code
  */
 NexusResult build_command(NexusContext* ctx);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* NLINK_MISSING_H */