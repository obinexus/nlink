// Zero-Knowledge Weighted XOR Proofs - Rule Definition DSL
// OBINexus Standard Compliant
// 
// This file defines malware detection rules based on kernel scheduler behavior

// Rule 1: Detect rapid context switching (potential rootkit behavior)
rule rapid_context_switch {
    weight 750
    priority high
    
    when event context_switch and 
         threshold switch_rate > 1000 and
         threshold time_window < 1000000000  // 1 second
}

// Rule 2: Detect abnormal priority escalation
rule priority_escalation {
    weight 900
    priority critical
    
    when event priority_change and
         pattern "low_to_high" and
         threshold escalation_count > 5 and
         not event authorized_escalation
}

// Rule 3: Detect suspicious thread creation patterns
rule thread_spawn_anomaly {
    weight 600
    priority medium
    
    when event thread_create and
         threshold spawn_rate > 100 and
         threshold unique_parents < 3
}

// Rule 4: Detect scheduler manipulation attempts
rule scheduler_tampering {
    weight 1000
    priority critical
    
    when (event state_transition and pattern "invalid_transition") or
         (event scheduler_tick and threshold tick_variance > 50)
}

// Rule 5: Detect CPU migration anomalies
rule migration_pattern {
    weight 500
    priority medium
    
    when event migration and
         threshold migration_frequency > 20 and
         pattern "ping_pong"  // Rapid back-and-forth between CPUs
}

// Rule 6: Detect load balancer abuse
rule load_balance_exploit {
    weight 800
    priority high
    
    when event load_balance and
         threshold imbalance_ratio > 10 and
         event artificial_load
}

// Rule 7: Composite rule for advanced rootkit detection
rule advanced_rootkit {
    weight 950
    priority critical
    
    when (event context_switch and threshold switch_rate > 500) and
         (event priority_change and pattern "unauthorized") and
         (event thread_create and threshold spawn_rate > 50) and
         not event system_maintenance
}

// Rule 8: Detect kernel-level cryptominer behavior
rule crypto_mining_detection {
    weight 850
    priority high
    
    when event context_switch and
         pattern "cpu_intensive" and
         threshold cpu_usage > 90 and
         threshold consistent_duration > 300000000000  // 5 minutes
}

// Rule 9: Detect time-based evasion techniques
rule timing_evasion {
    weight 700
    priority high
    
    when event scheduler_tick and
         pattern "irregular_timing" and
         threshold tick_deviation > 20
}

// Rule 10: Detect process hollowing attempts
rule process_hollowing {
    weight 900
    priority critical
    
    when event state_transition and
         pattern "suspended_to_active" and
         event memory_modification and
         threshold modification_size > 4096
}
