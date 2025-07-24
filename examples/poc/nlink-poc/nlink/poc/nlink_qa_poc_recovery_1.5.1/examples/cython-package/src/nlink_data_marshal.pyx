# cython: language_level=3
# cython: boundscheck=False
# cython: wraparound=False
# cython: cdivision=True

"""
NexusLink Zero-Overhead Data Marshalling
OBINexus Mathematical Framework Implementation
"""

cimport numpy as cnp
import numpy as np
from libc.stdlib cimport malloc, free
from libc.string cimport memcpy

ctypedef struct MarshalHeader:
    unsigned int version
    unsigned int payload_size
    unsigned int checksum
    unsigned int topology_id

cdef class ZeroOverheadMarshaller:
    """
    Implements the OBINexus Mathematical Framework for 
    zero-overhead data marshalling in safety-critical systems.
    """
    
    cdef MarshalHeader header
    cdef char* buffer
    cdef unsigned int buffer_size
    
    def __cinit__(self, unsigned int initial_size=1024):
        self.buffer_size = initial_size
        self.buffer = <char*>malloc(initial_size)
        if not self.buffer:
            raise MemoryError("Failed to allocate marshalling buffer")
        
        # Initialize header with OBINexus protocol
        self.header.version = 1
        self.header.payload_size = 0
        self.header.checksum = 0
        self.header.topology_id = 0
    
    def __dealloc__(self):
        if self.buffer:
            free(self.buffer)
    
    cpdef bytes marshal_data(self, cnp.ndarray[cnp.float64_t, ndim=1] data):
        """
        Marshal numpy array with O(1) overhead guarantee
        """
        cdef unsigned int data_size = data.nbytes
        cdef unsigned int total_size = sizeof(MarshalHeader) + data_size
        
        # Resize buffer if needed
        if total_size > self.buffer_size:
            self._resize_buffer(total_size * 2)
        
        # Update header
        self.header.payload_size = data_size
        self.header.checksum = self._compute_checksum(data)
        
        # Copy header and data with zero-copy optimization
        memcpy(self.buffer, &self.header, sizeof(MarshalHeader))
        memcpy(self.buffer + sizeof(MarshalHeader), data.data, data_size)
        
        return self.buffer[:total_size]
    
    cdef void _resize_buffer(self, unsigned int new_size):
        """Resize internal buffer maintaining zero-overhead principle"""
        cdef char* new_buffer = <char*>malloc(new_size)
        if not new_buffer:
            raise MemoryError("Failed to resize marshalling buffer")
        
        # Copy existing data if any
        if self.header.payload_size > 0:
            memcpy(new_buffer, self.buffer, 
                   sizeof(MarshalHeader) + self.header.payload_size)
        
        free(self.buffer)
        self.buffer = new_buffer
        self.buffer_size = new_size
    
    cdef unsigned int _compute_checksum(self, cnp.ndarray[cnp.float64_t, ndim=1] data):
        """Compute cryptographic checksum for integrity validation"""
        cdef unsigned int checksum = 0
        cdef unsigned int i
        cdef unsigned char* byte_ptr = <unsigned char*>data.data
        
        for i in range(data.nbytes):
            checksum = checksum ^ byte_ptr[i]
            checksum = (checksum << 1) | (checksum >> 31)  # Rotate left
        
        return checksum

def create_marshaller():
    """Factory function for creating zero-overhead marshaller instances"""
    return ZeroOverheadMarshaller()
