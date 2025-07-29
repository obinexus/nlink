#!/bin/bash
# OBINexus NexusLink - Include Path Migration Script
# Systematically updates all include paths to use nlink/ prefix

echo "=== NexusLink Include Path Migration ==="
echo "Updating include paths from nlink_qa_poc/ to nlink/"

# Define the include mappings
declare -A INCLUDE_MAPPINGS=(
    ["nlink_qa_poc/etps/telemetry.h"]="nlink/core/etps/telemetry.h"
    ["nlink_qa_poc/etps/semverx_etps.h"]="nlink/core/etps/semverx_etps.h"
    ["nlink_qa_poc/etps/epts_telemtry.h"]="nlink/core/etps/epts_telemtry.h"
    # Add more mappings as needed
)

# Update C source files
find src/ -name "*.c" -type f | while read file; do
    echo "Processing: $file"
    cp "$file" "$file.backup"
    
    # Update each mapping
    for old_path in "${!INCLUDE_MAPPINGS[@]}"; do
        new_path="${INCLUDE_MAPPINGS[$old_path]}"
        sed -i "s|#include \"$old_path\"|#include \"$new_path\"|g" "$file"
    done
    
    # General pattern replacement
    sed -i 's|#include "nlink_qa_poc/\([^"]*\)"|#include "nlink/\1"|g' "$file"
done

# Update header files
find include/ -name "*.h" -type f | while read file; do
    echo "Processing: $file"
    cp "$file" "$file.backup"
    
    for old_path in "${!INCLUDE_MAPPINGS[@]}"; do
        new_path="${INCLUDE_MAPPINGS[$old_path]}"
        sed -i "s|#include \"$old_path\"|#include \"$new_path\"|g" "$file"
    done
    
    sed -i 's|#include "nlink_qa_poc/\([^"]*\)"|#include "nlink/\1"|g' "$file"
done

echo "Include path migration complete"
