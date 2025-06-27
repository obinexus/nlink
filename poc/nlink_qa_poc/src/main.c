/**
 * NexusLink QA POC - Main Entry Point with Complete SemVerX ETPS
 */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "nlink_qa_poc/core/config.h"
#include "nlink_qa_poc/etps/telemetry.h"

extern int nlink_init(void);
extern void nlink_shutdown(void);
extern int nlink_cli_execute(int argc, char* argv[]);
extern const char* nlink_get_version(void);
extern const char* nlink_get_build_info(void);

static volatile int g_shutdown_requested = 0;

void signal_handler(int signum) {
    (void)signum;
    g_shutdown_requested = 1;
    printf("\n[NLINK_INFO] Shutdown requested\n");
    nlink_shutdown();
    exit(0);
}

void print_banner(void) {
    printf("================================================================================\n");
    printf("🚀 OBINexus NexusLink QA POC - SemVerX ETPS Integration\n");
    printf("================================================================================\n");
    printf("Version: %s\n", nlink_get_version());
    printf("Build: %s\n", nlink_get_build_info());
    printf("Author: Nnamdi Michael Okpala (OBINexus Computing)\n");
    printf("Framework: OBINexus Mathematical Framework with #NoGhosting Policy\n");
    printf("================================================================================\n\n");
}

int main(int argc, char* argv[]) {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    print_banner();
    
    if (argc < 2) {
        printf("ℹ️  NexusLink initialized with SemVerX ETPS support\n");
        printf("ℹ️  Use --help for available commands\n\n");
        
        printf("📋 Available Commands:\n");
        printf("  --version                Show version information\n");
        printf("  --etps-test             Test ETPS system\n");
        printf("  --validate-compatibility Validate component compatibility\n");
        printf("  --semverx-status        Show SemVerX status\n");
        printf("  --migration-plan        Generate migration plan\n\n");
        
        return 0;
    }
    
    int result = nlink_cli_execute(argc, argv);
    
    if (!g_shutdown_requested) {
        nlink_shutdown();
    }
    
    return result;
}
