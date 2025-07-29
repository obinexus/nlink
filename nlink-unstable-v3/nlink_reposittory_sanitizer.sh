#!/bin/bash
# nlink_repository_sanitizer.sh - Complete build artifact purge protocol

# Phase 1: Aggressive artifact removal from tracking
purge_cmake_contamination() {
    echo "🔥 Initiating comprehensive CMake artifact purge..."
    
    # Remove ALL CMakeFiles directories recursively
    find . -type d -name "CMakeFiles" -exec git rm -rf --cached {} + 2>/dev/null
    find . -type d -name "*_build" -exec git rm -rf --cached {} + 2>/dev/null
    find . -type d -name "*_build_build" -exec git rm -rf --cached {} + 2>/dev/null
    
    # Remove test artifacts
    git rm -rf --cached Testing/ tests/cmake/ tests/integration/ 2>/dev/null
    
    # Remove generated directories
    git rm -rf --cached bin/ lib/ obj/ logs/ reports/ 2>/dev/null
    
    # Remove ALL .cmake files except critical ones
    find . -name "*.cmake" ! -name "CMakeLists.txt" -exec git rm -f --cached {} + 2>/dev/null
    
    echo "✅ Tracking contamination removed"
}

# Phase 2: Physical filesystem cleanup
sanitize_filesystem() {
    echo "🧹 Physical artifact removal..."
    git clean -fdx
}

# Phase 3: Create robust .gitignore
establish_hygiene_protocol() {
    cat > .gitignore << 'EOF'
# === OBINexus NexusLink Build Hygiene Protocol ===
# NO BUILD ARTIFACTS IN VERSION CONTROL

# CMake contamination prevention
**/CMakeFiles/
**/CMakeCache.txt
**/*.cmake
!CMakeLists.txt
!cmake/*.cmake
!NexusLinkConfig.cmake.in

# Build directory patterns
**/*_build/
**/*_build_build/
**/build/

# Output artifacts
bin/
lib/
obj/
logs/
reports/
Testing/
tests/cmake/
tests/integration/

# CTest/CPack
CTestTestfile.cmake
DartConfiguration.tcl
CPackConfig.cmake
CPackSourceConfig.cmake

# IDE artifacts
.vscode/
.idea/
*.swp
*.swo
*~
.DS_Store

# Backup files
*.bak
*.old
*.orig

# Temporary build logs
*.log
!nlink-unstable-v3.log  # Preserve specific logs if needed
EOF
    
    git add .gitignore
    echo "✅ Hygiene protocol established"
}
