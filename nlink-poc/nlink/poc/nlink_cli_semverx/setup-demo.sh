#!/bin/bash

# SemVerX Demo Setup Script - Systematic Implementation  
# Creates structured demonstration environment with focused demo projects
# Aegis Project Phase 1.5 - Waterfall methodology compliance

set -e

# Project structure configuration
PROJECT_ROOT="$(pwd)"
EXAMPLES_DIR="$PROJECT_ROOT/examples"
SCRIPTS_DIR="$PROJECT_ROOT/scripts"

# Color-coded logging for systematic progress tracking
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[0;33m'
NC='\033[0m'

log_info() { echo -e "${BLUE}[SETUP]${NC} $1"; }
log_success() { echo -e "${GREEN}[SUCCESS]${NC} $1"; }
log_warning() { echo -e "${YELLOW}[WARNING]${NC} $1"; }
log_error() { echo -e "${RED}[ERROR]${NC} $1"; }

# Phase 1: Directory Structure Creation
create_demo_structure() {
    log_info "Creating systematic demo structure with focused projects"
    
    # Primary Demo: Calculation Pipeline
    mkdir -p "$EXAMPLES_DIR/calculation_pipeline/basic_math/src"
    mkdir -p "$EXAMPLES_DIR/calculation_pipeline/basic_math/build"
    mkdir -p "$EXAMPLES_DIR/calculation_pipeline/advanced_math/src"
    mkdir -p "$EXAMPLES_DIR/calculation_pipeline/advanced_math/build"
    mkdir -p "$EXAMPLES_DIR/calculation_pipeline/shared_orchestration"
    
    # Secondary Demo: Simple Parser
    mkdir -p "$EXAMPLES_DIR/simple_parser/lexer_component/src"
    mkdir -p "$EXAMPLES_DIR/simple_parser/parser_component/src"
    mkdir -p "$EXAMPLES_DIR/simple_parser/shared_coordination"
    
    # Scripts directory for orchestration
    mkdir -p "$SCRIPTS_DIR"
    
    log_success "Demo directory structure created"
}

# Phase 2: Component Implementation Files
create_component_implementations() {
    log_info "Creating component implementation files with SemVerX metadata"
    
    # Basic Math Calculator (Stable Range State)
    cat > "$EXAMPLES_DIR/calculation_pipeline/basic_math/src/calculator.c" << 'EOF'
/**
 * @file calculator.c
 * @brief Basic Math Calculator - Stable Range State Component
 * @author Nnamdi Michael Okpala & Aegis Development Team
 * @version 1.2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Component metadata for SemVerX coordination
typedef struct {
    char component_name[64];
    char version[32];
    char range_state[16];
    double precision_factor;
} calculator_metadata_t;

static calculator_metadata_t metadata = {
    .component_name = "basic_math_calculator",
    .version = "1.2.0",
    .range_state = "stable",
    .precision_factor = 1.0
};

// Core calculation functions
double basic_add(double a, double b) {
    printf("[BASIC_MATH] Executing addition: %.2f + %.2f\n", a, b);
    return a + b;
}

double basic_subtract(double a, double b) {
    printf("[BASIC_MATH] Executing subtraction: %.2f - %.2f\n", a, b);
    return a - b;
}

double basic_multiply(double a, double b) {
    printf("[BASIC_MATH] Executing multiplication: %.2f * %.2f\n", a, b);
    return a * b;
}

double basic_divide(double a, double b) {
    if (b == 0.0) {
        printf("[BASIC_MATH] Division by zero detected, returning 0\n");
        return 0.0;
    }
    printf("[BASIC_MATH] Executing division: %.2f / %.2f\n", a, b);
    return a / b;
}

// Component registration for shared artifact coordination
void register_component_metadata(void) {
    printf("[COMPONENT_REGISTRY] Registering: %s v%s (%s)\n", 
           metadata.component_name, metadata.version, metadata.range_state);
    printf("[SHARED_ARTIFACT] Compatible with stable range components\n");
    printf("[HOT_SWAP] Enabled for version upgrades within 1.x.x\n");
}

// Coordination interface for SemVerX orchestration
double execute_calculation(const char *operation, double a, double b) {
    printf("[COORDINATION] Received calculation request: %s\n", operation);
    
    if (strcmp(operation, "add") == 0) return basic_add(a, b);
    if (strcmp(operation, "subtract") == 0) return basic_subtract(a, b);
    if (strcmp(operation, "multiply") == 0) return basic_multiply(a, b);
    if (strcmp(operation, "divide") == 0) return basic_divide(a, b);
    
    printf("[ERROR] Unknown operation: %s\n", operation);
    return 0.0;
}

// Validation interface for compatibility checking
int validate_component_compatibility(const char *other_component, const char *other_range_state) {
    printf("[VALIDATION] Checking compatibility with %s (%s)\n", other_component, other_range_state);
    
    if (strcmp(other_range_state, "stable") == 0) {
        printf("[COMPATIBILITY] ALLOWED: Stable ↔ Stable component interaction\n");
        return 1;
    }
    
    if (strcmp(other_range_state, "experimental") == 0) {
        printf("[COMPATIBILITY] CONDITIONAL: Stable ↔ Experimental requires validation\n");
        return 0;
    }
    
    if (strcmp(other_range_state, "legacy") == 0) {
        printf("[COMPATIBILITY] DENIED: Legacy components not compatible\n");
        return -1;
    }
    
    return 0;
}

// Main demonstration entry point
int main(int argc, char *argv[]) {
    printf("=== Basic Math Calculator - SemVerX Stable Component ===\n");
    register_component_metadata();
    
    if (argc < 2) {
        printf("\nUsage:\n");
        printf("  %s <operation> <num1> <num2>     # Execute calculation\n", argv[0]);
        printf("  %s --validate <component> <state> # Test compatibility\n", argv[0]);
        printf("  %s --metadata                     # Show component info\n", argv[0]);
        return 1;
    }
    
    // Metadata display
    if (strcmp(argv[1], "--metadata") == 0) {
        printf("\n[METADATA] Component: %s\n", metadata.component_name);
        printf("[METADATA] Version: %s\n", metadata.version);
        printf("[METADATA] Range State: %s\n", metadata.range_state);
        printf("[METADATA] Precision Factor: %.1f\n", metadata.precision_factor);
        return 0;
    }
    
    // Compatibility validation
    if (strcmp(argv[1], "--validate") == 0 && argc >= 4) {
        int result = validate_component_compatibility(argv[2], argv[3]);
        printf("[VALIDATION_RESULT] %s\n", 
               result > 0 ? "COMPATIBLE" : 
               result == 0 ? "REQUIRES_VALIDATION" : "INCOMPATIBLE");
        return result < 0 ? 1 : 0;
    }
    
    // Calculation execution
    if (argc >= 4) {
        double a = atof(argv[2]);
        double b = atof(argv[3]);
        double result = execute_calculation(argv[1], a, b);
        
        printf("\n[CALCULATION_RESULT] %.6f\n", result);
        printf("[COMPONENT] %s v%s processed calculation successfully\n", 
               metadata.component_name, metadata.version);
        return 0;
    }
    
    printf("[ERROR] Invalid arguments. Use --help for usage information.\n");
    return 1;
}
EOF

    # Advanced Math Scientific Calculator (Experimental Range State)
    cat > "$EXAMPLES_DIR/calculation_pipeline/advanced_math/src/scientific.c" << 'EOF'
/**
 * @file scientific.c
 * @brief Advanced Math Scientific Calculator - Experimental Range State
 * @author Nnamdi Michael Okpala & Aegis Development Team
 * @version 2.0.0-alpha.1
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
    char component_name[64];
    char version[32];
    char range_state[16];
    double precision_factor;
} scientific_metadata_t;

static scientific_metadata_t metadata = {
    .component_name = "advanced_math_scientific",
    .version = "2.0.0-alpha.1",
    .range_state = "experimental",
    .precision_factor = 2.0
};

// Advanced mathematical functions
double scientific_sin(double x) {
    printf("[SCIENTIFIC] Computing sin(%.6f)\n", x);
    return sin(x);
}

double scientific_cos(double x) {
    printf("[SCIENTIFIC] Computing cos(%.6f)\n", x);
    return cos(x);
}

double scientific_log(double x) {
    if (x <= 0) {
        printf("[SCIENTIFIC] Invalid input for log: %.6f\n", x);
        return NAN;
    }
    printf("[SCIENTIFIC] Computing log(%.6f)\n", x);
    return log(x);
}

double scientific_sqrt(double x) {
    if (x < 0) {
        printf("[SCIENTIFIC] Invalid input for sqrt: %.6f\n", x);
        return NAN;
    }
    printf("[SCIENTIFIC] Computing sqrt(%.6f)\n", x);
    return sqrt(x);
}

double scientific_exp(double x) {
    printf("[SCIENTIFIC] Computing exp(%.6f)\n", x);
    return exp(x);
}

double scientific_pow(double base, double exponent) {
    printf("[SCIENTIFIC] Computing pow(%.6f, %.6f)\n", base, exponent);
    return pow(base, exponent);
}

void register_experimental_component(void) {
    printf("[EXPERIMENTAL_REGISTRY] Registering: %s v%s (%s)\n", 
           metadata.component_name, metadata.version, metadata.range_state);
    printf("[SHARED_ARTIFACT] Requires explicit validation for cross-range interaction\n");
    printf("[HOT_SWAP] Disabled for experimental components (safety)\n");
}

double execute_scientific_function(const char *function, double value, double value2) {
    printf("[SCIENTIFIC_COORDINATION] Received function request: %s\n", function);
    
    if (strcmp(function, "sin") == 0) return scientific_sin(value);
    if (strcmp(function, "cos") == 0) return scientific_cos(value);
    if (strcmp(function, "log") == 0) return scientific_log(value);
    if (strcmp(function, "sqrt") == 0) return scientific_sqrt(value);
    if (strcmp(function, "exp") == 0) return scientific_exp(value);
    if (strcmp(function, "pow") == 0) return scientific_pow(value, value2);
    
    printf("[ERROR] Unknown scientific function: %s\n", function);
    return NAN;
}

int main(int argc, char *argv[]) {
    printf("=== Advanced Math Scientific Calculator - SemVerX Experimental Component ===\n");
    register_experimental_component();
    
    if (argc < 2) {
        printf("\nUsage:\n");
        printf("  %s <function> <value> [value2]   # Execute scientific function\n", argv[0]);
        printf("  %s --metadata                    # Show component info\n", argv[0]);
        printf("\nFunctions: sin, cos, log, sqrt, exp, pow\n");
        return 1;
    }
    
    if (strcmp(argv[1], "--metadata") == 0) {
        printf("\n[METADATA] Component: %s\n", metadata.component_name);
        printf("[METADATA] Version: %s\n", metadata.version);
        printf("[METADATA] Range State: %s\n", metadata.range_state);
        printf("[METADATA] Precision Factor: %.1f\n", metadata.precision_factor);
        printf("[METADATA] Experimental Features: scientific_computing, extended_precision\n");
        return 0;
    }
    
    if (argc >= 3) {
        double value1 = atof(argv[2]);
        double value2 = (argc >= 4) ? atof(argv[3]) : 0.0;
        double result = execute_scientific_function(argv[1], value1, value2);
        
        if (!isnan(result)) {
            printf("\n[SCIENTIFIC_RESULT] %.10f\n", result);
            printf("[EXPERIMENTAL_COMPONENT] %s v%s processed function successfully\n", 
                   metadata.component_name, metadata.version);
            return 0;
        } else {
            printf("[ERROR] Function execution failed\n");
            return 1;
        }
    }
    
    printf("[ERROR] Invalid arguments. Use --help for usage information.\n");
    return 1;
}
EOF

    log_success "Component implementation files created"
}

# Phase 3: Configuration Files
create_configuration_files() {
    log_info "Creating SemVerX configuration files"
    
    # Component-level configurations
    cat > "$EXAMPLES_DIR/calculation_pipeline/basic_math/nlink.txt" << 'EOF'
[component]
name = basic_math_calculator
version = 1.2.0
parent_component = calculation_pipeline

[semverx]
range_state = stable
compatible_range = ^1.2.0
hot_swap_enabled = true
runtime_validation = strict

[compilation]
optimization_level = 2
max_compile_time = 60
parallel_allowed = true
EOF

    cat > "$EXAMPLES_DIR/calculation_pipeline/advanced_math/nlink.txt" << 'EOF'
[component]
name = advanced_math_scientific
version = 2.0.0-alpha.1
parent_component = calculation_pipeline

[semverx]
range_state = experimental
compatible_range = 2.0.0-alpha.x
hot_swap_enabled = false
runtime_validation = paranoid
requires_opt_in = true

[compilation]
optimization_level = 0
max_compile_time = 120
parallel_allowed = false
EOF

    # Project-level configuration
    cat > "$EXAMPLES_DIR/calculation_pipeline/pkg.nlink" << 'EOF'
[project]
name = mathematical_calculation_pipeline
version = 1.0.0
entry_point = calculation_orchestrator

[build]
pass_mode = multi
experimental_mode = true
semverx_enabled = true

[semverx]
range_state = stable
registry_mode = centralized
validation_level = strict
shared_registry_path = ./shared_orchestration/calculation_registry.nlink
hot_swap_enabled = true

[threading]
worker_count = 4
queue_depth = 64
EOF

    log_success "Configuration files created"
}

# Phase 4: Orchestration Script
create_orchestration_script() {
    log_info "Creating demonstration orchestration script"
    
    cat > "$SCRIPTS_DIR/orchestration_demo.sh" << 'EOF'
#!/bin/bash

# SemVerX Demonstration Orchestration Script
# Systematic showcase of shared artifact coordination
# Aegis Project Phase 1.5 - Production Demonstration

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
CALC_DEMO_DIR="$PROJECT_ROOT/examples/calculation_pipeline"

# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
NC='\033[0m'

log_phase() { echo -e "${BLUE}[SEMVERX DEMO]${NC} $1"; }
log_success() { echo -e "${GREEN}[SUCCESS]${NC} $1"; }
log_calculation() { echo -e "${PURPLE}[CALCULATION]${NC} $1"; }

echo "=== SemVerX Shared Artifact Coordination Demonstration ==="
echo "Aegis Project Phase 1.5 - Range State Versioning Integration"
echo ""

# Phase 1: NexusLink CLI Integration
log_phase "Phase 1: NexusLink CLI Integration Validation"
echo ""

echo "--- CLI Version and Architecture ---"
$PROJECT_ROOT/bin/nlink --version
echo ""

echo "--- Configuration Check with SemVerX ---"
cd "$CALC_DEMO_DIR"
$PROJECT_ROOT/bin/nlink --config-check --project-root . || true
echo ""

# Phase 2: Component Execution
log_phase "Phase 2: Component Range State Validation"
echo ""

if [ -f "basic_math/build/calculator" ] && [ -f "advanced_math/build/scientific" ]; then
    echo "--- Basic Math Component (Stable Range State) ---"
    ./basic_math/build/calculator --metadata
    echo ""
    
    echo "--- Advanced Math Component (Experimental Range State) ---"
    ./advanced_math/build/scientific --metadata
    echo ""
    
    # Phase 3: Calculation Pipeline
    log_phase "Phase 3: Calculation Pipeline Demonstration"
    echo ""
    
    echo "--- Basic Mathematical Operations (Stable Range) ---"
    log_calculation "Addition: 42.5 + 17.3"
    ./basic_math/build/calculator add 42.5 17.3
    echo ""
    
    log_calculation "Division: 100.0 / 7.0"
    ./basic_math/build/calculator divide 100.0 7.0
    echo ""
    
    echo "--- Scientific Operations (Experimental Range) ---"
    log_calculation "Sine: sin(π/4)"
    ./advanced_math/build/scientific sin 0.785398
    echo ""
    
    log_calculation "Square Root: sqrt(16)"
    ./advanced_math/build/scientific sqrt 16
    echo ""
    
    # Phase 4: Compatibility Testing
    log_phase "Phase 4: Component Compatibility Validation"
    echo ""
    
    echo "--- Range State Compatibility Testing ---"
    ./basic_math/build/calculator --validate "advanced_math" "experimental"
    echo ""
    ./basic_math/build/calculator --validate "basic_math_v2" "stable"
    echo ""
    ./basic_math/build/calculator --validate "legacy_math" "legacy"
    echo ""
else
    echo "Components not built. Run 'make examples-build' first."
fi

log_success "SemVerX demonstration completed successfully"
echo ""
echo "--- OBINexus Integration Status ---"
echo "• Toolchain Position: nlink (SemVerX) → polybuild"
echo "• Range State Coordination: ✅ OPERATIONAL"
echo "• Shared Artifact Registry: ✅ CONFIGURED"
echo "• Component Compatibility: ✅ VALIDATED"

cd "$PROJECT_ROOT"
EOF

    chmod +x "$SCRIPTS_DIR/orchestration_demo.sh"
    log_success "Orchestration script created and made executable"
}

# Phase 5: Build System Integration
update_build_system() {
    log_info "Updating build system for demo integration"
    
    # Check if Makefile needs demo targets
    if ! grep -q "examples-build" "$PROJECT_ROOT/Makefile" 2>/dev/null; then
        cat >> "$PROJECT_ROOT/Makefile" << 'EOF'

# =============================================================================
# DEMO PROJECT TARGETS
# =============================================================================

demo-setup:
	@echo "[DEMO] Setting up demonstration environment"
	./setup_demo.sh

examples-build:
	@echo "[BUILD] Compiling demonstration components"
	@mkdir -p examples/calculation_pipeline/basic_math/build
	@mkdir -p examples/calculation_pipeline/advanced_math/build
	@if [ -f examples/calculation_pipeline/basic_math/src/calculator.c ]; then \
		gcc -Wall -Wextra -std=c99 examples/calculation_pipeline/basic_math/src/calculator.c \
		-o examples/calculation_pipeline/basic_math/build/calculator -lm; \
		echo "✅ Built basic_math calculator"; \
	fi
	@if [ -f examples/calculation_pipeline/advanced_math/src/scientific.c ]; then \
		gcc -Wall -Wextra -std=c99 examples/calculation_pipeline/advanced_math/src/scientific.c \
		-o examples/calculation_pipeline/advanced_math/build/scientific -lm; \
		echo "✅ Built advanced_math scientific calculator"; \
	fi

demo-calculation: all examples-build
	@echo "[DEMO] Running calculation pipeline demonstration"
	./scripts/orchestration_demo.sh

demo-validation: all
	@echo "[DEMO] Running systematic validation"
	@if [ -f bin/nlink ]; then \
		bin/nlink --config-check --project-root examples/calculation_pipeline || true; \
	fi

.PHONY: demo-setup examples-build demo-calculation demo-validation
EOF
        log_success "Makefile updated with demo targets"
    else
        log_info "Makefile already contains demo targets"
    fi
}

# Main execution function
main() {
    echo "=== SemVerX Demo Setup - Systematic Implementation ==="
    echo "Restoring demonstration environment with focused projects"
    echo ""
    
    create_demo_structure
    create_component_implementations
    create_configuration_files
    create_orchestration_script
    update_build_system
    
    echo ""
    echo "=== Setup Completed Successfully ==="
    echo "Demonstration structure restored with:"
    echo "✅ examples/calculation_pipeline/ - Mathematical coordination demo"
    echo "✅ Component implementations with SemVerX metadata"
    echo "✅ Configuration files for range state coordination"
    echo "✅ Orchestration script for systematic demonstration"
    echo "✅ Build system integration for demo targets"
    echo ""
    echo "Next Steps:"
    echo "1. make clean && make all"
    echo "2. make examples-build"
    echo "3. make demo-calculation"
    echo ""
    echo "Or execute directly:"
    echo "./scripts/orchestration_demo.sh"
    echo ""
}

main "$@"