# java-package CLI Makefile

.PHONY: all clean cli

all: cli

cli:
	@echo "Creating java-package CLI wrapper"
	@mkdir -p bin
	@cat > bin/nlink-java-package << 'CLIEOF'
#!/usr/bin/env python3
"""
NexusLink java-package CLI
ETPS-integrated marshalling interface
"""
import sys
import json
import time

def generate_etps_guid():
    import hashlib
    return int(hashlib.sha256(str(time.time_ns()).encode()).hexdigest()[:16], 16)

def main():
    etps_guid = generate_etps_guid()
    
    if len(sys.argv) > 1 and sys.argv[1] == "info":
        info = {
            "binding": "java-package",
            "version": "1.0.0",
            "etps_guid": str(etps_guid),
            "timestamp": time.time_ns(),
            "status": "active"
        }
        
        if "--json" in sys.argv:
            print(json.dumps(info, indent=2))
        else:
            print(f"NexusLink java-package CLI")
            print(f"ETPS GUID: {etps_guid}")
            print(f"Status: {info['status']}")
    else:
        print("Usage: nlink-java-package info [--json]")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
CLIEOF
	@chmod +x bin/nlink-java-package
	@echo "✅ java-package CLI created"

clean:
	@rm -rf bin/ build/
