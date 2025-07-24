"""
Modern Cython build script for NexusLink zero-overhead marshalling
Compatible with PEP-517 build isolation via pyproject.toml
"""

from setuptools import setup, Extension, find_packages
from Cython.Build import cythonize
import numpy
import os

# Define Cython extensions with optimization flags
extensions = [
    Extension(
        "nlink_data_marshal.core_marshal",
        ["src/nlink_data_marshal/core_marshal.pyx"],
        include_dirs=[
            numpy.get_include(),
            "src/nlink_data_marshal"
        ],
        extra_compile_args=[
            "-O3",           # Maximum optimization
            "-fPIC",         # Position independent code
            "-march=native", # Use native CPU features
            "-ffast-math",   # Fast math optimizations
            "-DNPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION"
        ],
        extra_link_args=["-shared"],
        define_macros=[
            ("NPY_NO_DEPRECATED_API", "NPY_1_7_API_VERSION"),
            ("CYTHON_TRACE", "0")  # Disable tracing for production
        ],
    )
]

# Cython compiler directives for zero-overhead guarantees
compiler_directives = {
    "language_level": "3",
    "boundscheck": False,       # Remove bounds checking
    "wraparound": False,        # Remove negative index support
    "initializedcheck": False,  # Remove initialization checks
    "cdivision": True,          # Use C division semantics
    "embedsignature": True,     # Embed function signatures
    "optimize.use_switch": True, # Use switch statements for optimization
    "optimize.unpack_method_calls": True,
    "warn.undeclared": True,    # Warn about undeclared variables
    "warn.unreachable": True,   # Warn about unreachable code
}

# Build extensions with optimization
ext_modules = cythonize(
    extensions,
    compiler_directives=compiler_directives,
    annotate=True,  # Generate HTML annotation files
)

# Use setup() for compatibility with pyproject.toml
if __name__ == "__main__":
    setup(ext_modules=ext_modules)
