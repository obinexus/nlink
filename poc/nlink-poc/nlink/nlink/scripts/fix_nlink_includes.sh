#!/bin/bash
# fix_nlink_includes.sh - Comprehensive include path fixer for NexusLink
#
# This script performs a comprehensive cleanup and standardization of include
# paths in the NexusLink codebase. It first removes invalid includes and then
# standardizes the remaining includes according to NexusLink's conventions.
#
# Author: Based on the architecture design by Nnamdi Okpala

set -e  # Exit on error

# Default values
PROJECT_ROOT="$(pwd)"
DRY_RUN=""
VERBOSE=""
BACKUP=""
HELP=0
REPORT_DIR="${PROJECT_ROOT}/reports/includes"
STRICT=""

# Print usage information
print_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -p, --project-root DIR   Project root directory (default: current directory)"
    echo "  -d, --dry-run            Show what would be changed without modifying files"
    echo "  -v, --verbose            Enable verbose output"
    echo "  -b, --backup             Create backup of files before modification"
    echo "  -s, --strict             Use stricter validation for include paths"
    echo "  -r, --report-dir DIR     Directory to store reports (default: ./reports/includes)"
    echo "  -h, --help               Display this help message"
    echo ""
    exit 1
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case "$1" in
        -p|--project-root)
            PROJECT_ROOT="$2"
            shift 2
            ;;
        -d|--dry-run)
            DRY_RUN="--dry-run"
            shift
            ;;
        -v|--verbose)
            VERBOSE="--verbose"
            shift
            ;;
        -b|--backup)
            BACKUP="--backup"
            shift
            ;;
        -s|--strict)
            STRICT="--strict"
            shift
            ;;
        -r|--report-dir)
            REPORT_DIR="$2"
            shift 2
            ;;
        -h|--help)
            HELP=1
            shift
            ;;
        *)
            echo "Unknown option: $1"
            print_usage
            ;;
    esac
done

# Show help if requested
if [ "$HELP" -eq 1 ]; then
    print_usage
fi

# Validate project directory
if [ ! -d "${PROJECT_ROOT}" ]; then
    echo "Error: Project root directory not found: ${PROJECT_ROOT}"
    exit 1
fi

# Make sure include and src directories exist
if [ ! -d "${PROJECT_ROOT}/include" ] || [ ! -d "${PROJECT_ROOT}/src" ]; then
    echo "Error: Required directories not found. Make sure 'include' and 'src' exist in the project root."
    exit 1
fi

# Create report directory
mkdir -p "${REPORT_DIR}"

# Generate timestamp for report files
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
INVALID_REPORT="${REPORT_DIR}/invalid_includes_${TIMESTAMP}.md"
STANDARDIZATION_REPORT="${REPORT_DIR}/standardized_includes_${TIMESTAMP}.md"
FINAL_REPORT="${REPORT_DIR}/include_summary_${TIMESTAMP}.md"

# Check for Python interpreter
if ! command -v python3 &> /dev/null; then
    echo "Error: Python 3 is required but not found in PATH"
    exit 1
fi

# Get the directory where this script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo "=== NexusLink Include Path Cleanup and Standardization ==="
echo "Project root: ${PROJECT_ROOT}"
echo "Report directory: ${REPORT_DIR}"
echo "Script directory: ${SCRIPT_DIR}"
echo ""

# Step 1: Remove invalid includes
echo "Step 1/2: Removing invalid include paths..."
python3 "${SCRIPT_DIR}/remove_invalid_includes.py" \
    --project-root "${PROJECT_ROOT}" \
    ${DRY_RUN} \
    ${VERBOSE} \
    ${STRICT} \
    --report-file "${INVALID_REPORT}"

# Capture status
INVALID_STATUS=$?
echo ""

# Step 2: Standardize include paths
echo "Step 2/2: Standardizing include paths..."
python3 "${SCRIPT_DIR}/standardize_nlink_includes.py" \
    --project-root "${PROJECT_ROOT}" \
    ${DRY_RUN} \
    ${VERBOSE} \
    ${BACKUP} \
    --report-file "${STANDARDIZATION_REPORT}"

# Capture status
STANDARDIZE_STATUS=$?
echo ""

# Generate final report
echo "Generating final summary report..."
cat > "${FINAL_REPORT}" << EOF
# NexusLink Include Path Standardization Summary

Date: $(date)
Project: ${PROJECT_ROOT}

## Process Summary

1. **Invalid Include Removal**: ${INVALID_STATUS}
   - See detailed report: [Invalid Includes]($(basename "${INVALID_REPORT}"))

2. **Include Path Standardization**: ${STANDARDIZE_STATUS}
   - See detailed report: [Standardized Includes]($(basename "${STANDARDIZATION_REPORT}"))

## Next Steps

1. Verify that the codebase builds successfully after these changes
2. Run the unit tests to ensure functionality hasn't been broken
3. Review circular dependency warnings in the compiler output

## Notes

This standardization follows the NexusLink architecture design, specifically
addressing the component structure required for proper separation of concerns.
The hierarchy established is:

- \`nlink/core/common\` - Common utilities and forward declarations
- \`nlink/core/symbols\` - Symbol registry and versioning
- \`nlink/core/minimizer\` - Okpala state machine minimizer
- \`nlink/core/versioning\` - Semantic versioning system
- \`nlink/core/metadata\` - Component metadata
- \`nlink/cli\` - Command-line interface

This structure should prevent circular dependencies and establish a clear
dependency hierarchy.
EOF

echo "Process completed"
echo "Summary report: ${FINAL_REPORT}"

# Determine overall success
if [ "$INVALID_STATUS" -eq 0 ] && [ "$STANDARDIZE_STATUS" -eq 0 ]; then
    echo "All include path fixes completed successfully"
    exit 0
else
    echo "Include path fixes completed with issues, check the reports for details"
    exit 1
fi