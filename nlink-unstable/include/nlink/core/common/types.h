/**
 * @file types.h
 * @brief Common type definitions for NexusLink
 * 
 * This header defines common types used throughout the NexusLink system.
 * All other headers should include this for basic type definitions.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #ifndef NLINK_COMMON_TYPES_H
 #define NLINK_COMMON_TYPES_H
 
 #include <stddef.h>
 #include <stdint.h>
 #include <stdbool.h>
 #include <stdarg.h>
 #include <time.h>
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 /**
  * @brief Result codes for NexusLink operations
  */
 typedef enum NexusResult {
     /* Success codes (0-99) */
     NEXUS_SUCCESS = 0,          /**< Operation completed successfully */
     NEXUS_PARTIAL_SUCCESS = 1,  /**< Operation partially succeeded */
     
     /* Error codes (100-999) */
     NEXUS_INVALID_PARAMETER = 100,  /**< Invalid parameter passed to function */
     NEXUS_NOT_INITIALIZED = 101,    /**< System not initialized */
     NEXUS_OUT_OF_MEMORY = 102,      /**< Memory allocation failed */
     NEXUS_NOT_FOUND = 103,          /**< Requested item not found */
     NEXUS_ALREADY_EXISTS = 104,     /**< Item already exists */
     NEXUS_INVALID_OPERATION = 105,  /**< Operation not valid in current state */
     NEXUS_UNSUPPORTED = 106,        /**< Operation not supported */
     NEXUS_IO_ERROR = 107,           /**< I/O error occurred */
     NEXUS_DEPENDENCY_ERROR = 108,   /**< Dependency resolution failed */
     NEXUS_VERSION_CONFLICT = 109,   /**< Version conflict detected */
     NEXUS_SYMBOL_ERROR = 110,       /**< Symbol resolution error */
     
     /* Component-specific errors (1000+) */
     NEXUS_ERROR_INVALID_ARGUMENT = 1000,  /**< Invalid argument for minimizer */
     NEXUS_ERROR_OUT_OF_MEMORY = 1001,     /**< Memory allocation failed in minimizer */
     NEXUS_ERROR_INVALID_STATE = 1002      /**< Invalid state for operation */
 } NexusResult;
 
 /**
  * @brief Log levels for NexusLink logging
  */
 typedef enum NexusLogLevel {
     NEXUS_LOG_DEBUG,   /**< Debug-level message */
     NEXUS_LOG_INFO,    /**< Informational message */
     NEXUS_LOG_WARNING, /**< Warning message */
     NEXUS_LOG_ERROR    /**< Error message */
 } NexusLogLevel;
 
 /**
  * @brief Configuration flags for NexusLink
  */
 typedef enum NexusFlags {
     NEXUS_FLAG_NONE = 0,               /**< No flags */
     NEXUS_FLAG_AUTO_LOAD = (1 << 0),   /**< Automatically load components */
     NEXUS_FLAG_AUTO_UNLOAD = (1 << 1), /**< Automatically unload unused components */
     NEXUS_FLAG_STRICT_DEPS = (1 << 2)  /**< Enforce strict dependency checking */
 } NexusFlags;
 
 /**
  * @brief Symbol types for NexusLink symbols
  */
 typedef enum NexusSymbolType {
     NEXUS_SYMBOL_UNKNOWN,  /**< Unknown symbol type */
     NEXUS_SYMBOL_FUNCTION, /**< Function symbol */
     NEXUS_SYMBOL_DATA,     /**< Data symbol */
     NEXUS_SYMBOL_CONST     /**< Constant symbol */
 } NexusSymbolType;
 
 /* Forward declarations of structures */
 typedef struct NexusSymbol NexusSymbol;
 typedef struct NexusSymbolTable NexusSymbolTable;
 typedef struct NexusSymbolRegistry NexusSymbolRegistry;
 typedef struct NexusContext NexusContext;
 typedef struct NexusConfig NexusConfig;
 typedef struct NexusComponent NexusComponent;
 typedef struct NexusHandleRegistry NexusHandleRegistry;
 typedef struct SemVer SemVer;
 typedef struct VersionInfo VersionInfo;
 typedef struct VersionedSymbol VersionedSymbol;
 typedef struct VersionedSymbolTable VersionedSymbolTable;
 typedef struct VersionedSymbolRegistry VersionedSymbolRegistry;
 typedef struct ComponentMetadata ComponentMetadata;
 typedef struct EnhancedComponentMetadata EnhancedComponentMetadata;
 typedef struct OkpalaState OkpalaState;
 typedef struct OkpalaAutomaton OkpalaAutomaton;
 typedef struct OkpalaNode OkpalaNode;
 typedef struct OkpalaAST OkpalaAST;
 
 /* Callback type definitions */
 typedef void (*NexusLogCallback)(NexusLogLevel level, const char* format, va_list args);
 typedef bool (*NexusComponentInit)(NexusContext* ctx);
 typedef void (*NexusComponentCleanup)(NexusContext* ctx);
 
 /* Constants */
 #define NEXUS_DEFAULT_TABLE_SIZE 64
 #define NEXUS_DEFAULT_REGISTRY_SIZE 16
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* NLINK_COMMON_TYPES_H */