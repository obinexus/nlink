"""
Pure Python fallback implementation for when Cython compilation is unavailable
"""

import numpy as np
import struct
import hashlib
from typing import Union

class ZeroOverheadMarshaller:
    """Pure Python fallback marshaller"""
    
    def __init__(self, initial_size: int = 1024):
        self.buffer_size = initial_size
        self.topology_id = id(self) & 0xFFFFFFFF
        self.marshal_count = 0
        self.error_count = 0
    
    def marshal_data(self, data: Union[np.ndarray, list]) -> bytes:
        """Marshal data using pure Python"""
        if isinstance(data, list):
            data = np.array(data, dtype=np.float64)
        
        payload = data.tobytes()
        payload_size = len(payload)
        
        # Simple checksum
        checksum = sum(payload) & 0xFFFFFFFF
        
        # Create header
        header = struct.pack('<IIII', 1, payload_size, checksum, self.topology_id)
        
        self.marshal_count += 1
        return header + payload
    
    def unmarshal_data(self, marshalled_data: bytes) -> np.ndarray:
        """Unmarshal data using pure Python"""
        if len(marshalled_data) < 16:
            self.error_count += 1
            raise ValueError("Invalid marshalled data")
        
        header = marshalled_data[:16]
        payload = marshalled_data[16:]
        
        version, payload_size, checksum, topology_id = struct.unpack('<IIII', header)
        
        if version != 1:
            self.error_count += 1
            raise ValueError(f"Invalid version: {version}")
        
        # Verify checksum
        if sum(payload) & 0xFFFFFFFF != checksum:
            self.error_count += 1
            raise ValueError("Checksum validation failed")
        
        return np.frombuffer(payload, dtype=np.float64)
    
    def get_topology_id(self):
        return self.topology_id
    
    def get_statistics(self):
        return {
            "topology_id": self.topology_id,
            "marshal_count": self.marshal_count,
            "error_count": self.error_count,
            "success_rate": self.marshal_count / max(self.marshal_count + self.error_count, 1),
            "buffer_size": self.buffer_size
        }

def create_marshaller(initial_size: int = 1024):
    return ZeroOverheadMarshaller(initial_size)
