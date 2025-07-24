// nlink/core/tatit/nlink_tatit_identity.h
// OBINexus Constitutional Header Guard: OBINEXUS_NLINK_CORE_TATIT_NLINK_TATIT_IDENTITY_H
#ifndef OBINEXUS_NLINK_CORE_TATIT_NLINK_TATIT_IDENTITY_H
#define OBINEXUS_NLINK_CORE_TATIT_NLINK_TATIT_IDENTITY_H

/**
 * @file identity.h
 * @brief Identity tactics for NexusLink
 * @copyright © 2025 OBINexus Computing
 *
 * This module implements the identity tactical pattern,
 * providing baseline operations that maintain state to
 * facilitate composition and chaining of operations.
 */

#ifndef NLINK_TACTIC_IDENTITY_H
#define NLINK_TACTIC_IDENTITY_H

#include <stddef.h>
#include <stdbool.h>

/**
 * @brief Identity function for any data type
 *
 * Returns the input unchanged. Useful in composition and as a default handler.
 *
 * @param data Input data
 * @param context Optional context (ignored)
 * @return Input data unchanged
 */
void* nlink_identity(void* data, void* context);

/**
 * @brief Type-safe identity function for pointers
 *
 * @param ptr Input pointer
 * @return Input pointer unchanged
 */
#define NLINK_IDENTITY_PTR(ptr) ((void*)ptr)

/**
 * @brief Identity transformer that does not modify input
 *
 * Used as a placeholder in transformation chains.
 *
 * @param data Input data
 * @param context Optional context (ignored)
 * @return Input data unchanged
 */
void* nlink_transform_identity(void* data, void* context);

/**
 * @brief Identity predicate that always returns true
 *
 * Useful as a default predicate that allows all elements through.
 *
 * @param data Input data (ignored)
 * @param context Optional context (ignored)
 * @return Always true
 */
bool nlink_predicate_true(void* data, void* context);

/**
 * @brief Identity predicate that always returns false
 *
 * Useful as a default predicate that blocks all elements.
 *
 * @param data Input data (ignored)
 * @param context Optional context (ignored)
 * @return Always false
 */
bool nlink_predicate_false(void* data, void* context);

/**
 * @brief No-operation consumer function
 *
 * Performs no operation on the input data.
 *
 * @param data Input data (ignored)
 * @param context Optional context (ignored)
 */
void nlink_consumer_noop(void* data, void* context);

/**
 * @brief Identity comparison function
 *
 * Compares pointers directly. Returns 0 if equal, negative if a < b, positive if a > b.
 *
 * @param a First pointer
 * @param b Second pointer
 * @param context Optional context (ignored)
 * @return Comparison result
 */
int nlink_compare_identity(void* a, void* b, void* context);

/**
 * @brief Identity hash function
 *
 * Returns the pointer value as a hash.
 *
 * @param data Input pointer
 * @param context Optional context (ignored)
 * @return Hash value derived from pointer
 */
size_t nlink_hash_identity(void* data, void* context);

/**
 * @brief Create a conditional identity transformer
 *
 * Returns a transformer that keeps the original value if the condition is true,
 * otherwise applies the transformation.
 *
 * @param condition Condition function
 * @param transform Transformation to apply when condition is false
 * @return New conditional identity transformer
 */
void* (*nlink_create_conditional_identity(bool (*condition)(void*, void*), 
                                        void* (*transform)(void*, void*)))
                                        (void*, void*);

/**
 * @brief Identity initializer
 *
 * Returns NULL as an initial value.
 *
 * @param context Optional context (ignored)
 * @return NULL
 */
void* nlink_initializer_null(void* context);

/**
 * @brief Identity combiner
 *
 * Returns the second value, ignoring the first.
 *
 * @param a First value (ignored)
 * @param b Second value
 * @param context Optional context (ignored)
 * @return Second value
 */
void* nlink_combiner_last(void* a, void* b, void* context);

/**
 * @brief Identity factory
 * 
 * Returns the context as the created object.
 *
 * @param context Context to return
 * @return Context
 */
void* nlink_factory_identity(void* context);


#endif // OBINEXUS_NLINK_CORE_TATIT_NLINK_TATIT_IDENTITY_H
