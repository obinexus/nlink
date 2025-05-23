#!/bin/bash
# compile_with_nlink.sh
# Compilation script for projects using the NexusLink (NLink) framework
# Author: Based on Nnamdi Michael Okpala's NexusLink architecture

set -e  # Exit on error

# Configuration variables
NLINK_ROOT="/usr/local"  # Default installation location, override with -n flag
OUTPUT_NAME="app"        # Default output name, override with -o flag
DEBUG=0                  # Debug mode flag
SOURCE_FILES=""          # Source files to compile
INCLUDE_DIRS=""          # Additional include directories
BUILD_DIR="build"        # Build directory
USE_MINIMIZER=1          # Whether to use the minimizer component
USE_AUTOMATON=1          # Whether to use the automaton component
USE_VERSIONING=1         # Whether to use the versioning component
USE_METRICS=0            # Whether to use the metrics component (if available)

# Function to print usage
print_usage() {
    echo "Usage: $0 -s source1.c [source2.c ...] [OPTIONS]"
    echo "Options:"
    echo "  -s, --source FILES    Source files to compile (required)"
    echo "  -n, --nlink-root DIR  NexusLink installation root (default: /usr/local)"
    echo "  -o, --output NAME     Output executable name (default: app)"
    echo "  -i, --include DIR     Additional include directories (can be used multiple times)"
    echo "  -b, --build-dir DIR   Build directory (default: build)"
    echo "  -d, --debug           Enable debug mode"
    echo "  --no-minimizer        Don't link against minimizer component"
    echo "  --no-automaton        Don't link against automaton component"
    echo "  --no-versioning       Don't link against versioning component"
    echo "  --use-metrics         Link against metrics component (if available)"
    echo "  -h, --help            Show this help message"
    exit 1
}

# Parse arguments
while [[ $# -gt 0 ]]; do
    case "$1" in
        -s|--source)
            shift
            while [[ $# -gt 0 && ! "$1" =~ ^- ]]; do
                SOURCE_FILES="$SOURCE_FILES $1"
                shift
            done
            ;;
        -n|--nlink-root)
            NLINK_ROOT="$2"
            shift 2
            ;;
        -o|--output)
            OUTPUT_NAME="$2"
            shift 2
            ;;
        -i|--include)
            INCLUDE_DIRS="$INCLUDE_DIRS -I$2"
            shift 2
            ;;
        -b|--build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        -d|--debug)
            DEBUG=1
            shift
            ;;
        --no-minimizer)
            USE_MINIMIZER=0
            shift
            ;;
        --no-automaton)
            USE_AUTOMATON=0
            shift
            ;;
        --no-versioning)
            USE_VERSIONING=0
            shift
            ;;
        --use-metrics)
            USE_METRICS=1
            shift
            ;;
        -h|--help)
            print_usage
            ;;
        *)
            echo "Unknown option: $1"
            print_usage
            ;;
    esac
done

# Check for required arguments
if [ -z "$SOURCE_FILES" ]; then
    echo "Error: No source files specified"
    print_usage
fi

# Set up compiler flags
CC="gcc"
CFLAGS="-Wall -Wextra"
LDFLAGS=""

# Add debug/release flags
if [ "$DEBUG" -eq 1 ]; then
    CFLAGS="$CFLAGS -g -DNEXUS_DEBUG"
else
    CFLAGS="$CFLAGS -O2 -DNDEBUG"
fi

# Set up NexusLink include and library paths
NLINK_INCLUDE="$NLINK_ROOT/include"
NLINK_LIB="$NLINK_ROOT/lib/nexuslink"

# Verify NexusLink installation
if [ ! -d "$NLINK_INCLUDE/nexuslink" ]; then
    echo "Error: NexusLink installation not found at $NLINK_ROOT"
    echo "Use -n option to specify NexusLink installation root"
    exit 1
fi

# Create build directory
mkdir -p "$BUILD_DIR"

# Determine object files
OBJ_FILES=""
for src in $SOURCE_FILES; do
    base_name=$(basename "$src" .c)
    OBJ_FILES="$OBJ_FILES $BUILD_DIR/${base_name}.o"
done

# Build library flags list
LIB_FLAGS="-lnexus_core -lnexus_common -lnexus_symbols"

if [ "$USE_MINIMIZER" -eq 1 ]; then
    LIB_FLAGS="$LIB_FLAGS -lnexus_minimizer"
fi

if [ "$USE_AUTOMATON" -eq 1 ]; then
    LIB_FLAGS="$LIB_FLAGS -lnexus_automaton"
fi

if [ "$USE_VERSIONING" -eq 1 ]; then
    LIB_FLAGS="$LIB_FLAGS -lnexus_versioning"
fi

if [ "$USE_METRICS" -eq 1 ]; then
    # Check if metrics library exists
    if [ -f "$NLINK_LIB/libnexus_metrics.so" ]; then
        LIB_FLAGS="$LIB_FLAGS -lnexus_metrics"
    else
        echo "Warning: Metrics library not found, continuing without it"
    fi
fi

# Add full library flags
LDFLAGS="$LDFLAGS -L$NLINK_LIB $LIB_FLAGS -ldl -lpthread -Wl,-rpath,$NLINK_LIB"

# Display build information
echo "Building with NexusLink from $NLINK_ROOT"
echo "Source files: $SOURCE_FILES"
echo "Output: $OUTPUT_NAME"
echo "Debug mode: $([ "$DEBUG" -eq 1 ] && echo "enabled" || echo "disabled")"
echo "Components: symbol registry"
[ "$USE_VERSIONING" -eq 1 ] && echo "           versioning"
[ "$USE_MINIMIZER" -eq 1 ] && echo "           minimizer"
[ "$USE_AUTOMATON" -eq 1 ] && echo "           automaton"
[ "$USE_METRICS" -eq 1 ] && echo "           metrics"

# Compile source files
for src in $SOURCE_FILES; do
    base_name=$(basename "$src" .c)
    obj_file="$BUILD_DIR/${base_name}.o"
    
    echo "Compiling $src -> $obj_file"
    $CC $CFLAGS -I"$NLINK_INCLUDE" $INCLUDE_DIRS -c "$src" -o "$obj_file"
done

# Link executable
echo "Linking $OUTPUT_NAME"
$CC -o "$OUTPUT_NAME" $OBJ_FILES $LDFLAGS

echo "Build completed successfully: ./$OUTPUT_NAME"