# NexusLink Python Data Marshalling

Pure Python implementation of the OBINexus Mathematical Framework for zero-overhead data marshalling in distributed safety-critical systems.

## Features

- Cross-language compatibility with C and Java implementations
- Cryptographic integrity validation
- Topology-aware marshalling
- Zero-overhead design principles
- Thread-safe operations

## Installation

```bash
pip install nlink-python-marshal
```

## Usage

```python
from nlink_marshal import create_marshaller

# Create marshaller instance
marshaller = create_marshaller()

# Marshal data
data = [1.0, 2.5, 3.14, 4.7]
marshalled = marshaller.marshal_data(data)

# Unmarshal data
recovered = marshaller.unmarshal_data(marshalled)
assert data == recovered
```
