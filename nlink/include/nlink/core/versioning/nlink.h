/**
 * @file nlink.h
 * @brief Main header for the NexusLink library
 * 
 * This file provides the public API for the NexusLink library,
 * a dynamic component linkage system for efficient binary sizes and runtime loading.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #ifndef NLINK_H
 #define NLINK_H
 
#include "common/result.h"
#include "common/types.h"
#include "common/nexus_core.h"
#include "common/command.h"
#include "minimizer/nexus_minimizer.h"
#include "versioning/nexus_version.h"
#include "common/command.h"




 #ifdef __cplusplus
 extern "C" {
 #endif
 
 
 /**
  * @brief Log levels for the NexusLink library
  */
 typedef enum {
     NLINK_LOG_DEBUG,    /**< Debug messages */
     NLINK_LOG_INFO,     /**< Informational messages */
     NLINK_LOG_WARNING,  /**< Warning messages */
     NLINK_LOG_ERROR     /**< Error messages */
 } NlinkLogLevel;
 
 /**
  * @brief Flags for the NexusLink library
  */
 typedef enum {
     NLINK_FLAG_NONE = 0,                /**< No flags */
     NLINK_FLAG_AUTO_LOAD = (1 << 0),    /**< Automatically load dependencies */
     NLINK_FLAG_AUTO_MINIMIZE = (1 << 1) /**< Automatically minimize components */
 } NlinkFlags;
 
 /**
  * @brief Minimization levels for the NexusLink library
  */
 typedef enum {
     NLINK_MINIMIZE_BASIC = 1,      /**< Basic minimization */
     NLINK_MINIMIZE_STANDARD = 2,   /**< Standard minimization */
     NLINK_MINIMIZE_AGGRESSIVE = 3  /**< Aggressive minimization */
 } NlinkMinimizeLevel;
 
 /**
  * @brief Log callback function type
  */
 typedef void (*NlinkLogCallback)(NlinkLogLevel level, const char* format, va_list args);
 
 /**
  * @brief Configuration for the NexusLink library
  */
 typedef struct {
     NlinkFlags flags;              /**< Library flags */
     NlinkLogLevel log_level;       /**< Log level */
     NlinkLogCallback log_callback; /**< Log callback function */
     const char* component_path;    /**< Path to component directory */
 } NlinkConfig;
 
 /**
  * @brief Build information for the NexusLink library
  */
 typedef struct {
     const char* version;    /**< Library version */
     const char* build_date; /**< Build date */
     const char* copyright;  /**< Copyright information */
 } NlinkBuildInfo;
 
 /**
  * @brief Initialize the NexusLink library
  * 
  * @param config Configuration for the library, or NULL for defaults
  * @return NexusResult Result code
  */
 NexusResult nlink_initialize(const NlinkConfig* config);
 
 /**
  * @brief Get the global NexusLink context
  * 
  * @return NexusContext* The global context, or NULL if not initialized
  */
 NexusContext* nlink_get_context(void);
 
 /**
  * @brief Clean up the NexusLink library
  */
 void nlink_cleanup(void);
 
 /**
  * @brief Get the NexusLink version
  * 
  * @return const char* Version string
  */
 const char* nlink_get_version(void);
 
 /**
  * @brief Get build information for the NexusLink library
  * 
  * @param info Output build information
  */
 void nlink_get_build_info(NlinkBuildInfo* info);
 
 /**
  * @brief Load a component
  * 
  * @param path Path to the component
  * @param id Component ID
  * @param version Version constraint, or NULL for any version
  * @return NexusComponent* Loaded component, or NULL on error
  */
 NexusComponent* nlink_load_component(const char* path, const char* id, const char* version);
 
 /**
  * @brief Minimize a component
  * 
  * @param path Path to the component
  * @param level Minimization level
  * @return NexusResult Result code
  */
 NexusResult nlink_minimize_component(const char* path, NlinkMinimizeLevel level);
 
 /**
  * @brief Get the load command
  * 
  * @return NexusCommand* The load command
  */
 NexusCommand* nlink_get_load_command(void);
 
 /**
  * @brief Get the version command
  * 
  * @return NexusCommand* The version command
  */
 NexusCommand* nlink_get_version_command(void);
 
 /**
  * @brief Get the minimal command
  * 
  * @return NexusCommand* The minimal command
  */
 NexusCommand* nlink_get_minimal_command(void);
 
 /**
  * @brief Get the minimize command
  * 
  * @return NexusCommand* The minimize command
  */
 NexusCommand* nlink_get_minimize_command(void);

 #ifdef __cplusplus
 }
 #endif
 
 #endif /* NLINK_H */