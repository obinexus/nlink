// include/nlink/core/marshal/marshal_types.h
// OBINexus Marshal Types: Constitutional Type Definition Framework
#ifndef OBINEXUS_NLINK_CORE_MARSHAL_MARSHAL_TYPES_H
#define OBINEXUS_NLINK_CORE_MARSHAL_MARSHAL_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Constitutional Marshal Context Complete Type Definition
struct marshal_context {
    uint32_t magic_marker;          // Constitutional validation marker: 0x4D415253 "MARS"
    uint8_t *buffer;                // Serialization buffer memory
    size_t buffer_size;             // Total buffer allocation size
    size_t position;                // Current buffer position cursor
    bool initialized;               // Context initialization state flag
    void *metadata_registry;        // Metadata coordination registry
    size_t registry_size;           // Registry allocation management
};

// Constitutional Marshal Buffer Management Structure
typedef struct marshal_buffer {
    uint8_t *data;                  // Buffer data storage
    size_t size;                    // Current data size
    size_t capacity;                // Total buffer capacity
    size_t position;                // Buffer position cursor
    bool locked;                    // Buffer lock state
} marshal_buffer_t;

// Advanced Marshal Configuration Structure
typedef struct marshal_config {
    uint32_t version;               // Marshal protocol version
    bool enable_compression;        // Compression coordination flag
    bool enable_encryption;         // Encryption boundary management
    size_t default_buffer_size;     // Default buffer allocation size
} marshal_config_t;

#ifdef __cplusplus
}
#endif

#endif // OBINEXUS_NLINK_CORE_MARSHAL_MARSHAL_TYPES_H
