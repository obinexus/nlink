/**
 * @file types.h
 * @brief Common type definitions for NexusLink
 */

 #ifndef NLINK_COMMON_TYPES_H
 #define NLINK_COMMON_TYPES_H
 
 #include <stddef.h>
 #include <stdint.h>
 #include <stdbool.h>
 #include <stdarg.h>
 #include <time.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 
 #ifdef __cplusplus
 extern "C"
 {
 #endif
 
     /* Forward declarations for structures to avoid circular dependencies */
     typedef struct NexusContext NexusContext;
     typedef struct NexusComponent NexusComponent;
     
     /**
      * @brief Symbol structure for NexusLink
      */
     typedef struct NexusSymbol
     {
         char* name;             /**< Symbol name */
         void* address;          /**< Symbol address */
         NexusSymbolType type;   /**< Symbol type */
         char* component_id;     /**< Component that provides this symbol */
         int ref_count;          /**< Reference count */
     } NexusSymbol;
     
     /**
      * @brief Symbol table structure for NexusLink
      */
     typedef struct NexusSymbolTable
     {
         NexusSymbol* symbols;   /**< Array of symbols */
         size_t size;            /**< Current number of symbols */
         size_t capacity;        /**< Total capacity of symbol array */
     } NexusSymbolTable;
     
     /**
      * @brief Symbol registry structure for NexusLink
      */
     typedef struct NexusSymbolRegistry
     {
         NexusSymbolTable global;    /**< Global symbols */
         NexusSymbolTable imported;  /**< Imported symbols */
         NexusSymbolTable exported;  /**< Exported symbols */
     } NexusSymbolRegistry;
 
     /**
      * @brief Result codes for NexusLink operations
      */
     typedef enum NexusResult
     {
         /* Success codes (0-99) */
         NEXUS_SUCCESS = 0,         /**< Operation completed successfully */
         NEXUS_PARTIAL_SUCCESS = 1, /**< Operation partially succeeded */
 
         /* Error codes (100-999) */
         NEXUS_INVALID_PARAMETER = 100, /**< Invalid parameter passed to function */
         NEXUS_NOT_INITIALIZED = 101,   /**< System not initialized */
         NEXUS_OUT_OF_MEMORY = 102,     /**< Memory allocation failed */
         NEXUS_NOT_FOUND = 103,         /**< Requested item not found */
         NEXUS_ALREADY_EXISTS = 104,    /**< Item already exists */
         NEXUS_INVALID_OPERATION = 105, /**< Operation not valid in current state */
         NEXUS_UNSUPPORTED = 106,       /**< Operation not supported */
         NEXUS_IO_ERROR = 107,          /**< I/O error occurred */
         NEXUS_DEPENDENCY_ERROR = 108,  /**< Dependency resolution failed */
         NEXUS_VERSION_CONFLICT = 109,  /**< Version conflict detected */
         NEXUS_SYMBOL_ERROR = 110,      /**< Symbol resolution error */
 
         /* Component-specific errors (1000+) */
         NEXUS_ERROR_INVALID_ARGUMENT = 1000,  /**< Invalid argument for minimizer */
         NEXUS_ERROR_OUT_OF_MEMORY = 1001,     /**< Memory allocation failed in minimizer */
         NEXUS_ERROR_INVALID_STATE = 1002,     /**< Invalid state for operation */
         NEXUS_ERROR_INVALID_REFERENCE = 1003  /**< Invalid reference provided */
     } NexusResult;
 
     /**
      * @brief Log levels for NexusLink logging
      */
     typedef enum NexusLogLevel
     {
         NEXUS_LOG_DEBUG,   /**< Debug-level message */
         NEXUS_LOG_INFO,    /**< Informational message */
         NEXUS_LOG_WARNING, /**< Warning message */
         NEXUS_LOG_ERROR    /**< Error message */
     } NexusLogLevel;
 
     /**
      * @brief Configuration flags for NexusLink
      */
     typedef enum NexusFlags
     {
         NEXUS_FLAG_NONE = 0,               /**< No flags */
         NEXUS_FLAG_AUTO_LOAD = (1 << 0),   /**< Automatically load components */
         NEXUS_FLAG_AUTO_UNLOAD = (1 << 1), /**< Automatically unload unused components */
         NEXUS_FLAG_STRICT_DEPS = (1 << 2)  /**< Enforce strict dependency checking */
     } NexusFlags;
 
     /**
      * @brief Symbol types for NexusLink symbols
      */
     typedef enum NexusSymbolType
     {
         NEXUS_SYMBOL_FUNCTION,  /**< Function symbol */
         NEXUS_SYMBOL_VARIABLE,  /**< Variable symbol */
         NEXUS_SYMBOL_TYPE,      /**< Type symbol */
         NEXUS_SYMBOL_CONSTANT,  /**< Constant symbol */
         NEXUS_SYMBOL_MACRO,     /**< Macro symbol */
         NEXUS_SYMBOL_STRUCT,    /**< Structure symbol */
         NEXUS_SYMBOL_ENUM,      /**< Enumeration symbol */
         NEXUS_SYMBOL_UNION,     /**< Union symbol */
         NEXUS_SYMBOL_UNKNOWN,   /**< Unknown symbol type */
         /* Legacy names for backward compatibility */
         NEXUS_SYMBOL_DATA = NEXUS_SYMBOL_VARIABLE, /**< Data symbol (alias for VARIABLE) */
         NEXUS_SYMBOL_CONST = NEXUS_SYMBOL_CONSTANT /**< Const symbol (alias for CONSTANT) */
     } NexusSymbolType;
 
     /* Callback type definitions */
     typedef void (*NexusLogCallback)(NexusLogLevel level, const char *format, va_list args);
     typedef bool (*NexusComponentInit)(NexusContext *ctx);
     typedef void (*NexusComponentCleanup)(NexusContext *ctx);
 
     /* Constants */
     #define NEXUS_DEFAULT_TABLE_SIZE 64
     #define NEXUS_DEFAULT_REGISTRY_SIZE 16
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* NLINK_COMMON_TYPES_H */