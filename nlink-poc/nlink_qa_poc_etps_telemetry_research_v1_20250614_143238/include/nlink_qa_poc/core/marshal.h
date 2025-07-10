/**
 * @file marshal.h
 * @brief NexusLink Zero-Overhead Data Marshalling
 * @author Nnamdi Michael Okpala & Aegis Development Team
 * @version 1.5.1
 */

#ifndef NLINK_QA_POC_CORE_MARSHAL_H
#define NLINK_QA_POC_CORE_MARSHAL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// Marshalling Header Structure (16 bytes, aligned)
// =============================================================================

typedef struct nlink_marshal_header {
    uint32_t version;           // Protocol version
    uint32_t payload_size;      // Data payload size in bytes
    uint32_t checksum;          // XOR-based checksum with rotation
    uint32_t topology_id;       // Unique topology identifier
} nlink_marshal_header_t;

// =============================================================================
// Marshalling Context Structure
// =============================================================================

typedef struct nlink_marshaller {
    uint8_t* buffer;            // Working buffer for marshalling operations
    size_t buffer_size;         // Current buffer capacity
    uint32_t topology_id;       // Topology identifier for this marshaller
    uint64_t marshal_count;     // Successful marshalling operations count
    uint64_t error_count;       // Error count for diagnostics
} nlink_marshaller_t;

// =============================================================================
// Core Marshalling API Functions
// =============================================================================

// Context management
int nlink_marshaller_create(nlink_marshaller_t** marshaller, size_t initial_size);
void nlink_marshaller_destroy(nlink_marshaller_t* marshaller);

// Data marshalling operations
int nlink_marshal_data(nlink_marshaller_t* marshaller, 
                      const double* data, size_t count,
                      uint8_t** output, size_t* output_size);

int nlink_unmarshal_data(nlink_marshaller_t* marshaller,
                        const uint8_t* input, size_t input_size,
                        double** output, size_t* output_count);

// Utility functions
uint32_t nlink_compute_checksum(const uint8_t* data, size_t size);
int nlink_verify_header(const nlink_marshal_header_t* header);

// Statistics and diagnostics
uint64_t nlink_marshaller_get_operation_count(const nlink_marshaller_t* marshaller);
uint64_t nlink_marshaller_get_error_count(const nlink_marshaller_t* marshaller);
uint32_t nlink_marshaller_get_topology_id(const nlink_marshaller_t* marshaller);

#ifdef __cplusplus
}
#endif

#endif // NLINK_QA_POC_CORE_MARSHAL_H
