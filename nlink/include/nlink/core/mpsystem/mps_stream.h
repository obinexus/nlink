/**
 * @file mps_stream.h
 * @brief Buffered streaming for multi-pass systems
 *
 * Defines structures and functions for passing data between components
 * in a multi-pass pipeline system, with support for routing and buffering.
 *
 * Copyright © 2025 OBINexus Computing
 */

#ifndef NLINK_MPS_STREAM_H
#define NLINK_MPS_STREAM_H

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
typedef void (*MPSStreamMetadataFreeFunc)(void* metadata);

/**
 * @brief Stream metadata entry
 */
typedef struct MPSStreamMetadataEntry {
    char* key;                           /**< Metadata key */
    void* value;                         /**< Metadata value */
    MPSStreamMetadataFreeFunc free_func; /**< Function to free value */
    struct MPSStreamMetadataEntry* next; /**< Next entry in the list */
} MPSStreamMetadataEntry;

/**
 * @brief Stream buffer state
 */
typedef enum {
    MPS_STREAM_EMPTY,     /**< No data available */
    MPS_STREAM_PARTIAL,   /**< Some data available, but not complete */
    MPS_STREAM_READY,     /**< Data is ready for consumption */
    MPS_STREAM_CONSUMED,  /**< Data has been consumed */
    MPS_STREAM_CLOSED     /**< Stream is closed */
} MPSStreamState;

/**
 * @brief Data stream for multi-pass systems
 */
typedef struct NexusMPSDataStream {
    void* data;                     /**< Data buffer */
    size_t size;                    /**< Current data size */
    size_t capacity;                /**< Total buffer capacity */
    size_t position;                /**< Current read/write position */
    const char* format;             /**< Data format identifier */
    MPSStreamMetadataEntry* metadata; /**< Linked list of metadata entries */
    bool owns_data;                 /**< Whether the stream owns the data buffer */
    MPSStreamState state;           /**< Current stream state */
    int generation;                 /**< Generation counter for tracking updates */
    void* buffer_manager;           /**< Optional buffer manager */
} NexusMPSDataStream;

/**
 * @brief Stream connection key
 */
typedef struct {
    char* source_id;                /**< Source component ID */
    char* target_id;                /**< Target component ID */
} MPSStreamKey;

/**
 * @brief Stream map entry
 */
typedef struct {
    MPSStreamKey key;               /**< Stream connection key */
    NexusMPSDataStream* stream;     /**< Data stream */
} MPSStreamMapEntry;

/**
 * @brief Data stream map for multi-pass systems
 */
typedef struct NexusMPSDataStreamMap {
    MPSStreamMapEntry* entries;     /**< Array of stream map entries */
    size_t count;                   /**< Current number of entries */
    size_t capacity;                /**< Total capacity */
} NexusMPSDataStreamMap;

/**
 * @brief Create a new multi-pass data stream
 *
 * @param initial_capacity Initial buffer capacity
 * @return NexusMPSDataStream* New stream or NULL on failure
 */
NexusMPSDataStream* mps_stream_create(size_t initial_capacity);

/**
 * @brief Create a multi-pass data stream from existing data
 *
 * @param data Data buffer
 * @param size Data size
 * @param format Data format identifier
 * @return NexusMPSDataStream* New stream or NULL on failure
 */
NexusMPSDataStream* mps_stream_create_from_data(const void* data, size_t size, const char* format);

/**
 * @brief Resize a multi-pass data stream
 *
 * @param stream Stream to resize
 * @param new_capacity New buffer capacity
 * @return NexusResult Operation result
 */
NexusResult mps_stream_resize(NexusMPSDataStream* stream, size_t new_capacity);

/**
 * @brief Write data to a multi-pass stream
 *
 * @param stream Stream to write to
 * @param data Data to write
 * @param size Data size
 * @return NexusResult Operation result
 */
NexusResult mps_stream_write(NexusMPSDataStream* stream, const void* data, size_t size);

/**
 * @brief Read data from a multi-pass stream
 *
 * @param stream Stream to read from
 * @param buffer Buffer to read into
 * @param size Number of bytes to read
 * @param bytes_read Output parameter for bytes actually read
 * @return NexusResult Operation result
 */
NexusResult mps_stream_read(NexusMPSDataStream* stream, void* buffer, size_t size, size_t* bytes_read);

/**
 * @brief Create a stream map for multi-pass systems
 *
 * @param initial_capacity Initial map capacity
 * @return NexusMPSDataStreamMap* New stream map or NULL on failure
 */
NexusMPSDataStreamMap* mps_stream_map_create(size_t initial_capacity);

/**
 * @brief Add a stream to the map
 *
 * @param map Stream map to add to
 * @param source_id Source component ID
 * @param target_id Target component ID
 * @param stream Stream to add
 * @return NexusResult Operation result
 */
NexusResult mps_stream_map_add(NexusMPSDataStreamMap* map, 
                              const char* source_id, 
                              const char* target_id, 
                              NexusMPSDataStream* stream);

/**
 * @brief Get a stream from the map
 *
 * @param map Stream map to get from
 * @param source_id Source component ID
 * @param target_id Target component ID
 * @return NexusMPSDataStream* Found stream or NULL if not found
 */
NexusMPSDataStream* mps_stream_map_get(const NexusMPSDataStreamMap* map,
                                      const char* source_id,
                                      const char* target_id);

/**
 * @brief Get all streams for a component (as source)
 *
 * @param map Stream map to get from
 * @param source_id Source component ID
 * @param streams Output parameter for streams
 * @param target_ids Output parameter for target component IDs
 * @param count Output parameter for number of streams
 * @return NexusResult Operation result
 */
NexusResult mps_stream_map_get_outgoing(const NexusMPSDataStreamMap* map,
                                       const char* source_id,
                                       NexusMPSDataStream*** streams,
                                       char*** target_ids,
                                       size_t* count);

/**
 * @brief Get all streams for a component (as target)
 *
 * @param map Stream map to get from
 * @param target_id Target component ID
 * @param streams Output parameter for streams
 * @param source_ids Output parameter for source component IDs
 * @param count Output parameter for number of streams
 * @return NexusResult Operation result
 */
NexusResult mps_stream_map_get_incoming(const NexusMPSDataStreamMap* map,
                                       const char* target_id,
                                       NexusMPSDataStream*** streams,
                                       char*** source_ids,
                                       size_t* count);

/**
 * @brief Clear all streams in the map
 *
 * @param map Stream map to clear
 */
void mps_stream_map_clear(NexusMPSDataStreamMap* map);

/**
 * @brief Free stream map resources
 *
 * @param map Stream map to destroy
 */
void mps_stream_map_destroy(NexusMPSDataStreamMap* map);

/**
 * @brief Clone a multi-pass stream
 *
 * @param stream Stream to clone
 * @return NexusMPSDataStream* Cloned stream or NULL on failure
 */
NexusMPSDataStream* mps_stream_clone(const NexusMPSDataStream* stream);

/**
 * @brief Get stream metadata
 *
 * @param stream Stream to get metadata from
 * @param key Metadata key
 * @return void* Metadata value or NULL if not found
 */
void* mps_stream_get_metadata(const NexusMPSDataStream* stream, const char* key);

/**
 * @brief Set stream metadata
 *
 * @param stream Stream to set metadata on
 * @param key Metadata key
 * @param value Metadata value
 * @param free_func Function to free the value
 * @return NexusResult Operation result
 */
NexusResult mps_stream_set_metadata(NexusMPSDataStream* stream, 
                                   const char* key, 
                                   void* value, 
                                   MPSStreamMetadataFreeFunc free_func);

/**
 * @brief Clear a stream (reset position but keep capacity)
 *
 * @param stream Stream to clear
 */
void mps_stream_clear(NexusMPSDataStream* stream);

/**
 * @brief Reset a stream to initial state
 *
 * @param stream Stream to reset
 */
void mps_stream_reset(NexusMPSDataStream* stream);

/**
 * @brief Free stream resources
 *
 * @param stream Stream to destroy
 */
void mps_stream_destroy(NexusMPSDataStream* stream);

#ifdef __cplusplus
}
#endif

#endif /* NLINK_MPS_STREAM_H */
