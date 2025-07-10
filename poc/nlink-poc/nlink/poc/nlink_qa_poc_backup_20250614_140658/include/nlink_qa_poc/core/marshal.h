/**
 * @file marshal.h
 * @brief NexusLink Zero-Overhead Data Marshalling
 * @version 1.0.0
 */

#ifndef NLINK_CORE_MARSHAL_H
#define NLINK_CORE_MARSHAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/* Marshalling header structure (16 bytes, matching other implementations) */
typedef struct nlink_marshal_header {
    uint32_t version;
    uint32_t payload_size;
    uint32_t checksum;
    uint32_t topology_id;
} nlink_marshal_header_t;

/* Marshalling context */
typedef struct nlink_marshaller {
    uint8_t* buffer;
    size_t buffer_size;
    uint32_t topology_id;
    uint64_t marshal_count;
    uint64_t error_count;
} nlink_marshaller_t;

/* Marshalling API */
int nlink_marshaller_create(nlink_marshaller_t** marshaller, size_t initial_size);
void nlink_marshaller_destroy(nlink_marshaller_t* marshaller);

int nlink_marshal_data(nlink_marshaller_t* marshaller, 
                      const double* data, size_t count,
                      uint8_t** output, size_t* output_size);

int nlink_unmarshal_data(nlink_marshaller_t* marshaller,
                        const uint8_t* input, size_t input_size,
                        double** output, size_t* output_count);

uint32_t nlink_compute_checksum(const uint8_t* data, size_t size);
int nlink_verify_header(const nlink_marshal_header_t* header);

#ifdef __cplusplus
}
#endif

#endif /* NLINK_CORE_MARSHAL_H */
