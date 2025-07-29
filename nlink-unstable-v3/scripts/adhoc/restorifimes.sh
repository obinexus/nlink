#!/bin/bash

# Clean build artifacts
rm -rf build/

# Execute component structure restoration
python3 generate_component_build_files.py

# Verify ExecutableConfig.cmake exists
if [ ! -f "cmake/ExecutableConfig.cmake" ]; then
    echo "ERROR: ExecutableConfig.cmake missing. Cannot proceed with build."
    exit 1
fi

# Verify BuildSystem.cmake has correct path
if grep -q "cmake/cmake/ExecutableConfig.cmake" "cmake/BuildSystem.cmake"; then
    echo "Correcting path in BuildSystem.cmake..."
    sed -i 's|cmake/cmake/ExecutableConfig.cmake|ExecutableConfig.cmake|g' cmake/BuildSystem.cmake
fi

# Execute build with proper concurrency
make -j$(nproc)

# Verify component artifacts
for component in common minimizer symbols versioning pipeline pattern mpsystem spsystem metadata; do
    if [ ! -f "build/lib/libnlink_${component}.a" ]; then
        echo "ERROR: Component library missing: libnlink_${component}.a"
    else
        echo "VERIFIED: Component library exists: libnlink_${component}.a"
    fi
done

# Verify unified library
if [ ! -f "build/lib/libnlink_core.a" ]; then
    echo "ERROR: Unified library missing: libnlink_core.a"
else
    echo "VERIFIED: Unified library exists: libnlink_core.a"
fi
