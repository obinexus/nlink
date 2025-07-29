#!/bin/bash
# nlink_clean_merge.sh - OBINexus compliant merge protocol

clean_build_artifacts() {
    echo "🧹 Purging build contamination..."
    rm -rf build/ bin/ lib/ obj/
    rm -f CMakeCache.txt *.cmake
    rm -rf Testing/ CMakeFiles/
}

safe_merge() {
    local source_branch=$1
    local target_branch=$2
    
    clean_build_artifacts
    git merge ${source_branch} --no-ff --no-commit
    
    # Auto-remove any build artifacts from merge
    git rm -f --ignore-unmatch CMakeCache.txt 2>/dev/null
    git rm -rf --ignore-unmatch CMakeFiles/ 2>/dev/null
}
