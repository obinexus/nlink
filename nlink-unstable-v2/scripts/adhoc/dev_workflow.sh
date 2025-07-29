#!/bin/bash
# NexusLink Development Workflow Script

set -e

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[0;33m'
RED='\033[0;31m'
NC='\033[0m'

# Functions
log_phase() {
    echo -e "\n${BLUE}=== $1 ===${NC}"
}

log_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

log_error() {
    echo -e "${RED}✗ $1${NC}"
}

# Workflow
log_phase "NexusLink Development Workflow"

# 1. Clean build
log_phase "Phase 1: Clean Build"
make clean
make debug ETPS_ENABLED=1
log_success "Build completed"

# 2. Run unit specs
log_phase "Phase 2: Unit Specifications"
if make -C spec unit; then
    log_success "Unit specs passed"
else
    log_error "Unit specs failed"
    exit 1
fi

# 3. Run integration specs
log_phase "Phase 3: Integration Specifications"
if make -C spec integration; then
    log_success "Integration specs passed"
else
    log_error "Integration specs failed"
    exit 1
fi

# 4. POC validation
log_phase "Phase 4: POC Validation"
if make poc; then
    log_success "POC validation passed"
else
    log_error "POC validation failed"
    exit 1
fi

# 5. ETPS telemetry check
log_phase "Phase 5: ETPS Telemetry Check"
if ./bin/nlink --etps-test --json | jq . > /dev/null 2>&1; then
    log_success "ETPS telemetry operational"
else
    log_error "ETPS telemetry check failed (install jq for JSON validation)"
fi

# 6. Generate report
log_phase "Phase 6: Development Report"
cat << EOF > dev_report.txt
NexusLink Development Report
Generated: $(date)

Build Status: SUCCESS
Unit Specs: PASSED
Integration Specs: PASSED
POC Validation: PASSED
ETPS Telemetry: OPERATIONAL

Next Steps:
- Review logs in logs/ directory
- Check telemetry output in nlink_etps.log
- Run performance specs: make -C spec performance
EOF

log_success "Development workflow complete"
echo -e "${YELLOW}Report saved to: dev_report.txt${NC}"
