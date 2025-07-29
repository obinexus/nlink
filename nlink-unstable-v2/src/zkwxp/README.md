# Zero-Knowledge Weighted XOR Proofs (ZK-WXP)

## OBINexus Standard Compliant Implementation

This module implements Zero-Knowledge Weighted XOR Proofs for malware detection through kernel scheduler audit log analysis, integrated with the NexusLink POC framework.

## Architecture Overview

The ZK-WXP system consists of several key components:

### 1. Core Engine (`zkwxp_core.c`)
- Implements the weighted XOR accumulator
- Manages rule evaluation against audit log entries
- Generates and verifies zero-knowledge proofs
- Integrates with NLink ETPS telemetry

### 2. Domain Specific Language (`dsl/zkwxp_dsl.c`)
- Custom DSL for expressing detection rules
- Compiles rules to bytecode for efficient execution
- Supports complex conditions and pattern matching

### 3. Remote Scanning (`remote/zkwxp_remote.c`)
- Enables scanning of kernel data structures
- Provides read-only access through secure channels
- Maintains isolation and attestation

### 4. QA Framework (`qa/zkwxp_qa.c`)
- Waterfall QA methodology implementation
- Zero false-positive tolerance enforcement
- Quadrant analysis (TP/TN/FP/FN)

## Building

```bash
# From the zkwxp directory
make all            # Build libraries
make test           # Build and run tests
make qa-validate    # Run QA validation
make install        # Install to lib directory
```

## Git Workflow

Use the provided workflow script for managing branches:

```bash
# Initialize feature branches
./scripts/zkwxp_git_workflow.sh init

# Work on specific components
./scripts/zkwxp_git_workflow.sh core
./scripts/zkwxp_git_workflow.sh dsl
./scripts/zkwxp_git_workflow.sh remote
./scripts/zkwxp_git_workflow.sh integration

# Merge all branches
./scripts/zkwxp_git_workflow.sh merge

# Create release
./scripts/zkwxp_git_workflow.sh release 1.0.0
```

## DSL Rule Definition

Rules are defined in a custom DSL that compiles to efficient bytecode:

```dsl
rule rapid_context_switch {
    weight 750
    priority high
    
    when event context_switch and 
         threshold switch_rate > 1000 and
         threshold time_window < 1000000000  // 1 second
}
```

### Rule Components:
- **weight**: Importance factor for XOR accumulation (0-1000)
- **priority**: Detection priority (critical/high/medium/low)
- **when**: Condition expression using events and thresholds

## API Usage

### Basic Usage

```c
#include "nlink/zkwxp/zkwxp_core.h"

// Initialize context
zkwxp_config_t config = {
    .proof_rounds = 10,
    .challenge_bits = 128,
    .batch_size = 1024,
    .enable_etps_telemetry = true
};

zkwxp_context_t* ctx;
zkwxp_init(&ctx, &config);

// Load rules
zkwxp_load_rules(ctx, "config/zkwxp_rules.dsl");

// Process audit entries
zkwxp_process_entries(ctx, entries, entry_count);

// Generate proof
zkwxp_proof_t* proof;
zkwxp_generate_proof(ctx, &proof);

// Verify proof
NexusResult result = zkwxp_verify_proof(ctx, proof);

// Cleanup
zkwxp_destroy(ctx);
```

### Remote Scanning

```c
// Initialize remote capability
uint8_t auth_key[32] = { /* secure key */ };
zkwxp_remote_init(ctx, "kernel://localhost:9999", auth_key, 32);

// Scan kernel structures
zkwxp_proof_t* scan_proof;
zkwxp_remote_scan(ctx, kernel_addr, scan_size, &scan_proof);
```

## Zero-Knowledge Protocol

The proof protocol ensures:
1. **Completeness**: Valid scans always produce verifiable proofs
2. **Soundness**: Invalid scans cannot produce valid proofs
3. **Zero-Knowledge**: Proofs reveal nothing about the actual data

### Proof Structure:
- **Commitment**: SHA-256 hash of accumulator state
- **Challenge**: Random value from verifier
- **Response**: Computed from commitment and challenge
- **Verification**: Validates the mathematical relationship

## Integration with NLink

The ZK-WXP system integrates seamlessly with NLink's build pipeline:

### Manifest Gating
```yaml
# pkg.nlink.in
modules:
  zkwxp:
    whitelist:
      - src/zkwxp/core/*.c
      - src/zkwxp/dsl/*.c
    blacklist:
      - src/zkwxp/experimental/*
```

### Build Orchestration
The system follows NLink's statement→expression transformation pattern, ensuring all DSL rules are compiled to efficient expression-based bytecode.

## Performance Considerations

- **Batch Processing**: Process entries in configurable batches
- **Accumulator Efficiency**: O(1) updates per entry
- **Proof Generation**: O(n) where n is proof_rounds
- **Memory Usage**: Configurable cache size for entries

## Security Notes

1. **Authentication**: All remote connections use cryptographic authentication
2. **Isolation**: Kernel scanning is read-only with strict boundaries
3. **Attestation**: Build artifacts are signed and verified
4. **Entropy**: Shannon entropy validation on proof data

## Troubleshooting

### Common Issues:

1. **QA Validation Failures**
   - Check for false positives in rule definitions
   - Verify threshold values are appropriate
   - Review QA quadrant report

2. **Performance Issues**
   - Adjust batch_size in configuration
   - Enable performance profiling with `make profile`
   - Check ETPS telemetry logs

3. **Integration Problems**
   - Ensure NLink is properly built and installed
   - Verify include paths in Makefile
   - Check library dependencies with `ldd`

## Contributing

Follow the OBINexus methodology:
1. All changes must pass waterfall QA
2. Zero false-positive tolerance
3. Use provided Git workflow script
4. Document all rule definitions

## License

This implementation follows the OBINexus Legal Policy architecture with milestone-based investment and #NoGhosting principles.
