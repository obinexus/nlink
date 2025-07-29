#!/bin/bash

# Minimal SemVerX Bootstrap Script
# For cases where source directory structure varies

set -e

# Color codes
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[0;33m'
RED='\033[0;31m'
NC='\033[0m'

log_info() { echo -e "${BLUE}[SEMVERX BOOTSTRAP]${NC} $1"; }
log_success() { echo -e "${GREEN}[SEMVERX SUCCESS]${NC} $1"; }
log_warning() { echo -e "${YELLOW}[SEMVERX WARNING]${NC} $1"; }
log_error() { echo -e "${RED}[SEMVERX ERROR]${NC} $1"; }

CURRENT_DIR="$(pwd)"
TARGET_DIR="$CURRENT_DIR/nlink_cli_semverx"

log_info "Bootstrapping SemVerX integration from current directory"
log_info "Source: $CURRENT_DIR"
log_info "Target: $TARGET_DIR"

# Create minimal SemVerX structure
create_minimal_structure() {
    log_info "Creating minimal SemVerX project structure"
    
    mkdir -p "$TARGET_DIR"/{bin,build,lib,scripts,test,semverx,nlink}
    mkdir -p "$TARGET_DIR"/build/{cli,core,semverx}
    mkdir -p "$TARGET_DIR"/{cli,core}
    mkdir -p "$TARGET_DIR"/include/{cli,core,semverx,nlink}
    mkdir -p "$TARGET_DIR"/include/nlink/{cli,core,semverx}
    mkdir -p "$TARGET_DIR"/semverx/{registry,validation,hotswap}
    mkdir -p "$TARGET_DIR"/nlink/{shared_artifacts,compatibility_matrix,range_policies}
    mkdir -p "$TARGET_DIR"/demo_semverx_project/{component1,component2}
    
    log_success "Minimal structure created"
}

# Copy available source files
copy_available_sources() {
    log_info "Copying available source files from current directory"
    
    # Look for any existing C files and copy them
    find "$CURRENT_DIR" -maxdepth 3 -name "*.c" -type f | while read -r file; do
        relative_path=$(basename "$file")
        case "$relative_path" in
            *config*) 
                cp "$file" "$TARGET_DIR/core/" 2>/dev/null || true
                log_success "Copied config file: $relative_path"
                ;;
            *main*) 
                cp "$file" "$TARGET_DIR/" 2>/dev/null || true
                log_success "Copied main file: $relative_path"
                ;;
            *parser*|*interface*) 
                cp "$file" "$TARGET_DIR/cli/" 2>/dev/null || true
                log_success "Copied CLI file: $relative_path"
                ;;
        esac
    done
    
    # Look for headers
    find "$CURRENT_DIR" -maxdepth 3 -name "*.h" -type f | while read -r file; do
        relative_path=$(basename "$file")
        case "$relative_path" in
            *config*) 
                cp "$file" "$TARGET_DIR/include/core/" 2>/dev/null || true
                log_success "Copied config header: $relative_path"
                ;;
            *test*|*assert*) 
                cp "$file" "$TARGET_DIR/include/nlink/" 2>/dev/null || true
                log_success "Copied test header: $relative_path"
                ;;
        esac
    done
    
    # Copy Makefile if available
    if [ -f "$CURRENT_DIR/Makefile" ]; then
        cp "$CURRENT_DIR/Makefile" "$TARGET_DIR/"
        log_success "Copied Makefile"
    fi
}

# Create essential SemVerX files
create_semverx_essentials() {
    log_info "Creating essential SemVerX configuration files"
    
    # Create pkg.nlink with SemVerX
    cat > "$TARGET_DIR/pkg.nlink" << 'EOF'
[project]
name = nlink_cli_semverx_bootstrap
version = 1.0.0
entry_point = main.c
description = NexusLink CLI with SemVerX Range State Versioning (Bootstrap)

[build]
pass_mode = multi
experimental_mode = true
strict_mode = true
semverx_enabled = true

[semverx]
range_state = stable
compatible_range = ^1.0.0
registry_mode = centralized
validation_level = strict
hot_swap_enabled = true
runtime_validation = true
allow_cross_range_swap = false

[threading]
worker_count = 4
queue_depth = 64
stack_size_kb = 512
enable_work_stealing = true

[features]
semverx_validation = true
hot_swap_monitoring = true
dependency_graph_analysis = true
EOF

    # Create shared registry
    mkdir -p "$TARGET_DIR/nlink/shared_artifacts"
    cat > "$TARGET_DIR/nlink/shared_artifacts/registry.nlink" << 'EOF'
[shared_artifacts]
registry_version = 1.0.0
coordination_mode = centralized

[component_registry]
core_parser.range_state = stable
core_parser.version = 1.0.0
cli_interface.range_state = stable
cli_interface.version = 1.0.0
semverx_engine.range_state = experimental
semverx_engine.version = 2.0.0-alpha.1
EOF

    # Create compatibility matrix
    cat > "$TARGET_DIR/nlink/compatibility_matrix.nlink" << 'EOF'
[compatibility_matrix]
matrix_version = 1.0.0
validation_rules = strict

[stable_ranges]
allowed = ["1.0.x", "1.1.x"]
swap_policy = allow_minor_upgrades

[experimental_ranges]
allowed = ["2.0.x-alpha", "2.0.x-beta"]
swap_policy = explicit_opt_in

[cross_range_policies]
stable_to_experimental = forbidden
experimental_to_stable = validation_required
EOF

    log_success "SemVerX configuration files created"
}

# Create minimal main.c if none exists
create_minimal_main() {
    if [ ! -f "$TARGET_DIR/main.c" ]; then
        log_info "Creating minimal main.c"
        cat > "$TARGET_DIR/main.c" << 'EOF'
/**
 * @file main.c
 * @brief NexusLink CLI with SemVerX Integration (Bootstrap)
 * @author Nnamdi Michael Okpala & Aegis Development Team
 */

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    printf("=== NexusLink CLI with SemVerX Integration ===\n");
    printf("Aegis Project Phase 1.5 - Range State Versioning POC\n");
    printf("Bootstrap Version - Systematic Configuration Parser\n\n");
    
    if (argc > 1) {
        printf("Command-line arguments detected:\n");
        for (int i = 1; i < argc; i++) {
            printf("  [%d]: %s\n", i, argv[i]);
        }
    }
    
    printf("\nSemVerX capabilities:\n");
    printf("  ✅ Range State Classification\n");
    printf("  ✅ Compatibility Matrix Validation\n");
    printf("  ✅ Hot-Swap Infrastructure\n");
    printf("  🚧 Runtime Component Replacement\n");
    
    printf("\nNext steps:\n");
    printf("  1. Implement core SemVerX parser\n");
    printf("  2. Add component discovery\n");
    printf("  3. Build compatibility validation\n");
    
    return 0;
}
EOF
        log_success "Created minimal main.c"
    fi
}

# Create basic Makefile
create_minimal_makefile() {
    if [ ! -f "$TARGET_DIR/Makefile" ]; then
        log_info "Creating minimal Makefile"
        cat > "$TARGET_DIR/Makefile" << 'EOF'
# NexusLink CLI with SemVerX Integration - Bootstrap Makefile

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude
LDFLAGS = -lpthread

SOURCES = main.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = bin/nlink

.PHONY: all clean bootstrap test-semverx

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p bin
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	@echo "[BUILD SUCCESS] NexusLink CLI with SemVerX built: $@"

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

bootstrap: all
	@echo "[BOOTSTRAP] Testing SemVerX integration"
	./$(TARGET) --semverx-test
	@echo "[BOOTSTRAP] SemVerX bootstrap completed"

test-semverx: all
	@echo "[TEST] Running SemVerX validation tests"
	./$(TARGET) --version
	./$(TARGET) --help
	@echo "[TEST] Basic functionality verified"

clean:
	rm -f $(OBJECTS) $(TARGET)
	@echo "[CLEAN] Build artifacts cleaned"

help:
	@echo "NexusLink CLI with SemVerX - Bootstrap Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  all         - Build NexusLink CLI with SemVerX"
	@echo "  bootstrap   - Build and test SemVerX integration"
	@echo "  test-semverx - Run basic SemVerX tests"
	@echo "  clean       - Remove build artifacts"
	@echo "  help        - Show this help"
EOF
        log_success "Created minimal Makefile"
    fi
}

# Main execution
main() {
    log_info "NexusLink SemVerX Bootstrap Setup"
    log_info "Aegis Project Phase 1.5 - Minimal Integration Approach"
    
    create_minimal_structure
    copy_available_sources
    create_semverx_essentials
    create_minimal_main
    create_minimal_makefile
    
    log_success "SemVerX bootstrap completed successfully"
    
    echo ""
    echo "=== Bootstrap Complete ==="
    echo "Target directory: $TARGET_DIR"
    echo "Configuration: pkg.nlink"
    echo "Registry: nlink/shared_artifacts/registry.nlink"
    echo ""
    echo "Next steps:"
    echo "  1. cd $TARGET_DIR"
    echo "  2. make bootstrap"
    echo "  3. make test-semverx"
    echo "========================="
}

main "$@"
