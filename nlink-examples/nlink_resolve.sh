#!/bin/bash
# nlink_resolve.sh - Resolve NLink components

# Project root
PROJECT_ROOT="my-nlinked-project-for-sdl"
cd "$PROJECT_ROOT" || exit 1

# Initialize NLink
echo "Initializing NLink system..."
nlink init

# Resolve sound processing diram component
echo "Resolving sound-processing.memory_management.diram component..."
nlink resolve "sound-processing.memory_management.diram" \
      --context="sdl.project" \
      --output="nlink_diram.elf"

# Check resolution status
if [ $? -ne 0 ]; then
    echo "Failed to resolve diram component"
    exit 1
fi

# Resolve game windows SDL component
echo "Resolving game-windows.window_management.sdl component..."
nlink resolve "game-windows.window_management.sdl" \
      --context="sdl.project" \
      --output="nlink_sdl.elf"

# Check resolution status
if [ $? -ne 0 ]; then
    echo "Failed to resolve SDL component"
    exit 1
fi

# Validate component linking with consciousness preservation
echo "Validating consciousness preservation..."
nlink validate --consciousness-preservation

# Build the project with resolved components
echo "Building project with resolved components..."
make

echo "NLink resolution complete"
