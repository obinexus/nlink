#!/bin/bash
# OBINexus Constitutional Recovery Protocol
# Emergency rollback for module renaming protocol

set -euo pipefail

BACKUP_DIR="backup/constitutional_backup_20250724_234141"

echo "🚨 OBINexus Constitutional Recovery Protocol: Emergency Rollback"
echo "================================================================"

if [[ ! -d "$BACKUP_DIR" ]]; then
    echo "❌ CRITICAL: Backup directory not found: $BACKUP_DIR"
    exit 1
fi

echo "📦 Restoring original constitutional state..."
rm -rf src/ include/
cp -r "$BACKUP_DIR/src_original" src/
cp -r "$BACKUP_DIR/include_original" include/

if [[ -f "CMakeLists.txt.constitutional_backup_20250724_234141" ]]; then
    cp "CMakeLists.txt.constitutional_backup_20250724_234141" CMakeLists.txt
fi

if [[ -f "Makefile.constitutional_backup_20250724_234141" ]]; then
    cp "Makefile.constitutional_backup_20250724_234141" Makefile
fi

echo "✅ Constitutional recovery completed successfully"
echo "   Original file structure restored from entropy-sealed backups"
