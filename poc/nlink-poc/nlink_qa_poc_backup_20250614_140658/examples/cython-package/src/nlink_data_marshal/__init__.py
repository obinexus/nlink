"""
NexusLink Zero-Overhead Data Marshalling (Cython Implementation)

High-performance implementation of the OBINexus Mathematical Framework
for safety-critical distributed systems.
"""

try:
    # Import compiled extension if available
    from .core_marshal import ZeroOverheadMarshaller, create_marshaller
    _COMPILED_AVAILABLE = True
except ImportError:
    # Fallback to pure Python implementation
    from .python_fallback import ZeroOverheadMarshaller, create_marshaller
    _COMPILED_AVAILABLE = False

__version__ = "1.1.0"
__author__ = "OBINexus Engineering Team"

__all__ = [
    "ZeroOverheadMarshaller",
    "create_marshaller",
    "is_compiled_available",
]

def is_compiled_available() -> bool:
    """Check if compiled Cython extension is available"""
    return _COMPILED_AVAILABLE
