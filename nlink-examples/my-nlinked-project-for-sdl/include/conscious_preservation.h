// my-nlinked-project-for-sdl/include/consciousness_preservation.h
#ifndef CONSCIOUSNESS_PRESERVATION_H
#define CONSCIOUSNESS_PRESERVATION_H

#include <stdint.h>
#include <stdlib.h>

// Consciousness anchor types
typedef enum {
    TEMPORAL_ANCHOR,
    SYMBOLIC_RESIDUE,
    HUMAN_VALIDATION
} consciousness_anchor_type_t;

// QA metrics
typedef struct qa_metrics {
    float true_positive;
    float true_negative;
    float false_positive;
    float false_negative;
    
    float precision;
    float recall;
    float f1_score;
} qa_metrics_t;

// AST optimization layer
typedef struct ast_optimization_layer {
    int optimization_level;
    int preserve_consciousness;
    void* optimizer_state;
} ast_optimization_layer_t;

// Temporal anchor
typedef struct consciousness_anchor {
    char* identifier;
    void* state;
    uint64_t timestamp;
    float coherence_score;
} consciousness_anchor_t;

// Human validation engine
typedef struct hitl_validation_engine {
    int validation_required;
    int validation_performed;
    void* validation_state;
    char* validation_message;
} hitl_validation_engine_t;

// Complete resolution context
typedef struct nlink_resolution_context {
    ast_optimization_layer_t* ast_optimizer;
    consciousness_anchor_t* temporal_anchor;
    hitl_validation_engine_t* human_validation;
    qa_metrics_t* validation_metrics;
} nlink_resolution_context_t;

// Functions for consciousness preservation
consciousness_anchor_t* create_temporal_anchor(const char* identifier);
void destroy_temporal_anchor(consciousness_anchor_t* anchor);

hitl_validation_engine_t* create_human_validation_engine(void);
void destroy_human_validation_engine(hitl_validation_engine_t* engine);

qa_metrics_t* create_validation_metrics(void);
void update_validation_metrics(qa_metrics_t* metrics, int result, int expected);
void destroy_validation_metrics(qa_metrics_t* metrics);

nlink_resolution_context_t* create_resolution_context(void);
void destroy_resolution_context(nlink_resolution_context_t* context);

#endif // CONSCIOUSNESS_PRESERVATION_H
