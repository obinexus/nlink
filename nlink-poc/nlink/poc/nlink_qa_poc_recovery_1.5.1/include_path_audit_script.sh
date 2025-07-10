#!/bin/bash

# =============================================================================
# OBINexus NexusLink QA POC - Include Path Systematic Audit & Replace
# Comprehensive Search and Replace for nlink_qa_poc References - FIXED
# =============================================================================

set -e

# Color codes for structured output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m'

log_phase() { echo -e "${BLUE}[AUDIT PHASE]${NC} $1"; }
log_success() { echo -e "${GREEN}[SUCCESS]${NC} $1"; }
log_warning() { echo -e "${YELLOW}[WARNING]${NC} $1"; }
log_error() { echo -e "${RED}[ERROR]${NC} $1"; }
log_info() { echo -e "${CYAN}[INFO]${NC} $1"; }

echo "=============================================================================="
echo "🔍 OBINexus Include Path Systematic Audit & Replace"
echo "=============================================================================="
echo "Objective: Find and resolve ALL nlink_qa_poc include path references"
echo "Methodology: Waterfall-compliant systematic search and replace"
echo "Environment: nlink_qa_poc_recovery_1.5.1"
echo "=============================================================================="
echo ""

# Validate recovery environment
if [ ! -d "include/nlink_qa_poc" ]; then
    log_error "Recovery environment not detected. Expected: include/nlink_qa_poc/"
    exit 1
fi

log_success "Recovery environment validated: $(pwd)"

# =============================================================================
# Phase 1: Comprehensive Search for nlink_qa_poc References - FIXED
# =============================================================================

log_phase "1. Comprehensive search for all nlink_qa_poc references"

# Create audit report
AUDIT_REPORT="nlink_qa_poc_audit_$(date +%Y%m%d_%H%M%S).txt"
echo "OBINexus Include Path Audit Report" > "$AUDIT_REPORT"
echo "Generated: $(date)" >> "$AUDIT_REPORT"
echo "Environment: $(pwd)" >> "$AUDIT_REPORT"
echo "===========================================" >> "$AUDIT_REPORT"
echo "" >> "$AUDIT_REPORT"

# Search all source files for nlink_qa_poc references
echo "🔍 Scanning for nlink_qa_poc references in source files..."
echo ""

# Initialize arrays properly
declare -a ALL_FILES
FILE_COUNT=0

# Function to scan directory for files with nlink_qa_poc references
scan_directory() {
    local dir="$1"
    local description="$2"
    
    if [ ! -d "$dir" ]; then
        return 0
    fi
    
    log_info "Scanning $description directory: $dir"
    
    # Use find with explicit file type checking
    find "$dir" -type f \( -name "*.c" -o -name "*.h" \) | while read -r file; do
        if [ -f "$file" ] && grep -q "nlink_qa_poc" "$file" 2>/dev/null; then
            ALL_FILES+=("$file")
            ((FILE_COUNT++))
            
            echo "📄 $file:" | tee -a "$AUDIT_REPORT"
            grep -n "nlink_qa_poc" "$file" 2>/dev/null | head -10 | while read -r line; do
                echo "    $line" | tee -a "$AUDIT_REPORT"
            done
            echo "" | tee -a "$AUDIT_REPORT"
        fi
    done
}

# Scan directories systematically
scan_directory "src" "source"
scan_directory "include" "headers"
scan_directory "examples" "examples"
scan_directory "test" "test"

# Alternative approach: Direct find and grep
echo "📊 Collecting all files with nlink_qa_poc references..."

# Create temporary file list
TEMP_FILE_LIST=$(mktemp)
find . -type f \( -name "*.c" -o -name "*.h" \) -exec grep -l "nlink_qa_poc" {} \; 2>/dev/null > "$TEMP_FILE_LIST" || true

# Count files
FILE_COUNT=$(wc -l < "$TEMP_FILE_LIST")

echo "📊 AUDIT SUMMARY:" | tee -a "$AUDIT_REPORT"
echo "Files containing nlink_qa_poc references: $FILE_COUNT" | tee -a "$AUDIT_REPORT"
echo "" | tee -a "$AUDIT_REPORT"

# Process each file
if [ -s "$TEMP_FILE_LIST" ]; then
    while IFS= read -r file; do
        if [ -f "$file" ]; then
            echo "📄 $file:" | tee -a "$AUDIT_REPORT"
            grep -n "nlink_qa_poc" "$file" 2>/dev/null | head -10 | while IFS= read -r line; do
                echo "    $line" | tee -a "$AUDIT_REPORT"
            done
            echo "" | tee -a "$AUDIT_REPORT"
        fi
    done < "$TEMP_FILE_LIST"
fi

# =============================================================================
# Phase 2: Include Path Analysis - FIXED
# =============================================================================

log_phase "2. Include path pattern analysis"

echo "🔍 Analyzing include path patterns..." | tee -a "$AUDIT_REPORT"
echo "" | tee -a "$AUDIT_REPORT"

# Extract unique include patterns using simpler approach
UNIQUE_INCLUDES_FILE=$(mktemp)

# Find all include statements with nlink_qa_poc
find . -type f \( -name "*.c" -o -name "*.h" \) -exec grep -h "#include.*nlink_qa_poc" {} \; 2>/dev/null | \
    sed 's/.*#include[[:space:]]*[<"]\([^<>"]*\)[<>"].*/\1/' | \
    sort | uniq > "$UNIQUE_INCLUDES_FILE" || true

echo "UNIQUE INCLUDE PATTERNS FOUND:" | tee -a "$AUDIT_REPORT"
while IFS= read -r include; do
    if [ -n "$include" ]; then
        echo "  📎 #include \"$include\"" | tee -a "$AUDIT_REPORT"
        
        # Check if corresponding header exists
        if [ -f "include/$include" ]; then
            echo "    ✅ Header exists: include/$include" | tee -a "$AUDIT_REPORT"
        else
            echo "    ❌ Header missing: include/$include" | tee -a "$AUDIT_REPORT"
        fi
    fi
done < "$UNIQUE_INCLUDES_FILE"
echo "" | tee -a "$AUDIT_REPORT"

# =============================================================================
# Phase 3: Missing Header Detection - FIXED
# =============================================================================

log_phase "3. Missing header detection and cataloging"

echo "🔍 Detecting missing headers..." | tee -a "$AUDIT_REPORT"
echo "" | tee -a "$AUDIT_REPORT"

# Create missing headers list
MISSING_HEADERS_FILE=$(mktemp)

while IFS= read -r include; do
    if [ -n "$include" ] && [ ! -f "include/$include" ]; then
        echo "$include" >> "$MISSING_HEADERS_FILE"
    fi
done < "$UNIQUE_INCLUDES_FILE"

MISSING_COUNT=$(wc -l < "$MISSING_HEADERS_FILE" 2>/dev/null || echo "0")

if [ "$MISSING_COUNT" -gt 0 ]; then
    echo "❌ MISSING HEADERS DETECTED:" | tee -a "$AUDIT_REPORT"
    while IFS= read -r header; do
        if [ -n "$header" ]; then
            echo "  📄 include/$header" | tee -a "$AUDIT_REPORT"
        fi
    done < "$MISSING_HEADERS_FILE"
    echo "" | tee -a "$AUDIT_REPORT"
else
    echo "✅ All referenced headers are present" | tee -a "$AUDIT_REPORT"
    echo "" | tee -a "$AUDIT_REPORT"
fi

# =============================================================================
# Phase 4: Replace Strategy Options - FIXED
# =============================================================================

log_phase "4. Include path replacement strategy options"

echo "🔧 REPLACEMENT STRATEGY OPTIONS:" | tee -a "$AUDIT_REPORT"
echo "" | tee -a "$AUDIT_REPORT"

echo "Option 1: Remove nlink_qa_poc prefix (Direct header names)" | tee -a "$AUDIT_REPORT"
echo "  Example: #include \"nlink_qa_poc/core/config.h\" → #include \"core/config.h\"" | tee -a "$AUDIT_REPORT"
echo "  Requires: Adjusting -I include path to include/nlink_qa_poc" | tee -a "$AUDIT_REPORT"
echo "" | tee -a "$AUDIT_REPORT"

echo "Option 2: Keep nlink_qa_poc prefix (Current approach)" | tee -a "$AUDIT_REPORT"
echo "  Example: #include \"nlink_qa_poc/core/config.h\" (no change)" | tee -a "$AUDIT_REPORT"
echo "  Requires: Ensuring all headers exist in include/nlink_qa_poc/" | tee -a "$AUDIT_REPORT"
echo "" | tee -a "$AUDIT_REPORT"

echo "Option 3: Use absolute project paths" | tee -a "$AUDIT_REPORT"
echo "  Example: #include \"nlink_qa_poc/core/config.h\" → #include \"nlink/core/config.h\"" | tee -a "$AUDIT_REPORT"
echo "  Requires: Restructuring include directory to match OBINexus naming" | tee -a "$AUDIT_REPORT"
echo "" | tee -a "$AUDIT_REPORT"

# =============================================================================
# Phase 5: Systematic Replacement Execution - FIXED
# =============================================================================

log_phase "5. Executing systematic replacement"

# Default to safest strategy (create missing headers)
STRATEGY=2
echo "🔧 Selected Strategy: $STRATEGY (Keep nlink_qa_poc prefix, create missing headers)"

case $STRATEGY in
    2)
        log_info "Executing Strategy 2: Keep prefix, create missing headers"
        
        # Create missing headers
        if [ -s "$MISSING_HEADERS_FILE" ]; then
            while IFS= read -r header; do
                if [ -n "$header" ]; then
                    header_dir="include/$(dirname "$header")"
                    header_file="include/$header"
                    
                    log_info "Creating directory: $header_dir"
                    mkdir -p "$header_dir"
                    
                    # Extract header name for guard
                    header_name=$(basename "$header")
                    guard_name=$(echo "$header" | tr '[:lower:]/' '[:upper:]_' | sed 's/\.H$/_H/')
                    
                    log_info "Creating placeholder header: $header_file"
                    cat > "$header_file" << EOF
/**
 * @file $header_name
 * @brief Auto-generated header placeholder
 * @author OBINexus Systematic Recovery
 * @version 1.5.1
 * TODO: Implement proper header content based on source requirements
 */

#ifndef $guard_name
#define $guard_name

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// TODO: Add proper function declarations and type definitions
// This is a placeholder header created by systematic recovery

#ifdef __cplusplus
}
#endif

#endif // $guard_name
EOF
                fi
            done < "$MISSING_HEADERS_FILE"
        else
            log_info "No missing headers to create"
        fi
        ;;
esac

# =============================================================================
# Phase 6: Validation and Testing - FIXED
# =============================================================================

log_phase "6. Post-replacement validation"

echo "🧪 Testing header resolution after changes..."

# Test compilation of existing headers
test_header() {
    local header="$1"
    local test_file="test_$(basename "$header" .h).c"
    
    echo "#include \"$header\"" > "$test_file"
    echo "int main() { return 0; }" >> "$test_file"
    
    if gcc -I include -c "$test_file" -o "${test_file%.c}.o" 2>/dev/null; then
        echo "  ✅ $header: OK"
        rm -f "$test_file" "${test_file%.c}.o"
        return 0
    else
        echo "  ❌ $header: FAILED"
        rm -f "$test_file" "${test_file%.c}.o"
        return 1
    fi
}

# Test core headers
echo "Testing core headers..."
test_header "nlink_qa_poc/core/config.h" || true
test_header "nlink_qa_poc/core/marshal.h" || true
test_header "nlink_qa_poc/etps/telemetry.h" || true

# =============================================================================
# Phase 7: Summary Report - FIXED
# =============================================================================

log_phase "7. Systematic audit completion summary"

echo "" | tee -a "$AUDIT_REPORT"
echo "=============================================================================="
echo "🎯 INCLUDE PATH AUDIT COMPLETION SUMMARY"
echo "=============================================================================="
echo "Files processed: $FILE_COUNT" | tee -a "$AUDIT_REPORT"
echo "Missing headers detected: $MISSING_COUNT" | tee -a "$AUDIT_REPORT"
echo "Strategy applied: $STRATEGY" | tee -a "$AUDIT_REPORT"
echo "Audit report: $AUDIT_REPORT" | tee -a "$AUDIT_REPORT"
echo "" | tee -a "$AUDIT_REPORT"

if [ "$MISSING_COUNT" -eq 0 ]; then
    echo "✅ All include paths should now resolve correctly" | tee -a "$AUDIT_REPORT"
else
    echo "⚠️ Created $MISSING_COUNT placeholder headers - manual implementation required" | tee -a "$AUDIT_REPORT"
fi

echo ""
echo "📋 Next Steps:"
echo "1. Review audit report: $AUDIT_REPORT"
echo "2. Test compilation: make clean && make"
echo "3. Implement missing function bodies in created placeholder headers"
echo "4. Continue with systematic development"
echo ""
echo "🚀 Include path audit and replacement completed"
echo "=============================================================================="

# Cleanup temporary files
rm -f "$TEMP_FILE_LIST" "$UNIQUE_INCLUDES_FILE" "$MISSING_HEADERS_FILE" 2>/dev/null || true

log_success "Systematic include path audit completed successfully"
