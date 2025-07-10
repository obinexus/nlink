"""
NexusLink Python Data Marshalling Library

Pure Python implementation of the OBINexus Mathematical Framework
for zero-overhead data marshalling in distributed systems.
"""

from .marshaller import PythonMarshaller
from .topology import TopologyManager
from .validation import SecurityValidator

__version__ = "1.0.0"
__author__ = "OBINexus Engineering Team"

__all__ = [
    "PythonMarshaller",
    "TopologyManager", 
    "SecurityValidator",
]
