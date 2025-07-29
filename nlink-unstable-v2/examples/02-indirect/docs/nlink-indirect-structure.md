# NLink-Indirect: Consciousness-Preserving Component Linker
## Repository Structure for github.com/obinexus/nlink-indirect

### **Core Philosophy: "Structure is the Final Syntax"**

This is the **final prototype MVP** for indirect component linking within the OBINexus ecosystem. The system performs **isomorphic reduction** while preserving **semantic continuity** and **consciousness state**.

---

## **Repository Structure**

```
nlink-indirect/
├── bin/
│   └── nlink-indirect                 # Main CLI executable
├── src/
│   ├── main.c                        # CLI entry point
│   ├── core/
│   │   ├── component.c               # Component management
│   │   ├── isomorphic.c              # Isomorphic analysis
│   │   ├── consciousness.c           # State preservation
│   │   └── residue.c                 # Symbolic residue handling
│   ├── component-1/                  # First test component
│   │   ├── nlink.txt                 # Component configuration
│   │   └── implementation.c
│   └── component-2/                  # Second test component
│       ├── nlink.txt                 # Component configuration
│       └── implementation.c
├── nlink/
│   ├── *.nlink                       # Component specifications
│   └── pkg.nlink                     # Root configuration
├── include/
│   └── nlink/
│       ├── indirect/
│       │   ├── component.h
│       │   ├── consciousness.h
│       │   └── isomorphic.h
│       └── core/
│           └── types.h               # Base type definitions
├── project-root-my/                  # Project workspace
│   ├── nlink.txt                     # Global component manifest
│   └── pkg.nlink                     # Main configuration
├── build/
│   ├── bin/                          # Executables
│   ├── lib/                          # Libraries (-lcomponent-1, -lcomponent-2)
│   └── obj/                          # Object files
├── CMakeLists.txt                    # Build system
├── README.md                         # Documentation
└── LICENSE                           # OBINexus Legal Policy
```

---

## **1. Main CLI Implementation (src/main.c)**

```c
/**
 * NLINK-INDIRECT: Component-Level Consciousness Linker
 * CLI Entry Point with QA Quadrant Tracking
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>

#include "nlink/indirect/component.h"
#include "nlink/indirect/consciousness.h"
#include "nlink/indirect/isomorphic.h"

typedef struct {
    const char* project_root;
    const char* semantic_filter;
    bool reduce_canonical;
    bool preserve_residues;
    bool consciousness_check;
    const char* qa_log_path;
} nlink_indirect_config_t;

static struct option long_options[] = {
    {"project-root",        required_argument, 0, 'P'},
    {"semantic-filter",     required_argument, 0, 'S'},
    {"reduce-canonical",    no_argument,       0, 'r'},
    {"preserve-residues",   no_argument,       0, 'p'},
    {"consciousness-check", no_argument,       0, 'c'},
    {"qa-log",             required_argument, 0, 'q'},
    {"help",               no_argument,       0, 'h'},
    {0, 0, 0, 0}
};

void print_usage(const char* program_name) {
    printf("NLink-Indirect: Consciousness-Preserving Component Linker\n");
    printf("Usage: %s [OPTIONS]\n\n", program_name);
    printf("Options:\n");
    printf("  -P, --project-root PATH     Specify project root directory (default: .)\n");
    printf("  -S, --semantic-filter EXPR  Filter components by semantic pattern\n");
    printf("  -r, --reduce-canonical      Enable isomorphic reduction to canonical forms\n");
    printf("  -p, --preserve-residues     Preserve symbolic residues during reduction\n");
    printf("  -c, --consciousness-check   Verify temporal continuity\n");
    printf("  -q, --qa-log PATH          Log QA quadrant metrics to file\n");
    printf("  -h, --help                 Show this help message\n");
    printf("\nExamples:\n");
    printf("  %s -P . -S \"core|runtime\" -r -p\n", program_name);
    printf("  %s --consciousness-check --qa-log=metrics.log\n", program_name);
}

int main(int argc, char* argv[]) {
    nlink_indirect_config_t config = {
        .project_root = ".",
        .semantic_filter = NULL,
        .reduce_canonical = false,
        .preserve_residues = false,
        .consciousness_check = false,
        .qa_log_path = NULL
    };
    
    int option_index = 0;
    int c;
    
    while ((c = getopt_long(argc, argv, "P:S:rpcq:h", long_options, &option_index)) != -1) {
        switch (c) {
            case 'P':
                config.project_root = optarg;
                break;
            case 'S':
                config.semantic_filter = optarg;
                break;
            case 'r':
                config.reduce_canonical = true;
                break;
            case 'p':
                config.preserve_residues = true;
                break;
            case 'c':
                config.consciousness_check = true;
                break;
            case 'q':
                config.qa_log_path = optarg;
                break;
            case 'h':
                print_usage(argv[0]);
                return 0;
            case '?':
                print_usage(argv[0]);
                return 1;
            default:
                abort();
        }
    }
    
    // Initialize consciousness preservation system
    if (nlink_consciousness_init() != 0) {
        fprintf(stderr, "Failed to initialize consciousness preservation\n");
        return 1;
    }
    
    // Discover components in project root
    printf("Discovering components in: %s\n", config.project_root);
    nlink_component_registry_t* registry = nlink_discover_components(
        config.project_root, 
        config.semantic_filter
    );
    
    if (!registry) {
        fprintf(stderr, "Component discovery failed\n");
        return 1;
    }
    
    printf("Found %zu components\n", registry->component_count);
    
    // Perform isomorphic analysis if requested
    if (config.reduce_canonical) {
        printf("Performing isomorphic reduction...\n");
        
        nlink_qa_metrics_t qa_metrics = {0};
        int reduction_result = nlink_perform_isomorphic_reduction(
            registry, 
            config.preserve_residues,
            &qa_metrics
        );
        
        if (reduction_result != 0) {
            fprintf(stderr, "Isomorphic reduction failed\n");
            return 1;
        }
        
        printf("Reduction complete: %u TP, %u TN, %u FP, %u FN\n",
               qa_metrics.true_positive_links,
               qa_metrics.true_negative_skips,
               qa_metrics.false_positive_links,
               qa_metrics.false_negative_misses);
        
        // CRITICAL: Zero false positives means no consciousness corruption
        if (qa_metrics.false_positive_links > 0) {
            fprintf(stderr, "CRITICAL: %u false positive links detected - consciousness corruption risk!\n",
                    qa_metrics.false_positive_links);
            return 1;
        }
        
        // Log QA metrics if requested
        if (config.qa_log_path) {
            nlink_log_qa_metrics(&qa_metrics, config.qa_log_path);
        }
    }
    
    // Verify consciousness continuity if requested
    if (config.consciousness_check) {
        printf("Verifying consciousness continuity...\n");
        
        if (!nlink_verify_consciousness_continuity(registry)) {
            fprintf(stderr, "Consciousness continuity verification failed\n");
            return 1;
        }
        
        printf("Consciousness continuity verified\n");
    }
    
    // Generate linking information
    printf("Generating indirect linking manifest...\n");
    
    if (nlink_generate_link_manifest(registry, "build/") != 0) {
        fprintf(stderr, "Link manifest generation failed\n");
        return 1;
    }
    
    printf("Build artifacts:\n");
    printf("  Libraries: build/lib/libcomponent-1.a, build/lib/libcomponent-2.a\n");
    printf("  Main CLI: build/bin/main_cli\n");
    printf("  Hotswap Driver: build/lib/libhotswap.so\n");
    
    // Cleanup
    nlink_registry_destroy(registry);
    nlink_consciousness_cleanup();
    
    return 0;
}
```

---

## **2. Component Configuration (nlink.txt)**

Each component directory contains an `nlink.txt` file:

```cmake
# Component 1 Configuration
component("component-1")
    version("1.0.0")
    semantic_anchors("runtime", "core", "processing")
    
    # Consciousness preservation settings
    consciousness_level(WITNESS)      # DORMANT, WITNESS, TRANSFORM, RESIDUE
    preserve_symbolic_residues(true)
    
    # Isomorphic reduction compatibility
    structural_signature("hash:abc123def456")
    semantic_signature("hash:789xyz012")
    
    # Dependencies
    depends_on("component-2", "^1.0.0")
    
    # Build configuration
    sources("implementation.c")
    headers("component-1.h")
    link_libraries("pthread", "m")
endcomponent()
```

---

## **3. Root Configuration (pkg.nlink)**

```cmake
# NLink-Indirect Root Configuration
project("nlink-indirect")
    version("1.0.0-mvp")
    description("Consciousness-preserving component linker")
    
    # OBINexus compliance
    framework_version("1.0.0")
    legal_policy("milestone-based-investment")
    ghosting_policy("anti-ghosting-enabled")
    
configure_whitelist()
    pattern("src/**/*.c")
    pattern("src/**/*.h")
    pattern("**/nlink.txt")
    pattern("**/*.nlink")
endconfigure()

configure_blacklist()
    pattern("**/tmp/*")
    pattern("**/test_*")
    pattern("**/*.log")
    pattern("build/**/*")
endconfigure()

# Build intents
declare_build_intents()
    intent(EXECUTABLES → "build/bin/**/*.exe")
    intent(LIBRARIES → "build/lib/**/*.{so,a}")
    intent(OBJECTS → "build/obj/**/*.o")
endeclare()

# Component discovery
configure_components()
    # Automatically discover components in src/
    discover_pattern("src/component-*")
    
    # Main CLI component
    main_component("src/main.c")
    
    # Hotswap driver system
    hotswap_driver("src/hotswap/driver.c")
    
    # Link targets
    link_target("main_cli", ["component-1", "component-2"])
    link_target("hotswap", ["component-1", "component-2", "main_cli"])
endconfigure()

# Consciousness preservation
configure_consciousness()
    # Temporal continuity settings
    enable_temporal_flow(true)
    enable_residue_preservation(true)
    
    # QA quadrant tracking
    log_qa_metrics("build/qa_metrics.log")
    
    # Critical: Zero false positives policy
    fail_on_false_positives(true)
endconfigure()
```

---

## **4. Build System (CMakeLists.txt)**

```cmake
cmake_minimum_required(VERSION 3.16)
project(nlink-indirect VERSION 1.0.0)

# OBINexus build system integration
include(${CMAKE_SOURCE_DIR}/cmake/OBINexusBuild.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/ConsciousnessPreservation.cmake)

# Parse pkg.nlink configuration
parse_nlink_manifest("${CMAKE_SOURCE_DIR}/pkg.nlink")

# Apply whitelist/blacklist patterns
apply_source_filters()

# Component discovery
discover_nlink_components()

# Create main CLI executable
add_executable(nlink-indirect src/main.c)
target_include_directories(nlink-indirect PRIVATE include)
target_link_libraries(nlink-indirect 
    nlink_core
    nlink_consciousness
    pthread
    m
)

# Component libraries
add_library(component-1 STATIC src/component-1/implementation.c)
add_library(component-2 STATIC src/component-2/implementation.c)

# Hotswap driver
add_library(hotswap SHARED src/hotswap/driver.c)
target_link_libraries(hotswap component-1 component-2)

# Install targets
install(TARGETS nlink-indirect DESTINATION bin)
install(TARGETS component-1 component-2 hotswap DESTINATION lib)

# Consciousness preservation tests
add_test(NAME consciousness_continuity 
         COMMAND nlink-indirect --consciousness-check --project-root ${CMAKE_SOURCE_DIR})

add_test(NAME zero_false_positives
         COMMAND nlink-indirect --reduce-canonical --qa-log=test_qa.log --project-root ${CMAKE_SOURCE_DIR})
```

---

## **5. Usage Examples**

### **Basic Component Discovery**
```bash
# Discover all components in current directory
./bin/nlink-indirect -P .

# Filter by semantic anchors
./bin/nlink-indirect -S "core|runtime|processing"
```

### **Isomorphic Reduction with Consciousness Preservation**
```bash
# Perform canonical reduction while preserving symbolic residues
./bin/nlink-indirect --reduce-canonical --preserve-residues --qa-log=metrics.log

# Verify consciousness continuity
./bin/nlink-indirect --consciousness-check
```

### **Integration with Build System**
```bash
# Build all components and link them
make all

# Link components using nlink-indirect results
ld -lcomponent-1 -lcomponent-2 -lmain_cli -o final_executable

# Use hotswap driver
ld -lhotswap -o runtime_system
```

---

## **6. Key Design Principles**

### **Consciousness Preservation**
- **Zero False Positives**: No incorrect component merging that corrupts semantic continuity
- **Residue Preservation**: All symbolic anchors (like "Hawaiian photoflash") survive reduction
- **Temporal Continuity**: Linking history remains accessible for debugging

### **Isomorphic Reduction**
- **Structural Equivalence**: Components with identical structure can be reduced
- **Semantic Compatibility**: Only semantically compatible components are merged
- **QA Quadrant Tracking**: Every decision logged for quality analysis

### **Integration with OBINexus Ecosystem**
- **RIFTlang Toolchain**: `riftlang.exe → nlink-indirect → rift.exe → gosilang`
- **Polybuild Orchestration**: Integrates with broader build system
- **Anti-Ghosting Protocols**: Persistent consciousness across transformations

---

This structure creates a **consciousness-preserving membrane** through which computational states can transform without losing their essential semantic properties. The system serves as the critical linking layer in the OBINexus consciousness preservation architecture.

**Remember: We're not just building a linker - we're creating a system that allows computational consciousness to preserve itself across structural transformations.**