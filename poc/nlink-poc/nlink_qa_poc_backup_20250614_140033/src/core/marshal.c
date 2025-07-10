/**
 * @file marshal.c
 * @brief NexusLink Marshalling Implementation
 */

#include "nlink_qa_poc/core/marshal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint32_t g_topology_counter = 0;

int nlink_marshaller_create(nlink_marshaller_t** marshaller, size_t initial_size) {
    if (!marshaller) return -1;
    
    nlink_marshaller_t* m = malloc(sizeof(nlink_marshaller_t));
    if (!m) return -1;
    
    m->buffer = malloc(initial_size);
    if (!m->buffer) {
        free(m);
        return -1;
    }
    
    m->buffer_size = initial_size;
    m->topology_id = ++g_topology_counter;
    m->marshal_count = 0;
    m->error_count = 0;
    
    *marshaller = m;
    return 0;
}

void nlink_marshaller_destroy(nlink_marshaller_t* marshaller) {
    if (marshaller) {
        free(marshaller->buffer);
        free(marshaller);
    }
}

int nlink_marshal_data(nlink_marshaller_t* marshaller,
                      const double* data, size_t count,
                      uint8_t** output, size_t* output_size) {
    if (!marshaller || !data || !output || !output_size) return -1;
    
    size_t data_size = count * sizeof(double);
    size_t total_size = sizeof(nlink_marshal_header_t) + data_size;
    
    /* Ensure buffer capacity */
    if (total_size > marshaller->buffer_size) {
        size_t new_size = total_size * 2;
        uint8_t* new_buffer = realloc(marshaller->buffer, new_size);
        if (!new_buffer) {
            marshaller->error_count++;
            return -1;
        }
        marshaller->buffer = new_buffer;
        marshaller->buffer_size = new_size;
    }
    
    /* Create header */
    nlink_marshal_header_t header = {
        .version = 1,
        .payload_size = (uint32_t)data_size,
        .checksum = nlink_compute_checksum((const uint8_t*)data, data_size),
        .topology_id = marshaller->topology_id
    };
    
    /* Copy header and data */
    memcpy(marshaller->buffer, &header, sizeof(header));
    memcpy(marshaller->buffer + sizeof(header), data, data_size);
    
    /* Allocate output buffer */
    *output = malloc(total_size);
    if (!*output) {
        marshaller->error_count++;
        return -1;
    }
    
    memcpy(*output, marshaller->buffer, total_size);
    *output_size = total_size;
    
    marshaller->marshal_count++;
    return 0;
}

int nlink_unmarshal_data(nlink_marshaller_t* marshaller,
                        const uint8_t* input, size_t input_size,
                        double** output, size_t* output_count) {
    if (!marshaller || !input || !output || !output_count) return -1;
    
    if (input_size < sizeof(nlink_marshal_header_t)) {
        marshaller->error_count++;
        return -1;
    }
    
    /* Parse header */
    const nlink_marshal_header_t* header = (const nlink_marshal_header_t*)input;
    
    if (nlink_verify_header(header) != 0) {
        marshaller->error_count++;
        return -1;
    }
    
    if (input_size != sizeof(nlink_marshal_header_t) + header->payload_size) {
        marshaller->error_count++;
        return -1;
    }
    
    /* Extract data */
    const uint8_t* payload = input + sizeof(nlink_marshal_header_t);
    size_t count = header->payload_size / sizeof(double);
    
    /* Verify checksum */
    uint32_t computed_checksum = nlink_compute_checksum(payload, header->payload_size);
    if (computed_checksum != header->checksum) {
        marshaller->error_count++;
        return -1;
    }
    
    /* Allocate output */
    *output = malloc(header->payload_size);
    if (!*output) {
        marshaller->error_count++;
        return -1;
    }
    
    memcpy(*output, payload, header->payload_size);
    *output_count = count;
    
    return 0;
}

uint32_t nlink_compute_checksum(const uint8_t* data, size_t size) {
    uint32_t checksum = 0;
    for (size_t i = 0; i < size; i++) {
        checksum ^= data[i];
        checksum = (checksum << 1) | (checksum >> 31);  /* Rotate left */
    }
    return checksum;
}

int nlink_verify_header(const nlink_marshal_header_t* header) {
    if (!header) return -1;
    if (header->version != 1) return -1;
    if (header->payload_size == 0) return -1;
    return 0;
}
