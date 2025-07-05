#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    int true_positive;
    int true_negative;
    int false_positive;  // MUST BE ZERO
    int false_negative;
} qa_metrics_t;

bool validate_soundness(qa_metrics_t* metrics) {
    printf("QA Soundness Validation\n");
    printf("=======================\n");
    printf("True Positive:  %d\n", metrics->true_positive);
    printf("True Negative:  %d\n", metrics->true_negative);
    printf("False Positive: %d (MUST BE ZERO)\n", metrics->false_positive);
    printf("False Negative: %d\n", metrics->false_negative);
    
    if (metrics->false_positive > 0) {
        printf("\nERROR: False positives detected! Soundness violation!\n");
        return false;
    }
    
    float precision = (float)metrics->true_positive / 
                     (metrics->true_positive + metrics->false_positive);
    float recall = (float)metrics->true_positive / 
                   (metrics->true_positive + metrics->false_negative);
    
    printf("\nPrecision: %.2f%%\n", precision * 100);
    printf("Recall: %.2f%%\n", recall * 100);
    
    return true;
}

int main(void) {
    qa_metrics_t metrics = {
        .true_positive = 95,
        .true_negative = 98,
        .false_positive = 0,  // Zero tolerance
        .false_negative = 5
    };
    
    return validate_soundness(&metrics) ? 0 : 1;
}
