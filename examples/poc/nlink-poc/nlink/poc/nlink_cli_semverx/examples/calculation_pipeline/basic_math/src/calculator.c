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
