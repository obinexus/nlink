#!/bin/bash
# NexusLink Quick Start

echo "🚀 NexusLink Quick Start"
echo "Building system..."
make clean && make debug

echo "Setting up POC..."
make poc-setup

echo "Running QA specs..."
make spec-run

echo "Testing ETPS..."
./bin/nlink --etps-test --json

echo "✅ Quick start complete!"
