/**
 * @file sps_stream.c
 * @brief Data streaming for single-pass systems
 *
 * Implements data streaming functionality for passing data between
 * components in a single-pass pipeline system.
 *
 * Copyright © 2025 OBINexus Computing
 */

 #include "nlink/spsystem/sps_stream.h"
 #include "nlink/core/common/nexus_core.h"
 #include <string.h>
 #include <stdlib.h>
 
 /* Forward declarations for helper functions */
 static NexusResult ensure_stream_capacity(NexusDataStream* stream, size_t required_size);
 static void free_metadata_entry(StreamMetadataEntry* entry);
 
 /**
  * Create a new data stream
  */
 NexusDataStream* sps_stream_create(size_t initial_capacity) {
     // Use a minimum size to avoid frequent resizing
     if (initial_capacity < 128) {
         initial_capacity = 128;
     }
     
     NexusDataStream* stream = (NexusDataStream*)calloc(1, sizeof(NexusDataStream));
     if (!stream) {
         return NULL;
     }
     
     // Allocate the data buffer
     stream->data = malloc(initial_capacity);
     if (!stream->data) {
         free(stream);
         return NULL;
     }
     
     // Initialize stream properties
     stream->capacity = initial_capacity;
     stream->size = 0;
     stream->position = 0;
     stream->format = NULL;
     stream->metadata = NULL;
     stream->owns_data = true;
     
     return stream;
 }
 
 /**
  * Create a data stream from existing data
  */
 NexusDataStream* sps_stream_create_from_data(const void* data, size_t size, const char* format) {
     if (!data || size == 0) {
         return NULL;
     }
     
     NexusDataStream* stream = sps_stream_create(size);
     if (!stream) {
         return NULL;
     }
     
     // Copy data into the stream
     memcpy(stream->data, data, size);
     stream->size = size;
     
     // Set format if provided
     if (format) {
         stream->format = strdup(format);
     }
     
     return stream;
 }
 
 /**
  * Resize a data stream
  */
 NexusResult sps_stream_resize(NexusDataStream* stream, size_t new_capacity) {
     if (!stream) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     // Don't resize if smaller than current size
     if (new_capacity < stream->size) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     // Don't resize if not needed
     if (new_capacity == stream->capacity) {
         return NEXUS_SUCCESS;
     }
     
     // Allocate new buffer
     void* new_data = realloc(stream->data, new_capacity);
     if (!new_data) {
         return NEXUS_OUT_OF_MEMORY;
     }
     
     // Update stream properties
     stream->data = new_data;
     stream->capacity = new_capacity;
     
     return NEXUS_SUCCESS;
 }
 
 /**
  * Ensure a stream has enough capacity
  */
 static NexusResult ensure_stream_capacity(NexusDataStream* stream, size_t required_size) {
     if (!stream) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     // Check if we have enough capacity
     if (stream->capacity >= required_size) {
         return NEXUS_SUCCESS;
     }
     
     // Calculate new capacity (grow by 1.5x or to required size, whichever is larger)
     size_t new_capacity = stream->capacity * 3 / 2;
     if (new_capacity < required_size) {
         new_capacity = required_size;
     }
     
     // Resize the stream
     return sps_stream_resize(stream, new_capacity);
 }
 
 /**
  * Write data to a stream
  */
 NexusResult sps_stream_write(NexusDataStream* stream, const void* data, size_t size) {
     if (!stream || !data || size == 0) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     // If writing at the end, grow the stream
     size_t end_pos = stream->position + size;
     if (end_pos > stream->capacity) {
         NexusResult result = ensure_stream_capacity(stream, end_pos);
         if (result != NEXUS_SUCCESS) {
             return result;
         }
     }
     
     // Copy data into the stream at the current position
     memcpy((char*)stream->data + stream->position, data, size);
     stream->position += size;
     
     // Update size if we've extended the stream
     if (stream->position > stream->size) {
         stream->size = stream->position;
     }
     
     return NEXUS_SUCCESS;
 }
 
 /**
  * Read data from a stream
  */
 NexusResult sps_stream_read(NexusDataStream* stream, void* buffer, size_t size, size_t* bytes_read) {
     if (!stream || !buffer || size == 0) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     // Calculate how many bytes we can read
     size_t available = stream->size - stream->position;
     size_t to_read = size;
     
     if (available < to_read) {
         to_read = available;
     }
     
     // Copy data to the buffer
     if (to_read > 0) {
         memcpy(buffer, (char*)stream->data + stream->position, to_read);
         stream->position += to_read;
     }
     
     // Update bytes_read if provided
     if (bytes_read) {
         *bytes_read = to_read;
     }
     
     // Return success even if we read fewer bytes than requested
     return NEXUS_SUCCESS;
 }
 
 /**
  * Get stream metadata
  */
 void* sps_stream_get_metadata(const NexusDataStream* stream, const char* key) {
     if (!stream || !key) {
         return NULL;
     }
     
     // Search for the key in the metadata linked list
     StreamMetadataEntry* entry = stream->metadata;
     while (entry) {
         if (strcmp(entry->key, key) == 0) {
             return entry->value;
         }
         entry = entry->next;
     }
     
     return NULL;
 }
 
 /**
  * Set stream metadata
  */
 NexusResult sps_stream_set_metadata(NexusDataStream* stream, const char* key, void* value, StreamMetadataFreeFunc free_func) {
     if (!stream || !key) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     // Check if the key already exists
     StreamMetadataEntry* entry = stream->metadata;
     while (entry) {
         if (strcmp(entry->key, key) == 0) {
             // Free old value if we have a free function
             if (entry->value && entry->free_func) {
                 entry->free_func(entry->value);
             }
             
             // Update the entry
             entry->value = value;
             entry->free_func = free_func;
             return NEXUS_SUCCESS;
         }
         entry = entry->next;
     }
     
     // Create a new entry
     StreamMetadataEntry* new_entry = (StreamMetadataEntry*)malloc(sizeof(StreamMetadataEntry));
     if (!new_entry) {
         return NEXUS_OUT_OF_MEMORY;
     }
     
     // Initialize the entry
     new_entry->key = strdup(key);
     if (!new_entry->key) {
         free(new_entry);
         return NEXUS_OUT_OF_MEMORY;
     }
     
     new_entry->value = value;
     new_entry->free_func = free_func;
     
     // Add to the head of the list
     new_entry->next = stream->metadata;
     stream->metadata = new_entry;
     
     return NEXUS_SUCCESS;
 }
 
 /**
  * Free metadata entry
  */
 static void free_metadata_entry(StreamMetadataEntry* entry) {
     if (!entry) {
         return;
     }
     
     // Free key
     free(entry->key);
     
     // Free value if we have a free function
     if (entry->value && entry->free_func) {
         entry->free_func(entry->value);
     }
     
     // Free the entry itself
     free(entry);
 }
 
 /**
  * Clear a stream (reset position but keep capacity)
  */
 void sps_stream_clear(NexusDataStream* stream) {
     if (!stream) {
         return;
     }
     
     // Reset position and size, but keep the buffer
     stream->position = 0;
     stream->size = 0;
 }
 
 /**
  * Reset a stream to initial state
  */
 void sps_stream_reset(NexusDataStream* stream) {
     if (!stream) {
         return;
     }
     
     // Clear the stream
     sps_stream_clear(stream);
     
     // Reset position to beginning
     stream->position = 0;
 }
 
 /**
  * Free stream resources
  */
 void sps_stream_destroy(NexusDataStream* stream) {
     if (!stream) {
         return;
     }
     
     // Free the data buffer if we own it
     if (stream->data && stream->owns_data) {
         free(stream->data);
     }
     
     // Free the format string
     if (stream->format) {
         free((void*)stream->format);
     }
     
     // Free metadata entries
     StreamMetadataEntry* entry = stream->metadata;
     while (entry) {
         StreamMetadataEntry* next = entry->next;
         free_metadata_entry(entry);
         entry = next;
     }
     
     // Free the stream itself
     free(stream);
 }
 
 /**
  * Clone a stream
  */
 NexusDataStream* sps_stream_clone(const NexusDataStream* stream) {
     if (!stream) {
         return NULL;
     }
     
     // Create a new stream with the same capacity
     NexusDataStream* clone = sps_stream_create(stream->capacity);
     if (!clone) {
         return NULL;
     }
     
     // Copy data
     memcpy(clone->data, stream->data, stream->size);
     clone->size = stream->size;
     clone->position = stream->position;
     
     // Copy format if available
     if (stream->format) {
         clone->format = strdup(stream->format);
         if (!clone->format) {
             sps_stream_destroy(clone);
             return NULL;
         }
     }
     
     // Copy metadata
     StreamMetadataEntry* entry = stream->metadata;
     while (entry) {
         // Deep copy would require knowing how to clone the values
         // For now, we'll just copy the references
         // In a real implementation, we'd need a way to clone metadata values
         sps_stream_set_metadata(clone, entry->key, entry->value, entry->free_func);
         entry = entry->next;
     }
     
     return clone;
 }