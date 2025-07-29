/**
 * @file type.h
 * @brief Header for the NexusLink type subsystem
 * @author NexusLink Implementation Team
 * @copyright OBINexus Computing, 2025
 */

#ifndef NLINK_TYPE_H
#define NLINK_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Configuration structure for type subsystem
 */
typedef struct {
    int mode;                   /**< Operating mode */
    unsigned int flags;         /**< Configuration flags */
    void* context;              /**< User-defined context */
} type_config_t;

/**
 * @brief Initialize the type subsystem
 * @param config Configuration parameters
 * @return Status code (0 = success)
 */
int type_initialize(type_config_t* config);

/**
 * @brief Shutdown the type subsystem
 * @return Status code (0 = success)
 */
int type_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* NLINK_TYPE_H */
