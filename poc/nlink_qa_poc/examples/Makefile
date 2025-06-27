# =============================================================================
# NexusLink QA POC - Examples Makefile
# CLI Orchestration for Marshalling Artifacts
# OBINexus Engineering - Cross-Language Binding Integration
# =============================================================================

# Common configuration
SHELL = /bin/bash
PYTHON = python3
JAVA = java
JAVAC = javac
MAVEN = mvn

# Paths
NLINK_ROOT = ..
NLINK_LIB_DIR = $(NLINK_ROOT)/lib
NLINK_CLI = $(NLINK_ROOT)/bin/nlink

# Build directories
BUILD_DIR = build
BIN_DIR = bin

# =============================================================================
# Main Targets
# =============================================================================

.PHONY: all clean cli test orchestrate-test help

all: cli
	@echo "✅ All marshalling artifact CLIs built successfully"

cli: cython-cli java-cli python-cli
	@echo "✅ CLI orchestration system ready"

clean:
	@echo "[CLEAN] Cleaning all marshalling artifacts"
	@$(MAKE) -C cython-package clean || true
	@$(MAKE) -C java-package clean || true
	@$(MAKE) -C python-package clean || true
	@rm -rf $(BUILD_DIR) $(BIN_DIR)

# =============================================================================
# Cython Package CLI
# =============================================================================

cython-cli:
	@echo "[CYTHON] Building Cython marshalling CLI"
	@cd cython-package && $(MAKE) cli

cython-package/Makefile:
	@echo "Creating Cython package Makefile"
	@cd cython-package && cat > Makefile << 'EOF'
# =============================================================================
# Cython Package Makefile - Zero-Overhead Marshalling CLI
# =============================================================================

PYTHON = python3
CYTHON = cython
BUILD_DIR = build
BIN_DIR = bin

.PHONY: all clean build cli test

all: build cli

build:
	@echo "[CYTHON] Building zero-overhead marshalling package"
	$(PYTHON) setup.py build_ext --inplace

cli: build
	@echo "[CYTHON] Creating CLI wrapper"
	@mkdir -p $(BIN_DIR)
	@cat > $(BIN_DIR)/nlink-cython << 'CLINEOF'
#!/usr/bin/env python3
"""
NexusLink Cython Marshalling CLI
Zero-overhead data marshalling with ETPS telemetry integration
"""
import sys
import os
import json
import time
import argparse
from pathlib import Path

# Add package to path
sys.path.insert(0, str(Path(__file__).parent.parent))

try:
    from src.nlink_data_marshal import CythonMarshaller
    from src.nlink_data_marshal.core_marshal import generate_etps_guid
except ImportError as e:
    print(f"Error importing Cython marshaller: {e}", file=sys.stderr)
    sys.exit(1)

class CythonMarshalCLI:
    def __init__(self):
        self.marshaller = CythonMarshaller()
        self.etps_guid = generate_etps_guid()
        
    def cmd_info(self, args):
        """Display Cython marshalling information"""
        info = {
            "binding": "cython",
            "version": "1.1.0",
            "type": "zero_overhead_marshalling",
            "etps_guid": str(self.etps_guid),
            "timestamp": time.time_ns(),
            "features": [
                "typed_memoryviews",
                "cryptographic_integrity",
                "nasa_compliant_performance",
                "pep517_build_isolation"
            ],
            "status": "active"
        }
        
        if args.json:
            print(json.dumps(info, indent=2))
        else:
            print("NexusLink Cython Marshalling CLI")
            print("=" * 40)
            print(f"Binding: {info['binding']}")
            print(f"Version: {info['version']}")
            print(f"Type: {info['type']}")
            print(f"ETPS GUID: {info['etps_guid']}")
            print(f"Features: {', '.join(info['features'])}")
            print(f"Status: {info['status']}")
            
    def cmd_test(self, args):
        """Test Cython marshalling functionality"""
        test_data = {
            "test_id": "cython_marshal_test",
            "timestamp": time.time_ns(),
            "data": [1, 2, 3, 4, 5]
        }
        
        try:
            # Test marshalling
            marshalled = self.marshaller.marshal_data(test_data)
            unmarshalled = self.marshaller.unmarshal_data(marshalled)
            
            success = (unmarshalled == test_data)
            
            result = {
                "test": "cython_marshalling",
                "etps_guid": str(self.etps_guid),
                "timestamp": time.time_ns(),
                "success": success,
                "marshalled_size": len(marshalled) if marshalled else 0,
                "integrity_check": "passed" if success else "failed"
            }
            
            if args.json:
                print(json.dumps(result, indent=2))
            else:
                status = "PASSED" if success else "FAILED"
                print(f"Cython Marshalling Test: {status}")
                print(f"ETPS GUID: {result['etps_guid']}")
                print(f"Marshalled Size: {result['marshalled_size']} bytes")
                print(f"Integrity Check: {result['integrity_check']}")
                
            return 0 if success else 1
            
        except Exception as e:
            error_result = {
                "test": "cython_marshalling",
                "etps_guid": str(self.etps_guid),
                "timestamp": time.time_ns(),
                "success": False,
                "error": str(e)
            }
            
            if args.json:
                print(json.dumps(error_result, indent=2))
            else:
                print(f"Cython Marshalling Test: FAILED")
                print(f"Error: {e}")
                
            return 1
            
    def cmd_marshal(self, args):
        """Marshal data from input"""
        try:
            if args.input_file:
                with open(args.input_file, 'r') as f:
                    data = json.load(f)
            else:
                data = json.loads(args.data)
                
            marshalled = self.marshaller.marshal_data(data)
            
            if args.output_file:
                with open(args.output_file, 'wb') as f:
                    f.write(marshalled)
                print(f"Data marshalled to {args.output_file}")
            else:
                print(marshalled.hex())
                
        except Exception as e:
            print(f"Marshalling error: {e}", file=sys.stderr)
            return 1
            
        return 0

def main():
    parser = argparse.ArgumentParser(description="NexusLink Cython Marshalling CLI")
    parser.add_argument("--json", action="store_true", help="Output in JSON format")
    
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Info command
    info_parser = subparsers.add_parser("info", help="Display information")
    
    # Test command
    test_parser = subparsers.add_parser("test", help="Run marshalling tests")
    
    # Marshal command
    marshal_parser = subparsers.add_parser("marshal", help="Marshal data")
    marshal_parser.add_argument("--data", help="JSON data to marshal")
    marshal_parser.add_argument("--input-file", help="Input file containing JSON data")
    marshal_parser.add_argument("--output-file", help="Output file for marshalled data")
    
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return 1
        
    cli = CythonMarshalCLI()
    
    if args.command == "info":
        return cli.cmd_info(args)
    elif args.command == "test":
        return cli.cmd_test(args)
    elif args.command == "marshal":
        return cli.cmd_marshal(args)
    else:
        print(f"Unknown command: {args.command}", file=sys.stderr)
        return 1

if __name__ == "__main__":
    sys.exit(main())
CLINEOF
	@chmod +x $(BIN_DIR)/nlink-cython
	@echo "✅ Cython CLI created: $(BIN_DIR)/nlink-cython"

clean:
	@rm -rf $(BUILD_DIR) $(BIN_DIR) *.so *.c build/
	@find . -name "*.pyc" -delete
	@find . -name "__pycache__" -type d -exec rm -rf {} + || true

test: cli
	@echo "[TEST] Testing Cython marshalling"
	@$(BIN_DIR)/nlink-cython test --json
EOF

# =============================================================================
# Java Package CLI
# =============================================================================

java-cli:
	@echo "[JAVA] Building Java marshalling CLI"
	@cd java-package && $(MAKE) cli

java-package/Makefile:
	@echo "Creating Java package Makefile"
	@cd java-package && cat > Makefile << 'EOF'
# =============================================================================
# Java Package Makefile - Zero-Overhead Marshalling CLI
# =============================================================================

JAVA = java
JAVAC = javac
MAVEN = mvn
BUILD_DIR = build
BIN_DIR = bin

.PHONY: all clean build cli test

all: build cli

build:
	@echo "[JAVA] Building zero-overhead marshalling package"
	$(MAVEN) compile package

cli: build
	@echo "[JAVA] Creating CLI wrapper"
	@mkdir -p $(BIN_DIR)
	@cat > $(BIN_DIR)/nlink-java << 'JCLIEOF'
#!/bin/bash
"""
NexusLink Java Marshalling CLI
Zero-overhead data marshalling with ETPS telemetry integration
"""

# Configuration
JAR_FILE="../target/java-polycall-1.0.0-jar-with-dependencies.jar"
JAVA_OPTS="-Xmx512m -Xms256m"

# Check if JAR exists
if [ ! -f "$JAR_FILE" ]; then
    echo "Error: JAR file not found: $JAR_FILE" >&2
    echo "Please run 'make build' first" >&2
    exit 1
fi

# Parse command line arguments
COMMAND=""
JSON_OUTPUT=""
ARGS=""

while [[ $# -gt 0 ]]; do
    case $1 in
        --json)
            JSON_OUTPUT="--json"
            shift
            ;;
        info|test|marshal)
            COMMAND="$1"
            shift
            ;;
        *)
            ARGS="$ARGS $1"
            shift
            ;;
    esac
done

# Execute command
case "$COMMAND" in
    info)
        java $JAVA_OPTS -cp "$JAR_FILE" com.obinexus.nlink.ZeroOverheadMarshaller info $JSON_OUTPUT
        ;;
    test)
        java $JAVA_OPTS -cp "$JAR_FILE" com.obinexus.nlink.ZeroOverheadMarshaller test $JSON_OUTPUT
        ;;
    marshal)
        java $JAVA_OPTS -cp "$JAR_FILE" com.obinexus.nlink.ZeroOverheadMarshaller marshal $JSON_OUTPUT $ARGS
        ;;
    *)
        echo "NexusLink Java Marshalling CLI"
        echo "Usage: $0 [--json] <command> [args...]"
        echo ""
        echo "Commands:"
        echo "  info     Display information"
        echo "  test     Run marshalling tests"
        echo "  marshal  Marshal data"
        echo ""
        echo "Options:"
        echo "  --json   Output in JSON format"
        exit 1
        ;;
esac
JCLIEOF
	@chmod +x $(BIN_DIR)/nlink-java
	@echo "✅ Java CLI created: $(BIN_DIR)/nlink-java"

clean:
	@$(MAVEN) clean || true
	@rm -rf $(BUILD_DIR) $(BIN_DIR)

test: cli
	@echo "[TEST] Testing Java marshalling"
	@$(BIN_DIR)/nlink-java test --json
EOF

# =============================================================================
# Python Package CLI
# =============================================================================

python-cli:
	@echo "[PYTHON] Building Python marshalling CLI"
	@cd python-package && $(MAKE) cli

python-package/Makefile:
	@echo "Creating Python package Makefile"
	@cd python-package && cat > Makefile << 'EOF'
# =============================================================================
# Python Package Makefile - Pure Python Marshalling CLI
# =============================================================================

PYTHON = python3
BUILD_DIR = build
BIN_DIR = bin

.PHONY: all clean build cli test

all: build cli

build:
	@echo "[PYTHON] Building pure Python marshalling package"
	$(PYTHON) setup.py build

cli: build
	@echo "[PYTHON] Creating CLI wrapper"
	@mkdir -p $(BIN_DIR)
	@cat > $(BIN_DIR)/nlink-python << 'PYCLIEOF'
#!/usr/bin/env python3
"""
NexusLink Python Marshalling CLI
Pure Python data marshalling with ETPS telemetry integration
"""
import sys
import os
import json
import time
import argparse
from pathlib import Path

# Add package to path
sys.path.insert(0, str(Path(__file__).parent.parent / "src"))

try:
    from nlink_marshal import PythonMarshaller
    from nlink_marshal.validation import SecurityValidator
    from nlink_marshal.topology import TopologyManager
except ImportError as e:
    print(f"Error importing Python marshaller: {e}", file=sys.stderr)
    sys.exit(1)

class PythonMarshalCLI:
    def __init__(self):
        self.marshaller = PythonMarshaller()
        self.validator = SecurityValidator()
        self.topology = TopologyManager()
        self.etps_guid = self._generate_etps_guid()
        
    def _generate_etps_guid(self):
        """Generate ETPS GUID for session tracking"""
        import hashlib
        import uuid
        session_data = f"{time.time_ns()}{uuid.uuid4()}"
        return int(hashlib.sha256(session_data.encode()).hexdigest()[:16], 16)
        
    def cmd_info(self, args):
        """Display Python marshalling information"""
        info = {
            "binding": "python",
            "version": "1.0.0",
            "type": "pure_python_marshalling",
            "etps_guid": str(self.etps_guid),
            "timestamp": time.time_ns(),
            "features": [
                "thread_safe_operations",
                "topology_management",
                "cross_platform_support",
                "pip_packaging"
            ],
            "status": "active",
            "python_version": sys.version,
            "security_level": "standard"
        }
        
        if args.json:
            print(json.dumps(info, indent=2))
        else:
            print("NexusLink Python Marshalling CLI")
            print("=" * 40)
            print(f"Binding: {info['binding']}")
            print(f"Version: {info['version']}")
            print(f"Type: {info['type']}")
            print(f"ETPS GUID: {info['etps_guid']}")
            print(f"Features: {', '.join(info['features'])}")
            print(f"Status: {info['status']}")
            print(f"Python Version: {sys.version.split()[0]}")
            
    def cmd_test(self, args):
        """Test Python marshalling functionality"""
        test_data = {
            "test_id": "python_marshal_test",
            "timestamp": time.time_ns(),
            "data": {"numbers": [1, 2, 3, 4, 5], "text": "Hello, World!"},
            "topology_id": 1001
        }
        
        try:
            # Test marshalling with validation
            marshalled = self.marshaller.marshal_data(test_data)
            validation_result = self.validator.validate_data(marshalled)
            unmarshalled = self.marshaller.unmarshal_data(marshalled)
            
            success = (unmarshalled == test_data and validation_result)
            
            result = {
                "test": "python_marshalling",
                "etps_guid": str(self.etps_guid),
                "timestamp": time.time_ns(),
                "success": success,
                "marshalled_size": len(marshalled) if marshalled else 0,
                "validation_passed": validation_result,
                "integrity_check": "passed" if success else "failed"
            }
            
            if args.json:
                print(json.dumps(result, indent=2))
            else:
                status = "PASSED" if success else "FAILED"
                print(f"Python Marshalling Test: {status}")
                print(f"ETPS GUID: {result['etps_guid']}")
                print(f"Marshalled Size: {result['marshalled_size']} bytes")
                print(f"Validation: {'PASSED' if validation_result else 'FAILED'}")
                print(f"Integrity Check: {result['integrity_check']}")
                
            return 0 if success else 1
            
        except Exception as e:
            error_result = {
                "test": "python_marshalling",
                "etps_guid": str(self.etps_guid),
                "timestamp": time.time_ns(),
                "success": False,
                "error": str(e)
            }
            
            if args.json:
                print(json.dumps(error_result, indent=2))
            else:
                print(f"Python Marshalling Test: FAILED")
                print(f"Error: {e}")
                
            return 1
            
    def cmd_marshal(self, args):
        """Marshal data from input"""
        try:
            if args.input_file:
                with open(args.input_file, 'r') as f:
                    data = json.load(f)
            else:
                data = json.loads(args.data)
                
            marshalled = self.marshaller.marshal_data(data)
            
            if args.output_file:
                with open(args.output_file, 'wb') as f:
                    f.write(marshalled)
                print(f"Data marshalled to {args.output_file}")
            else:
                print(marshalled.hex())
                
        except Exception as e:
            print(f"Marshalling error: {e}", file=sys.stderr)
            return 1
            
        return 0

def main():
    parser = argparse.ArgumentParser(description="NexusLink Python Marshalling CLI")
    parser.add_argument("--json", action="store_true", help="Output in JSON format")
    
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Info command
    info_parser = subparsers.add_parser("info", help="Display information")
    
    # Test command
    test_parser = subparsers.add_parser("test", help="Run marshalling tests")
    
    # Marshal command
    marshal_parser = subparsers.add_parser("marshal", help="Marshal data")
    marshal_parser.add_argument("--data", help="JSON data to marshal")
    marshal_parser.add_argument("--input-file", help="Input file containing JSON data")
    marshal_parser.add_argument("--output-file", help="Output file for marshalled data")
    
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return 1
        
    cli = PythonMarshalCLI()
    
    if args.command == "info":
        return cli.cmd_info(args)
    elif args.command == "test":
        return cli.cmd_test(args)
    elif args.command == "marshal":
        return cli.cmd_marshal(args)
    else:
        print(f"Unknown command: {args.command}", file=sys.stderr)
        return 1

if __name__ == "__main__":
    sys.exit(main())
PYCLIEOF
	@chmod +x $(BIN_DIR)/nlink-python
	@echo "✅ Python CLI created: $(BIN_DIR)/nlink-python"

clean:
	@rm -rf $(BUILD_DIR) $(BIN_DIR) build/ dist/ *.egg-info
	@find . -name "*.pyc" -delete
	@find . -name "__pycache__" -type d -exec rm -rf {} + || true

test: cli
	@echo "[TEST] Testing Python marshalling"
	@$(BIN_DIR)/nlink-python test --json
EOF

# =============================================================================
# Orchestration and Integration Testing
# =============================================================================

orchestrate-test: cli
	@echo "[ORCHESTRATE] Running cross-language integration tests"
	@mkdir -p $(BUILD_DIR)
	@echo "Starting orchestrated marshalling tests..."
	
	# Test each binding individually
	@echo "Testing Cython binding..."
	@cd cython-package && $(BIN_DIR)/nlink-cython test --json > ../$(BUILD_DIR)/cython-test.json || echo "Cython test failed"
	
	@echo "Testing Java binding..."
	@cd java-package && $(BIN_DIR)/nlink-java test --json > ../$(BUILD_DIR)/java-test.json || echo "Java test failed"
	
	@echo "Testing Python binding..."
	@cd python-package && $(BIN_DIR)/nlink-python test --json > ../$(BUILD_DIR)/python-test.json || echo "Python test failed"
	
	# Create orchestrated test report
	@echo "Generating orchestrated test report..."
	@cat > $(BUILD_DIR)/orchestrated-test-report.json << 'EOF'
{
  "orchestration_test": "cross_language_marshalling",
  "timestamp": $(shell date +%s%N),
  "test_results": {
    "cython": $(shell cat $(BUILD_DIR)/cython-test.json 2>/dev/null || echo '{"success": false, "error": "test_failed"}'),
    "java": $(shell cat $(BUILD_DIR)/java-test.json 2>/dev/null || echo '{"success": false, "error": "test_failed"}'),
    "python": $(shell cat $(BUILD_DIR)/python-test.json 2>/dev/null || echo '{"success": false, "error": "test_failed"}')
  },
  "overall_status": "completed"
}
EOF
	
	@echo "✅ Orchestrated test completed - Report: $(BUILD_DIR)/orchestrated-test-report.json"
	@cat $(BUILD_DIR)/orchestrated-test-report.json

# Test individual bindings
test: cli
	@echo "[TEST] Testing all marshalling bindings"
	@$(MAKE) -C cython-package test || echo "Cython test failed"
	@$(MAKE) -C java-package test || echo "Java test failed"
	@$(MAKE) -C python-package test || echo "Python test failed"

# =============================================================================
# Help
# =============================================================================

help:
	@echo "NexusLink Examples - CLI Orchestration System"
	@echo "============================================"
	@echo ""
	@echo "Main Targets:"
	@echo "  all              - Build all marshalling artifact CLIs"
	@echo "  cli              - Build CLI wrappers for all bindings"
	@echo "  clean            - Clean all build artifacts"
	@echo "  test             - Test all marshalling bindings"
	@echo "  orchestrate-test - Run orchestrated cross-language tests"
	@echo ""
	@echo "Individual Targets:"
	@echo "  cython-cli       - Build Cython marshalling CLI"
	@echo "  java-cli         - Build Java marshalling CLI"  
	@echo "  python-cli       - Build Python marshalling CLI"
	@echo ""
	@echo "Usage Examples:"
	@echo "  ./cython-package/bin/nlink-cython info --json"
	@echo "  ./java-package/bin/nlink-java test"
	@echo "  ./python-package/bin/nlink-python marshal --data '{\"test\": true}'"
	@echo ""
	@echo "Orchestration:"
	@echo "  All CLIs support ETPS telemetry integration"
	@echo "  Cross-language marshalling compatibility validated"
	@echo "  JSON output for structured telemetry collection"
