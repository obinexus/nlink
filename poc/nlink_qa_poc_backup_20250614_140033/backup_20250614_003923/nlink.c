#include <stddef.h>
#include <stdio.h>
#include "nlink_qa_poc/core/config.h"
#include "nlink_qa_poc/etps/telemetry.h"

const char* nlink_version(void) {
    return "1.0.0";
}

int nlink_init(void) {
    return etps_init();
}

void nlink_shutdown(void) {
    etps_shutdown();
}
