#!/bin/bash
# Fix ETPS header file names

set -e

ETPS_DIR="include/nlink/core/etps"

echo "Fixing ETPS header file names..."

# Rename misspelled files
if [ -f "$ETPS_DIR/epts_telemtry.h" ]; then
    mv "$ETPS_DIR/epts_telemtry.h" "$ETPS_DIR/etps_telemetry.h"
    echo "✓ Renamed epts_telemtry.h -> etps_telemetry.h"
fi

if [ -f "$ETPS_DIR/epts_telemtry.h.backup" ]; then
    mv "$ETPS_DIR/epts_telemtry.h.backup" "$ETPS_DIR/etps_telemetry.h.backup"
    echo "✓ Renamed backup files"
fi

if [ -f "$ETPS_DIR/epts_telemtry.h.old" ]; then
    mv "$ETPS_DIR/epts_telemtry.h.old" "$ETPS_DIR/etps_telemetry.h.old"
    echo "✓ Renamed old files"
fi

echo "Current ETPS directory structure:"
ls -la "$ETPS_DIR/"
