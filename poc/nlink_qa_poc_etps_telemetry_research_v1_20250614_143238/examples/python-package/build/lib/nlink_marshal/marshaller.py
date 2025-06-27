"""
Pure Python implementation of zero-overhead data marshalling
"""

import struct
import hashlib
import threading
from typing import List, Tuple, Optional
from dataclasses import dataclass


@dataclass
class MarshalHeader:
    """Header structure matching C/Java implementations"""
    version: int = 1
    payload_size: int = 0
    checksum: int = 0
    topology_id: int = 0
    
    def to_bytes(self) -> bytes:
        """Convert header to binary format"""
        return struct.pack('<IIII', 
                          self.version, 
                          self.payload_size,
                          self.checksum, 
                          self.topology_id)
    
    @classmethod
    def from_bytes(cls, data: bytes) -> 'MarshalHeader':
        """Create header from binary data"""
        version, payload_size, checksum, topology_id = struct.unpack('<IIII', data[:16])
        return cls(version, payload_size, checksum, topology_id)


class PythonMarshaller:
    """
    Pure Python implementation of NexusLink zero-overhead marshalling
    
    Provides compatibility layer for systems that cannot use compiled extensions
    while maintaining the same interface and security guarantees.
    """
    
    _topology_counter = 0
    _counter_lock = threading.Lock()
    
    def __init__(self, initial_size: int = 1024):
        self.buffer_size = initial_size
        with PythonMarshaller._counter_lock:
            PythonMarshaller._topology_counter += 1
            self.topology_id = PythonMarshaller._topology_counter
    
    def marshal_data(self, data: List[float]) -> bytes:
        """
        Marshal floating point data with cryptographic integrity
        
        Args:
            data: List of floating point numbers
            
        Returns:
            Marshalled bytes with header and payload
        """
        # Convert data to bytes
        payload = struct.pack('<' + 'd' * len(data), *data)
        payload_size = len(payload)
        
        # Compute checksum
        checksum = self._compute_checksum(data)
        
        # Create header
        header = MarshalHeader(
            version=1,
            payload_size=payload_size,
            checksum=checksum,
            topology_id=self.topology_id
        )
        
        # Combine header and payload
        return header.to_bytes() + payload
    
    def unmarshal_data(self, marshalled_data: bytes) -> List[float]:
        """
        Unmarshal data with integrity validation
        
        Args:
            marshalled_data: Previously marshalled bytes
            
        Returns:
            Original floating point data
            
        Raises:
            SecurityError: If validation fails
        """
        if len(marshalled_data) < 16:
            raise SecurityError("Invalid marshalled data: too short")
        
        # Parse header
        header = MarshalHeader.from_bytes(marshalled_data)
        
        # Validate version
        if header.version != 1:
            raise SecurityError(f"Invalid version: {header.version}")
        
        # Extract payload
        payload = marshalled_data[16:16 + header.payload_size]
        if len(payload) != header.payload_size:
            raise SecurityError("Payload size mismatch")
        
        # Unpack data
        double_count = header.payload_size // 8
        data = list(struct.unpack('<' + 'd' * double_count, payload))
        
        # Verify checksum
        computed_checksum = self._compute_checksum(data)
        if header.checksum != computed_checksum:
            raise SecurityError("Checksum validation failed")
        
        return data
    
    def _compute_checksum(self, data: List[float]) -> int:
        """Compute cryptographic checksum matching other implementations"""
        # Convert to bytes
        byte_data = struct.pack('<' + 'd' * len(data), *data)
        
        # Compute SHA-256 hash
        hash_obj = hashlib.sha256(byte_data)
        hash_bytes = hash_obj.digest()
        
        # Convert first 4 bytes to int
        return struct.unpack('<I', hash_bytes[:4])[0]
    
    @property
    def get_topology_id(self) -> int:
        """Get topology ID for this marshaller instance"""
        return self.topology_id


class SecurityError(Exception):
    """Exception raised for security validation failures"""
    pass


def create_marshaller() -> PythonMarshaller:
    """Factory function for creating marshaller instances"""
    return PythonMarshaller()
