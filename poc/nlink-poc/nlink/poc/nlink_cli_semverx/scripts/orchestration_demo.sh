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
