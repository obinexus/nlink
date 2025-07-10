#!/bin/bash

# SemVerX Validation Script
echo "[SEMVERX VALIDATION] Starting comprehensive validation"

# Build the project
make clean
make all-semverx

# Run basic functionality tests
echo "[SEMVERX] Testing basic functionality"
./bin/nlink --help
./bin/nlink --version

# Run SemVerX-specific validation
echo "[SEMVERX] Running SemVerX compatibility validation"
./bin/nlink --config-check --project-root demo_semverx_project

echo "[SEMVERX] Validation completed"
