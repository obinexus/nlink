#!/bin/bash
# Setup POC test files

set -e

# Create POC directory
mkdir -p poc

# Create test.js
cat > poc/test.js << 'EOF'
// NexusLink POC JavaScript Test
function nlinkETPS() {
    const telemetry = {
        guid: crypto.randomUUID ? crypto.randomUUID() : 
              'xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'.replace(/[xy]/g, function(c) {
                  const r = Math.random() * 16 | 0;
                  const v = c === 'x' ? r : (r & 0x3 | 0x8);
                  return v.toString(16);
              }),
        timestamp: Date.now(),
        operation: 'js_poc_test',
        metrics: {
            memory: (typeof performance !== 'undefined' && performance.memory) ? 
                    performance.memory.usedJSHeapSize : 0,
            timing: typeof performance !== 'undefined' ? performance.now() : 0
        }
    };
    
    console.log('ETPS:', JSON.stringify(telemetry));
    return telemetry;
}

// Execute POC test
if (typeof module !== 'undefined' && module.exports) {
    module.exports = { nlinkETPS };
} else {
    nlinkETPS();
}
EOF

# Create test.css
cat > poc/test.css << 'EOF'
/* NexusLink POC CSS Test */
:root {
    --nlink-primary: #007acc;
    --nlink-telemetry: #28a745;
    --etps-error: #dc3545;
    --etps-warning: #ffc107;
    --etps-info: #17a2b8;
}

.nlink-poc-container {
    display: flex;
    flex-direction: column;
    padding: 1rem;
    background: var(--nlink-primary);
    color: white;
    font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
}

.etps-telemetry-display {
    font-family: 'Courier New', Courier, monospace;
    background: #1e1e1e;
    color: var(--nlink-telemetry);
    padding: 1rem;
    border-radius: 4px;
    margin-top: 1rem;
    overflow-x: auto;
}

.etps-log-entry {
    margin: 0.25rem 0;
    padding: 0.25rem 0.5rem;
    border-left: 3px solid transparent;
}

.etps-log-entry.error {
    border-left-color: var(--etps-error);
    background: rgba(220, 53, 69, 0.1);
}

.etps-log-entry.warning {
    border-left-color: var(--etps-warning);
    background: rgba(255, 193, 7, 0.1);
}

.etps-log-entry.info {
    border-left-color: var(--etps-info);
    background: rgba(23, 162, 184, 0.1);
}
EOF

# Create test.html
cat > poc/test.html << 'EOF'
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>NexusLink ETPS Telemetry POC</title>
    <link rel="stylesheet" href="test.css">
</head>
<body>
    <div class="nlink-poc-container">
        <h1>NexusLink ETPS Telemetry POC</h1>
        <p>OBINexus Computing - Soundness-First Development</p>
        
        <div id="etps-output" class="etps-telemetry-display">
            <div class="etps-log-entry info">Initializing ETPS telemetry...</div>
        </div>
        
        <div style="margin-top: 2rem;">
            <h2>Telemetry Controls</h2>
            <button onclick="runETPSTest()">Run ETPS Test</button>
            <button onclick="clearLogs()">Clear Logs</button>
            <button onclick="exportLogs()">Export Logs</button>
        </div>
    </div>
    
    <script src="test.js"></script>
    <script>
        const output = document.getElementById('etps-output');
        const logs = [];
        
        function addLog(entry, level = 'info') {
            const logDiv = document.createElement('div');
            logDiv.className = `etps-log-entry ${level}`;
            logDiv.textContent = `[${new Date().toISOString()}] ${entry}`;
            output.appendChild(logDiv);
            logs.push({ timestamp: Date.now(), level, message: entry });
            output.scrollTop = output.scrollHeight;
        }
        
        function runETPSTest() {
            try {
                const telemetry = nlinkETPS();
                addLog('ETPS Test Executed', 'info');
                addLog(`Telemetry: ${JSON.stringify(telemetry, null, 2)}`, 'info');
            } catch (e) {
                addLog(`Error: ${e.message}`, 'error');
            }
        }
        
        function clearLogs() {
            output.innerHTML = '<div class="etps-log-entry info">Logs cleared</div>';
            logs.length = 0;
        }
        
        function exportLogs() {
            const blob = new Blob([JSON.stringify(logs, null, 2)], 
                                { type: 'application/json' });
            const url = URL.createObjectURL(blob);
            const a = document.createElement('a');
            a.href = url;
            a.download = `etps_logs_${Date.now()}.json`;
            a.click();
            URL.revokeObjectURL(url);
            addLog('Logs exported', 'info');
        }
        
        // Initial test run
        document.addEventListener('DOMContentLoaded', () => {
            addLog('DOM loaded, running initial test', 'info');
            runETPSTest();
        });
    </script>
</body>
</html>
EOF

echo "✅ POC test files created successfully"
ls -la poc/
