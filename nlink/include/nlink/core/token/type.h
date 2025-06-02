/**
 * @file type.h
 * @brief Type system for NexusLink
 * @copyright Copyright © 2025 OBINexus Computing
 * 
 * This module defines the core type system used throughout NexusLink.
 * It provides a foundation for type-based operations using tactical patterns.
 */

#ifndef NLINK_TYPE_H
#define NLINK_TYPE_H

#include <stddef.h>
#include <stdint.h>
#include "../tactic/tactic.h"

/**
 * NexusLink type identifier
 */
typedef uint32_t nlink_type_id;

/**
 * Basic type categories
 */
typedef enum {
    NLINK_TYPE_PRIMITIVE,
    NLINK_TYPE_COMPOSITE,
    NLINK_TYPE_FUNCTION,
    NLINK_TYPE_REFERENCE,
    NLINK_TYPE_CONTAINER,
    NLINK_TYPE_SPECIAL
} nlink_type_category;

/**
 * Core type descriptor
 */
typedef struct {c
    nlink_type_id id;
    const char* name;
    size_t size;
    nlink_type_category category;
    void* metadata;
} nlink_type;

/**
 * Type operations interface
 */
typedef struct {
    nlink_transform_fn clone;
    nlink_consumer_fn destroy;
    nlink_transform_fn serialize;
    nlink_transform_fn deserialize;
    nlink_transform_fn transform;
} nlink_type_ops;

/**
 * Register a new type
 * @param name Type name
 * @param size Size in bytes
 * @param category Type category
 * @param ops Type operations
 * @return Type identifier
 */
nlink_type_id nlink_register_type(const char* name, size_t size, 
                                  nlink_type_category category, nlink_type_ops* ops);

/**
 * Get type information by ID
 * @param id Type identifier
 * @return Type descriptor or NULL if not found
 */
const nlink_type* nlink_get_type(nlink_type_id id);

/**
 * Get type information by name
 * @param name Type name
 * @return Type descriptor or NULL if not found
 */
const nlink_type* nlink_get_type_by_name(const char* name);

/**
 * Check if a value matches a type
 * @param value Value to check
 * @param type Type to match against
 * @return true if value matches the type
 */
bool nlink_is_type(void* value, nlink_type_id type);

/**
 * Transform a value according to its type operations
 * @param value Value to transform
 * @param type Type of the value
 * @param context Transformation context
 * @return Transformed value
 */
void* nlink_transform_typed(void* value, nlink_type_id type, void* context);

#endif /* NLINK_TYPE_H */
