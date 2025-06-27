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
