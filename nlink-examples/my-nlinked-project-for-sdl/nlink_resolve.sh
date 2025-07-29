#!/bin/bash
# nlink_resolve.sh - Resolve NLink components

# Project root
PROJECT_ROOT="."
cd "$PROJECT_ROOT" || exit 1

# Initialize NLink
echo "Initializing NLink system..."
nlink init

# Resolve sound processing diram component
echo "Resolving sound-processing.memory_management.diram component..."
nlink resolve "sound-processing.memory_management.diram" \
      --context="sdl.project" \
      --output="build/nlink/nlink_diram.elf"

# Check resolution status
if [ $? -ne 0 ]; then
    echo "Failed to resolve diram component"
    exit 1
fi

echo "NLink resolution complete"
