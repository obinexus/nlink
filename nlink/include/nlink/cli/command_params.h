/**
 * @file command_params.h
 * @brief Command parameter extraction and handling for NexusLink CLI
 * 
 * Provides functionality for extracting, storing, and accessing parameters
 * from command line inputs using pattern matching.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #ifndef NLINK_COMMAND_PARAMS_H
 #define NLINK_COMMAND_PARAMS_H
 
 #include "nlink/core/common/types.h"
 #include "nlink/core/common/result.h"
 #include <stdbool.h>
 #include <stddef.h>
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 /**
  * @brief Command parameter structure
  */
 struct NlinkCommandParam {
     char* name;              // Parameter name
     char* value;             // Parameter value
        size_t value_length;     // Length of the value
        
     struct NlinkCommandParam* next;  // Next parameter in the list
 };
 
/**
 * @brief Command parameter list
 */
struct NlinkCommandParams {
     size_t count;               // Number of parameters
     struct NlinkCommandParam* first;   // First parameter in the list
};
 
 /**
  * @brief Typedef for convenience
  */
 typedef struct NlinkCommandParams NlinkCommandParams;
 typedef struct NlinkCommandParam NlinkCommandParam;
 
 /**
  * @brief Create a new command parameter list
  * 
  * @return NlinkCommandParams* New parameter list or NULL on failure
  */
 NlinkCommandParams* nlink_command_params_create(void);
 
 /**
  * @brief Add a parameter to the list
  * 
  * @param params Parameter list
  * @param name Parameter name
  * @param value Parameter value (can be NULL)
  * @return NexusResult Result code
  */
 NexusResult nlink_command_params_add(NlinkCommandParams* params, 
                                    const char* name, 
                                    const char* value);
 
/**
 * @brief Get a parameter value by name
 * 
 * @param params Parameter list
 * @param name Parameter name
 * @return const char* Parameter value or NULL if not found
 */
const char* nlink_command_params_get(const NlinkCommandParams* params, const char* name);

/**
 * @brief Get a parameter value by name as string
 * 
 * @param params Parameter list
 * @param name Parameter name
 * @return const char* Parameter value or NULL if not found
 */
const char* nlink_command_params_get_string(const NlinkCommandParams* params, const char* name);
 
 /**
  * @brief Check if a parameter exists
  * 
  * @param params Parameter list
  * @param name Parameter name
  * @return bool True if parameter exists, false otherwise
  */
 bool nlink_command_params_has(const NlinkCommandParams* params, const char* name);
 
 /**
  * @brief Get the number of parameters
  * 
  * @param params Parameter list
  * @return size_t Number of parameters
  */
 size_t nlink_command_params_count(const NlinkCommandParams* params);
 
 /**
  * @brief Get parameter at index
  * 
  * @param params Parameter list
  * @param index Parameter index
  * @param name Pointer to store parameter name
  * @param value Pointer to store parameter value (can be NULL)
  * @return bool True if parameter found, false otherwise
  */
 bool nlink_command_params_get_at(const NlinkCommandParams* params, 
                                size_t index, 
                                const char** name, 
                                const char** value);
 
 /**
  * @brief Free command parameter list resources
  * 
  * @param params Parameter list to free
  */
 void nlink_command_params_destroy(NlinkCommandParams* params);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* NLINK_COMMAND_PARAMS_H */