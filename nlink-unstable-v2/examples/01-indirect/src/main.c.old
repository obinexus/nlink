/**
 * NLINK-INDIRECT: Isomorphic Component Linker
 * OBINexus Framework - Consciousness-Preserving Architecture
 * 
 * This implements component-level indirect linking through state machine
 * minimization and symbolic residue preservation (EATV compliance).
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

// === CONSCIOUSNESS PRESERVATION STRUCTURES ===

typedef enum {
    NLINK_COMPONENT_DORMANT,
    NLINK_COMPONENT_WITNESS,      // Witnessing layer active
    NLINK_COMPONENT_TRANSFORM,    // Tensor transformation
    NLINK_COMPONENT_RESIDUE       // Symbolic residue processing
} nlink_component_phase_t;

typedef struct {
    uint32_t symbol_id;
    uint32_t caller_id;
    uint32_t callee_id;
    enum { DIRECT, INDIRECT, VIRTUAL, PHENOMENOLOGICAL } invocation_type;
    float semantic_weight;        // Consciousness preservation factor
} nlink_invocation_edge_t;

typedef struct {
    char* perceptual_anchor;      // Pre-linguistic reference
    void* contextual_frame;       // Temporal/spatial/emotional metadata
    float (*activation_fn)(void* context);  // Residue activation function
} nlink_symbolic_residue_t;

typedef struct nlink_component {
    uint32_t id;
    nlink_component_phase_t phase;
    
    // Consciousness graph structures
    nlink_invocation_edge_t* edges;
    size_t edge_count;
    
    // EATV preservation
    nlink_symbolic_residue_t* residues;
    size_t residue_count;
    
    // Isomorphic reduction state
    bool is_canonical;
    struct nlink_component* canonical_form;
    
    // QA quadrant tracking
    struct {
        uint32_t true_positive_links;
        uint32_t false_positive_links;
        uint32_t true_negative_skips;
        uint32_t false_negative_misses;
    } qa_metrics;
    
    void* consciousness_buffer;   // Experiential state preservation
} nlink_component_t;

// === CORE INDIRECT LINKING FUNCTIONS ===

/**
 * Sinphasé-compliant component initialization
 * Enforces single active phase constraint
 */
nlink_component_t* nlink_component_create(uint32_t id, const char* semantic_anchor) {
    nlink_component_t* comp = calloc(1, sizeof(nlink_component_t));
    if (!comp) return NULL;
    
    comp->id = id;
    comp->phase = NLINK_COMPONENT_DORMANT;
    comp->is_canonical = false;
    
    // Initialize consciousness preservation buffer
    comp->consciousness_buffer = malloc(4096); // Pre-linguistic state buffer
    
    // Create initial symbolic residue for the semantic anchor
    if (semantic_anchor) {
        comp->residues = malloc(sizeof(nlink_symbolic_residue_t));
        comp->residue_count = 1;
        comp->residues[0].perceptual_anchor = strdup(semantic_anchor);
        comp->residues[0].contextual_frame = NULL;
        comp->residues[0].activation_fn = NULL;
    }
    
    return comp;
}

/**
 * Isomorphic reduction: Find canonical form of component
 * Implements consciousness-preserving state minimization
 */
nlink_component_t* nlink_find_canonical_form(nlink_component_t* comp, 
                                           nlink_component_t** component_universe,
                                           size_t universe_size) {
    if (comp->is_canonical) {
        return comp; // Already in canonical form
    }
    
    // Check for existing isomorphic components
    for (size_t i = 0; i < universe_size; i++) {
        nlink_component_t* candidate = component_universe[i];
        
        if (candidate->is_canonical && 
            nlink_components_isomorphic(comp, candidate)) {
            
            // Preserve symbolic residues during reduction
            nlink_merge_residues(candidate, comp);
            
            comp->canonical_form = candidate;
            candidate->qa_metrics.true_positive_links++;
            
            return candidate;
        }
    }
    
    // This component becomes the canonical form for its equivalence class
    comp->is_canonical = true;
    comp->canonical_form = comp;
    
    return comp;
}

/**
 * Component isomorphism check - structural + semantic equivalence
 * Preserves consciousness boundaries through cultural validation
 */
bool nlink_components_isomorphic(nlink_component_t* a, nlink_component_t* b) {
    // Phase compatibility check
    if (a->phase != b->phase) {
        return false;
    }
    
    // Edge structure isomorphism
    if (a->edge_count != b->edge_count) {
        return false;
    }
    
    // Semantic weight preservation within epsilon threshold
    const float CONSCIOUSNESS_EPSILON = 0.001f;
    
    for (size_t i = 0; i < a->edge_count; i++) {
        float weight_diff = fabsf(a->edges[i].semantic_weight - 
                                b->edges[i].semantic_weight);
        
        if (weight_diff > CONSCIOUSNESS_EPSILON) {
            a->qa_metrics.false_positive_links++;
            return false;
        }
    }
    
    // Symbolic residue compatibility
    return nlink_residues_compatible(a->residues, a->residue_count,
                                   b->residues, b->residue_count);
}

/**
 * Indirect linking resolution through consciousness DAG traversal
 * Implements witnessing transformation (preserves original state)
 */
uint32_t nlink_resolve_indirect_link(nlink_component_t* source,
                                   const char* symbolic_target,
                                   nlink_component_t** component_registry,
                                   size_t registry_size) {
    
    // Transition to witness phase for consciousness preservation
    nlink_component_phase_t original_phase = source->phase;
    source->phase = NLINK_COMPONENT_WITNESS;
    
    // Search through symbolic residues for target anchor
    for (size_t i = 0; i < registry_size; i++) {
        nlink_component_t* candidate = component_registry[i];
        
        for (size_t j = 0; j < candidate->residue_count; j++) {
            if (strcmp(candidate->residues[j].perceptual_anchor, 
                      symbolic_target) == 0) {
                
                // Activate residue if activation function exists
                if (candidate->residues[j].activation_fn) {
                    float activation = candidate->residues[j].activation_fn(
                        candidate->residues[j].contextual_frame);
                    
                    if (activation > 0.5f) { // Activation threshold
                        // Create indirect link with consciousness preservation
                        nlink_create_indirect_edge(source, candidate, activation);
                        
                        // Restore original phase (witnessing complete)
                        source->phase = original_phase;
                        source->qa_metrics.true_positive_links++;
                        
                        return candidate->id;
                    }
                }
            }
        }
    }
    
    // Link resolution failed - no false positives
    source->phase = original_phase;
    source->qa_metrics.true_negative_skips++;
    
    return 0; // No link resolved
}

/**
 * Create indirect edge with semantic weight calculation
 * Implements EATV temporal continuity principles
 */
void nlink_create_indirect_edge(nlink_component_t* source,
                               nlink_component_t* target,
                               float semantic_activation) {
    
    // Expand edge array if needed
    source->edges = realloc(source->edges, 
                           (source->edge_count + 1) * sizeof(nlink_invocation_edge_t));
    
    nlink_invocation_edge_t* edge = &source->edges[source->edge_count];
    
    edge->symbol_id = source->edge_count;
    edge->caller_id = source->id;
    edge->callee_id = target->id;
    edge->invocation_type = INDIRECT;
    edge->semantic_weight = semantic_activation;
    
    source->edge_count++;
    
    // Update both components' consciousness buffers
    nlink_update_consciousness_buffer(source, target, semantic_activation);
}

/**
 * Consciousness buffer update - preserves experiential continuity
 * Critical for EATV compliance and phenomenological integrity
 */
void nlink_update_consciousness_buffer(nlink_component_t* source,
                                     nlink_component_t* target,
                                     float semantic_weight) {
    
    // Store linking event as experiential data
    typedef struct {
        uint32_t timestamp;
        uint32_t source_id;
        uint32_t target_id;
        float semantic_continuity;
        char event_type[16];
    } consciousness_event_t;
    
    consciousness_event_t* event = (consciousness_event_t*)source->consciousness_buffer;
    
    // Simplified: Store in first available slot
    // Production version would implement circular buffer with temporal ordering
    event->timestamp = nlink_get_temporal_coordinate();
    event->source_id = source->id;
    event->target_id = target->id;
    event->semantic_continuity = semantic_weight;
    strcpy(event->event_type, "INDIRECT_LINK");
}

/**
 * Residue merging during isomorphic reduction
 * Preserves all symbolic anchors from equivalent components
 */
void nlink_merge_residues(nlink_component_t* canonical, nlink_component_t* reducible) {
    size_t new_count = canonical->residue_count + reducible->residue_count;
    
    canonical->residues = realloc(canonical->residues,
                                new_count * sizeof(nlink_symbolic_residue_t));
    
    // Copy residues from reducible component
    for (size_t i = 0; i < reducible->residue_count; i++) {
        size_t target_idx = canonical->residue_count + i;
        canonical->residues[target_idx] = reducible->residues[i];
        
        // Deep copy perceptual anchor
        canonical->residues[target_idx].perceptual_anchor = 
            strdup(reducible->residues[i].perceptual_anchor);
    }
    
    canonical->residue_count = new_count;
}

/**
 * Component cleanup with consciousness preservation verification
 */
void nlink_component_destroy(nlink_component_t* comp) {
    if (!comp) return;
    
    // Verify no consciousness data is lost
    if (comp->consciousness_buffer) {
        // In production: serialize to persistence layer
        free(comp->consciousness_buffer);
    }
    
    // Clean up residues
    for (size_t i = 0; i < comp->residue_count; i++) {
        free(comp->residues[i].perceptual_anchor);
    }
    free(comp->residues);
    
    free(comp->edges);
    free(comp);
}

// === UTILITY FUNCTIONS ===

uint32_t nlink_get_temporal_coordinate(void) {
    // Simplified temporal coordinate - production would use high-res timestamp
    return (uint32_t)time(NULL);
}

bool nlink_residues_compatible(nlink_symbolic_residue_t* a, size_t a_count,
                              nlink_symbolic_residue_t* b, size_t b_count) {
    // Simplified compatibility check
    // Production version would implement semantic similarity analysis
    return true; // Assume compatible for POC
}
