#!/usr/bin/env python3
"""
OBINexus NexusLink Python POC Bridge
Provides JS/CSS/HTML processing capabilities for proof of concept validation
"""

import os
import sys
import json
import subprocess
import logging
from datetime import datetime
from typing import Dict, List, Optional
import uuid
import hashlib
import math

class NLinkPOCBridge:
    """Python bridge for NexusLink proof of concept systems"""
    
    def __init__(self, project_root: str):
        self.project_root = project_root
        self.etps_guid = str(uuid.uuid4())
        self.logger = self._setup_logging()
        
    def _setup_logging(self) -> logging.Logger:
        """Initialize ETPS-compatible logging"""
        logger = logging.getLogger('nlink_poc')
        handler = logging.FileHandler('nlink_etps_poc.log')
        formatter = logging.Formatter(
            '%(asctime)s [%(levelname)s] ETPS:%(name)s - %(message)s'
        )
        handler.setFormatter(formatter)
        logger.addHandler(handler)
        logger.setLevel(logging.DEBUG)
        return logger
    
    def process_web_assets(self, asset_type: str, file_path: str) -> Dict:
        """Process JS/CSS/HTML assets with telemetry"""
        start_time = datetime.now()
        
        result = {
            'etps_guid': self.etps_guid,
            'asset_type': asset_type,
            'file_path': file_path,
            'timestamp': start_time.isoformat(),
            'status': 'processing'
        }
        
        try:
            with open(file_path, 'r') as f:
                content = f.read()
            
            # Calculate Shannon entropy for validation
            entropy = self._calculate_shannon_entropy(content.encode())
            
            result.update({
                'status': 'completed',
                'file_size': len(content),
                'entropy': entropy,
                'checksum': hashlib.sha256(content.encode()).hexdigest()
            })
            
            self.logger.info(f"Processed {asset_type} asset: {file_path}")
            
        except Exception as e:
            result['status'] = 'error'
            result['error'] = str(e)
            self.logger.error(f"Failed to process {asset_type}: {e}")
        
        return result
    
    def _calculate_shannon_entropy(self, data: bytes) -> float:
        """Calculate Shannon entropy for data validation (FIXED)"""
        if not data:
            return 0.0
        
        # Count byte frequencies
        frequency = {}
        for byte in data:
            frequency[byte] = frequency.get(byte, 0) + 1
        
        # Calculate entropy using proper formula
        entropy = 0.0
        data_len = len(data)
        
        for count in frequency.values():
            if count > 0:
                p = count / data_len
                # Use log2 for entropy in bits
                entropy -= p * math.log2(p)
        
        return entropy
    
    def run_nlink_command(self, command: List[str]) -> Dict:
        """Execute nlink binary with ETPS telemetry"""
        env = os.environ.copy()
        env['LD_LIBRARY_PATH'] = os.path.join(self.project_root, 'lib')
        
        result = {
            'etps_guid': self.etps_guid,
            'command': ' '.join(command),
            'timestamp': datetime.now().isoformat()
        }
        
        try:
            proc = subprocess.run(
                command,
                capture_output=True,
                text=True,
                env=env,
                cwd=self.project_root
            )
            
            result.update({
                'status': 'success' if proc.returncode == 0 else 'error',
                'return_code': proc.returncode,
                'stdout': proc.stdout,
                'stderr': proc.stderr
            })
            
        except Exception as e:
            result['status'] = 'error'
            result['error'] = str(e)
        
        return result

# POC Test Runner
if __name__ == "__main__":
    bridge = NLinkPOCBridge(os.getcwd())
    
    # Test web asset processing
    test_assets = [
        ('javascript', 'poc/test.js'),
        ('css', 'poc/test.css'),
        ('html', 'poc/test.html')
    ]
    
    for asset_type, path in test_assets:
        result = bridge.process_web_assets(asset_type, path)
        print(json.dumps(result, indent=2))
    
    # Test nlink integration
    nlink_result = bridge.run_nlink_command(['./bin/nlink', '--etps-test', '--json'])
    print(json.dumps(nlink_result, indent=2))