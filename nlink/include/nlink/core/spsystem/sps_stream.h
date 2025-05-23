/**
 * @file sps_stream.h
 * @brief Data streaming for single-pass systems
 *
 * Defines structures and functions for passing data between components
 * in a single-pass pipeline system.
 *
 * Copyright © 2025 OBINexus Computing
 */

#ifndef NLINK_SPS_STREAM_H
#define NLINK_SPS_STREAM_H

#include "nlink/core/common/nexus_core.h"
#include "nlink/core/common/result.h"
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Function to free metadata
 */
typedef void (*StreamMetadataFreeFunc)(void* metadata);

/**
 * @brief Stream metadata entry
 */
typedef struct StreamMetadataEntry {
    char* key;                      /**< Metadata key */
    void* value;                    /**< Metadata value */
    StreamMetadataFreeFunc free_func; /**< Function to free value */
    struct StreamMetadataEntry* next; /**< Next entry in the list */
} StreamMetadataEntry;

/**
 * @brief Data stream for passing data between components
 */
typedef struct NexusDataStream {
    void* data;                     /**< Data buffer */
    size_t size;                    /**< Current data size */
    size_t capacity;                /**< Total buffer capacity */
    size_t position;                /**< Current read/write position */
    const char* format;             /**< Data format identifier */
    StreamMetadataEntry* metadata;  /**< Linked list of metadata entries */
    bool owns_data;                 /**< Whether the stream owns the data buffer */
} NexusDataStream;

/**
 * @brief Create a new data stream
 *
 * @param initial_capacity Initial buffer capacity
 * @return NexusDataStream* New stream or NULL on failure
 */
NexusDataStream* sps_stream_create(size_t initial_capacity);

/**
 * @brief Create a data stream from existing data
 *
 * @param data Data buffer
 * @param size Data size
 * @param format Data format identifier
 * @return NexusDataStream* New stream or NULL on failure
 */
NexusDataStream* sps_stream_create_from_data(const void* data, size_t size, const char* format);

/**
 * @brief Resize a data stream
 *
 * @param stream Stream to resize
 * @param new_capacity New buffer capacity
 * @return NexusResult Operation result
 */
NexusResult sps_stream_resize(NexusDataStream* stream, size_t new_capacity);

/**
 * @brief Write data to a stream
 *
 * @param stream Stream to write to
 * @param data Data to write
 * @param size Data size
 * @return NexusResult Operation result
 */
NexusResult sps_stream_write(NexusDataStream* stream, const void* data, size_t size);

/**
 * @brief Read data from a stream
 *
 * @param stream Stream to read from
 * @param buffer Buffer to read into
 * @param size Number of bytes to read
 * @param bytes_read Output parameter for bytes actually read
 * @return NexusResult Operation result
 */
NexusResult sps_stream_read(NexusDataStream* stream, void* buffer, size_t size, size_t* bytes_read);

/**
 * @brief Get stream metadata
 *
 * @param stream Stream to get metadata from
 * @param key Metadata key
 * @return void* Metadata value or NULL if not found
 */
void* sps_stream_get_metadata(const NexusDataStream* stream, const char* key);

/**
 * @brief Set stream metadata
 *
 * @param stream Stream to set metadata on
 * @param key Metadata key
 * @param value Metadata value
 * @param free_func Function to free the value
 * @return NexusResult Operation result
 */
NexusResult sps_stream_set_metadata(NexusDataStream* stream, const char* key, void* value, StreamMetadataFreeFunc free_func);

/**
 * @brief Clear a stream (reset position but keep capacity)
 *
 * @param stream Stream to clear
 */
void sps_stream_clear(NexusDataStream* stream);

/**
 * @brief Reset a stream to initial state
 *
 * @param stream Stream to reset
 */
void sps_stream_reset(NexusDataStream* stream);

/**
 * @brief Free stream resources
 *
 * @param stream Stream to destroy
 */
void sps_stream_destroy(NexusDataStream* stream);

/**
 * @brief Clone a stream
 *
 * @param stream Stream to clone
 * @return NexusDataStream* Cloned stream or NULL on failure
 */
NexusDataStream* sps_stream_clone(const NexusDataStream* stream);

#ifdef __cplusplus
}
#endif

#endif /* NLINK_SPS_STREAM_H */
