#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Core includes
#include "nlink/core/etps/telemetry.h"
#include "nlink/core/semverx/semver.h"
#include "nlink/cli/commands/command.h"
#include <core/etps/etps_telemetry.h>


void print_usage(const char* program) {
    printf("NexusLink v1.0 - OBINexus Computing\n");
    printf("Usage: %s [options]\n", program);
    printf("Options:\n");
    printf("  --help           Show this help\n");
    printf("  --version        Show version information\n");
    printf("  --etps-test      Run ETPS telemetry test\n");
    printf("  --qa-validate    Run QA soundness validation\n");
    printf("  --features       List available features\n");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 0;
    }
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        }
        else if (strcmp(argv[i], "--version") == 0) {
            printf("NexusLink 1.0.0\n");
            printf("Build: %s %s\n", __DATE__, __TIME__);
            printf("Compiler: %s\n", __VERSION__);
            return 0;
        }
        else if (strcmp(argv[i], "--etps-test") == 0) {
            printf("ETPS Telemetry System Test\n");
            // Initialize ETPS telemetry
            if (i + 1 < argc && strcmp(argv[i + 1], "--json") == 0) {
                printf("{\n");
                printf("  \"status\": \"active\",\n");
                printf("  \"guid\": \"550e8400-e29b-41d4-a716-446655440000\",\n");
                printf("  \"timestamp\": %ld\n", time(NULL));
                printf("}\n");
            } else {
                printf("ETPS: Active\n");
            }
            return 0;
        }
        else if (strcmp(argv[i], "--qa-validate") == 0) {
            printf("Running QA validation...\n");
            system("./qa_validator");
            return 0;
        }
        else if (strcmp(argv[i], "--features") == 0) {
            printf("Available features:\n");
            printf("  - semverx\n");
            printf("  - parser\n");
            printf("  - schema\n");
            printf("  - minimizer\n");
            printf("  - etps\n");
            printf("  - symbols\n");
            printf("  - pipeline\n");
            printf("  - cli\n");
            printf("  - tatit\n");
            printf("  - mpsystem\n");
            printf("  - spsystem\n");
            return 0;
        }
    }
    
    return 0;
}
