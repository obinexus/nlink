/**
 * @file result.c
 * @brief Implementation of result-related functions for NexusLink
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nlink/core/common/result.h"
 #include <stdio.h>
 
 const char* nexus_result_to_string(NexusResult result) {
     switch (result) {
         /* Success codes */
         case NEXUS_SUCCESS:            return "Success";
         case NEXUS_PARTIAL_SUCCESS:    return "Partial success";
         
         /* Error codes */
         case NEXUS_INVALID_PARAMETER:  return "Invalid parameter";
         case NEXUS_NOT_INITIALIZED:    return "Not initialized";
         case NEXUS_OUT_OF_MEMORY:      return "Out of memory";
         case NEXUS_NOT_FOUND:          return "Not found";
         case NEXUS_ALREADY_EXISTS:     return "Already exists";
         case NEXUS_INVALID_OPERATION:  return "Invalid operation";
         case NEXUS_UNSUPPORTED:        return "Unsupported operation";
         case NEXUS_IO_ERROR:           return "I/O error";
         case NEXUS_DEPENDENCY_ERROR:   return "Dependency error";
         case NEXUS_VERSION_CONFLICT:   return "Version conflict";
         case NEXUS_SYMBOL_ERROR:       return "Symbol error";
         
         /* Component-specific errors */
         case NEXUS_ERROR_INVALID_ARGUMENT: return "Invalid argument (minimizer)";
         case NEXUS_ERROR_OUT_OF_MEMORY:    return "Out of memory (minimizer)";
         case NEXUS_ERROR_INVALID_STATE:    return "Invalid state (minimizer)";
         
         /* Unknown code */
         default:                       return "Unknown error";
     }
 }
 
 void nexus_log_result(NexusContext* ctx, NexusResult result, const char* message) {
     // Forward declaration to avoid circular dependency with core module
     extern void nexus_log(NexusContext* ctx, NexusLogLevel level, const char* format, ...);
     
     if (!ctx) {
         // Use a simplified fallback implementation when context is NULL
         fprintf(stderr, "[ERROR] %s: %s\n", 
                 nexus_result_to_string(result), message ? message : "");
         return;
     }
     
     // Log the result using the context's logging system
     NexusLogLevel level = nexus_result_is_error(result) ? NEXUS_LOG_ERROR : NEXUS_LOG_INFO;
     nexus_log(ctx, level, "%s: %s", 
              nexus_result_to_string(result), message ? message : "");
 }