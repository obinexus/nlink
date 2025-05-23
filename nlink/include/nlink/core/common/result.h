/**
 * @file result.h
 * @brief Result codes and error handling for NexusLink
 */

 #ifndef NLINK_COMMON_RESULT_H
 #define NLINK_COMMON_RESULT_H
 
 #include "nlink/core/common/types.h"
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 /**
  * @brief Convert a result code to a human-readable string
  * 
  * @param result The result code to convert
  * @return const char* A human-readable description of the result code
  */
 const char* nexus_result_to_string(NexusResult result);
 
 /**
  * @brief Check if a result code indicates success
  * 
  * @param result The result code to check
  * @return bool True if the result indicates success, false otherwise
  */
 static inline bool nexus_result_is_success(NexusResult result) {
     return result < 100;  // All success codes are below 100
 }
 
 /**
  * @brief Check if a result code indicates an error
  * 
  * @param result The result code to check
  * @return bool True if the result indicates an error, false otherwise
  */
 static inline bool nexus_result_is_error(NexusResult result) {
     return result >= 100;  // All error codes are 100 or above
 }
 
 /**
  * @brief Log an error message for a result code
  * 
  * @param ctx The NexusLink context
  * @param result The result code
  * @param message Additional message to log
  */
 void nexus_log_result(NexusContext* ctx, NexusResult result, const char* message);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* NLINK_COMMON_RESULT_H */