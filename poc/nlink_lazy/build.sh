#!/bin/bash
# Build script for NexusLink lazy loading demonstration

echo "Building NexusLink PoC..."

# Compile cold function into shared library
gcc -shared -fPIC cold.c -o libcold.so

# Compile main program with lazy loading system
gcc main.c -ldl -o nexus_demo

echo "Build complete. Run with ./nexus_demo"