#!/bin/bash

# =============================================================================
# NexusLink QA POC Execution Script
# Comprehensive Quality Assurance Test Runner
# =============================================================================

set -e

QA_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$QA_ROOT"

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[0;33m'
RED='\033[0;31m'
NC='\033[0m'

log_phase() { echo -e "${BLUE}[QA RUNNER]${NC} $1"; }
log_success() { echo -e "${GREEN}[SUCCESS]${NC} $1"; }
log_warning() { echo -e "${YELLOW}[WARNING]${NC} $1"; }
log_error() { echo -e "${RED}[ERROR]${NC} $1"; }

echo "==============================================="
echo "NexusLink QA POC - Quality Assurance Runner"
echo "OBINexus Engineering - Systematic Validation"
echo "==============================================="
echo

# Phase 1: Environment Check
log_phase "Validating build environment"
command -v gcc >/dev/null 2>&1 || { log_error "GCC not found"; exit 1; }
command -v make >/dev/null 2>&1 || { log_error "Make not found"; exit 1; }
command -v python >/dev/null 2>&1 || { log_warning "Python not found - some tests will be skipped"; }
command -v javac >/dev/null 2>&1 || { log_warning "Java not found - Java tests will be skipped"; }
log_success "Build environment validated"

# Phase 2: Build Everything
log_phase "Building QA test suite and examples"
make clean
make all
log_success "Build completed successfully"

# Phase 3: Run Tests
log_phase "Running comprehensive test suite"
make test
log_success "All tests passed"

# Phase 4: Validate Integration
log_phase "Validating NexusLink integration"
make validate-nlink-integration
log_success "NexusLink integration validated"

# Phase 5: Cross-Language Validation
log_phase "Validating cross-language compatibility"
make validate-cross-language
log_success "Cross-language compatibility validated"

# Phase 6: Quality Metrics
log_phase "Generating quality metrics"
make validate
log_success "Quality assurance validation completed"

echo
echo "==============================================="
echo "🎉 QA POC VALIDATION COMPLETED SUCCESSFULLY 🎉"
echo "==============================================="
echo
echo "Summary:"
echo "✅ Build Environment: VALIDATED"
echo "✅ Unit Tests: PASSED"
echo "✅ Integration Tests: PASSED" 
echo "✅ Cross-Language Compatibility: VALIDATED"
echo "✅ NexusLink CLI Integration: VALIDATED"
echo "✅ Quality Gates: ALL PASSED"
echo
echo "The NexusLink QA POC demonstrates quality over quantity"
echo "with comprehensive validation across multiple languages."
echo
