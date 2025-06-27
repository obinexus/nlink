// src/nexus_json.c - Implementation of minimal JSON parser for NexusLink
#include "../include/nexus_json.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

// Create a JSON null value
NexusJsonValue* nexus_json_null(void) {
    NexusJsonValue* value = (NexusJsonValue*)malloc(sizeof(NexusJsonValue));
    value->type = NEXUS_JSON_NULL;
    return value;
}

// Create a JSON boolean value
NexusJsonValue* nexus_json_bool(bool b) {
    NexusJsonValue* value = (NexusJsonValue*)malloc(sizeof(NexusJsonValue));
    value->type = NEXUS_JSON_BOOL;
    value->data.boolean = b;
    return value;
}

// Create a JSON number value
NexusJsonValue* nexus_json_number(double n) {
    NexusJsonValue* value = (NexusJsonValue*)malloc(sizeof(NexusJsonValue));
    value->type = NEXUS_JSON_NUMBER;
    value->data.number = n;
    return value;
}

// Create a JSON string value
NexusJsonValue* nexus_json_string(const char* s) {
    NexusJsonValue* value = (NexusJsonValue*)malloc(sizeof(NexusJsonValue));
    value->type = NEXUS_JSON_STRING;
    value->data.string = strdup(s);
    return value;
}

// Create a JSON array value
NexusJsonValue* nexus_json_array(void) {
    NexusJsonValue* value = (NexusJsonValue*)malloc(sizeof(NexusJsonValue));
    value->type = NEXUS_JSON_ARRAY;
    value->data.array.items = NULL;
    value->data.array.count = 0;
    return value;
}

// Add an item to a JSON array
void nexus_json_array_add(NexusJsonValue* array, NexusJsonValue* item) {
    if (array->type != NEXUS_JSON_ARRAY) return;
    
    array->data.array.count++;
    array->data.array.items = (NexusJsonValue**)realloc(
        array->data.array.items, 
        array->data.array.count * sizeof(NexusJsonValue*)
    );
    
    array->data.array.items[array->data.array.count - 1] = item;
}

// Create a JSON object value
NexusJsonValue* nexus_json_object(void) {
    NexusJsonValue* value = (NexusJsonValue*)malloc(sizeof(NexusJsonValue));
    value->type = NEXUS_JSON_OBJECT;
    value->data.object.keys = NULL;
    value->data.object.values = NULL;
    value->data.object.count = 0;
    return value;
}

// Add a key-value pair to a JSON object
void nexus_json_object_add(NexusJsonValue* object, const char* key, NexusJsonValue* value) {
    if (object->type != NEXUS_JSON_OBJECT) return;
    
    object->data.object.count++;
    object->data.object.keys = (char**)realloc(
        object->data.object.keys, 
        object->data.object.count * sizeof(char*)
    );
    object->data.object.values = (NexusJsonValue**)realloc(
        object->data.object.values, 
        object->data.object.count * sizeof(NexusJsonValue*)
    );
    
    object->data.object.keys[object->data.object.count - 1] = strdup(key);
    object->data.object.values[object->data.object.count - 1] = value;
}

// Get a value from a JSON object by key
NexusJsonValue* nexus_json_object_get(NexusJsonValue* object, const char* key) {
    if (object->type != NEXUS_JSON_OBJECT) return NULL;
    
    for (size_t i = 0; i < object->data.object.count; i++) {
        if (strcmp(object->data.object.keys[i], key) == 0) {
            return object->data.object.values[i];
        }
    }
    
    return NULL;
}

// Get a string from a JSON object by key
const char* nexus_json_object_get_string(NexusJsonValue* object, const char* key, const char* default_value) {
    NexusJsonValue* value = nexus_json_object_get(object, key);
    if (value && value->type == NEXUS_JSON_STRING) {
        return value->data.string;
    }
    return default_value;
}

// Get a number from a JSON object by key
double nexus_json_object_get_number(NexusJsonValue* object, const char* key, double default_value) {
    NexusJsonValue* value = nexus_json_object_get(object, key);
    if (value && value->type == NEXUS_JSON_NUMBER) {
        return value->data.number;
    }
    return default_value;
}

// Get a boolean from a JSON object by key
bool nexus_json_object_get_bool(NexusJsonValue* object, const char* key, bool default_value) {
    NexusJsonValue* value = nexus_json_object_get(object, key);
    if (value && value->type == NEXUS_JSON_BOOL) {
        return value->data.boolean;
    }
    return default_value;
}

// Free a JSON value and all its children
void nexus_json_free(NexusJsonValue* value) {
    if (!value) return;
    
    switch (value->type) {
        case NEXUS_JSON_STRING:
            free(value->data.string);
            break;
        case NEXUS_JSON_ARRAY:
            for (size_t i = 0; i < value->data.array.count; i++) {
                nexus_json_free(value->data.array.items[i]);
            }
            free(value->data.array.items);
            break;
        case NEXUS_JSON_OBJECT:
            for (size_t i = 0; i < value->data.object.count; i++) {
                free(value->data.object.keys[i]);
                nexus_json_free(value->data.object.values[i]);
            }
            free(value->data.object.keys);
            free(value->data.object.values);
            break;
        default:
            break;
    }
    
    free(value);
}

// Skip whitespace in a JSON string
const char* nexus_json_skip_whitespace(const char* s) {
    while (isspace(*s)) s++;
    return s;
}

// Parse a JSON string value
const char* nexus_json_parse_string(const char* s, char** out) {
    if (*s != '"') return NULL;
    s++;
    
    // Calculate string length with escape handling
    size_t len = 0;
    const char* start = s;
    while (*s && *s != '"') {
        if (*s == '\\') {
            s++;
            if (!*s) return NULL;
        }
        s++;
        len++;
    }
    
    if (!*s) return NULL;
    
    // Allocate and copy string
    *out = (char*)malloc(len + 1);
    char* dest = *out;
    s = start;
    
    while (*s && *s != '"') {
        if (*s == '\\') {
            s++;
            switch (*s) {
                case 'n': *dest++ = '\n'; break;
                case 'r': *dest++ = '\r'; break;
                case 't': *dest++ = '\t'; break;
                case '\\': *dest++ = '\\'; break;
                case '"': *dest++ = '"'; break;
                default: *dest++ = *s; break;
            }
        } else {
            *dest++ = *s;
        }
        s++;
    }
    
    *dest = '\0';
    return s + 1; // Skip closing quote
}

// Forward declarations for mutually recursive functions implementation
const char* nexus_json_parse_value(const char* s, NexusJsonValue** out);
const char* nexus_json_parse_array(const char* s, NexusJsonValue** out);
const char* nexus_json_parse_object(const char* s, NexusJsonValue** out);

// Parse a JSON number value
const char* nexus_json_parse_number(const char* s, NexusJsonValue** out) {
    char* end;
    double number = strtod(s, &end);
    if (end == s) return NULL;
    
    *out = nexus_json_number(number);
    return end;
}

// Parse a JSON array
const char* nexus_json_parse_array(const char* s, NexusJsonValue** out) {
    if (*s != '[') return NULL;
    s++;
    
    *out = nexus_json_array();
    s = nexus_json_skip_whitespace(s);
    
    // Handle empty array
    if (*s == ']') {
        return s + 1;
    }
    
    while (1) {
        NexusJsonValue* item;
        s = nexus_json_skip_whitespace(s);
        s = nexus_json_parse_value(s, &item);
        if (!s) {
            nexus_json_free(*out);
            *out = NULL;
            return NULL;
        }
        
        nexus_json_array_add(*out, item);
        
        s = nexus_json_skip_whitespace(s);
        if (*s == ']') {
            return s + 1;
        }
        
        if (*s != ',') {
            nexus_json_free(*out);
            *out = NULL;
            return NULL;
        }
        
        s++;
    }
}

// Parse a JSON object
const char* nexus_json_parse_object(const char* s, NexusJsonValue** out) {
    if (*s != '{') return NULL;
    s++;
    
    *out = nexus_json_object();
    s = nexus_json_skip_whitespace(s);
    
    // Handle empty object
    if (*s == '}') {
        return s + 1;
    }
    
    while (1) {
        s = nexus_json_skip_whitespace(s);
        
        // Parse key
        char* key;
        s = nexus_json_parse_string(s, &key);
        if (!s) {
            nexus_json_free(*out);
            *out = NULL;
            return NULL;
        }
        
        s = nexus_json_skip_whitespace(s);
        if (*s != ':') {
            free(key);
            nexus_json_free(*out);
            *out = NULL;
            return NULL;
        }
        s++;
        
        // Parse value
        NexusJsonValue* value;
        s = nexus_json_skip_whitespace(s);
        s = nexus_json_parse_value(s, &value);
        if (!s) {
            free(key);
            nexus_json_free(*out);
            *out = NULL;
            return NULL;
        }
        
        // Add key-value pair to object
        nexus_json_object_add(*out, key, value);
        free(key);
        
        s = nexus_json_skip_whitespace(s);
        if (*s == '}') {
            return s + 1;
        }
        
        if (*s != ',') {
            nexus_json_free(*out);
            *out = NULL;
            return NULL;
        }
        
        s++;
    }
}

// Parse a JSON value
const char* nexus_json_parse_value(const char* s, NexusJsonValue** out) {
    s = nexus_json_skip_whitespace(s);
    
    switch (*s) {
        case 'n': // null
            if (strncmp(s, "null", 4) == 0) {
                *out = nexus_json_null();
                return s + 4;
            }
            return NULL;
        
        case 't': // true
            if (strncmp(s, "true", 4) == 0) {
                *out = nexus_json_bool(true);
                return s + 4;
            }
            return NULL;
        
        case 'f': // false
            if (strncmp(s, "false", 5) == 0) {
                *out = nexus_json_bool(false);
                return s + 5;
            }
            return NULL;
        
        case '"': // string
            {
                char* string_value;
                const char* next = nexus_json_parse_string(s, &string_value);
                if (!next) return NULL;
                *out = nexus_json_string(string_value);
                free(string_value);
                return next;
            }
        
        case '[': // array
            return nexus_json_parse_array(s, out);
        
        case '{': // object
            return nexus_json_parse_object(s, out);
        
        case '-': // number
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6': 
        case '7':
        case '8':
        case '9':
            return nexus_json_parse_number(s, out);
        
        default:
            return NULL;
    }
}

// Parse a JSON string into a JSON value
NexusJsonValue* nexus_json_parse(const char* json) {
    NexusJsonValue* result;
    const char* end = nexus_json_parse_value(json, &result);
    
    if (!end || *nexus_json_skip_whitespace(end) != '\0') {
        if (result) nexus_json_free(result);
        return NULL;
    }
    
    return result;
}

// Create a JSON string from a file
NexusJsonValue* nexus_json_parse_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return NULL;
    
    // Find file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // Read file content
    char* content = (char*)malloc(size + 1);
    size_t read_size = fread(content, 1, size, file);
    fclose(file);
    
    content[read_size] = '\0';
    
    // Parse JSON
    NexusJsonValue* result = nexus_json_parse(content);
    free(content);
    
    return result;
}

// Write a JSON value to a string with formatting
void nexus_json_write_to_string(const NexusJsonValue* value, char** out, size_t* length, size_t* capacity, int indent, int current_indent) {
    // Ensure buffer capacity
    if (*length + 256 > *capacity) {
        *capacity = *capacity * 2 + 256;
        *out = (char*)realloc(*out, *capacity);
    }
    
    char* buffer = *out + *length;
    size_t remaining = *capacity - *length;
    int written = 0;
    
    // Write indentation
    if (indent > 0 && current_indent > 0) {
        for (int i = 0; i < current_indent; i++) {
            buffer[written++] = ' ';
        }
    }
    
    // Write value based on type
    switch (value->type) {
        case NEXUS_JSON_NULL:
            written += snprintf(buffer + written, remaining - written, "null");
            break;
            
        case NEXUS_JSON_BOOL:
            written += snprintf(buffer + written, remaining - written, 
                             value->data.boolean ? "true" : "false");
            break;
            
        case NEXUS_JSON_NUMBER:
            // Format number intelligently - use integer format if it's an integer
            if (value->data.number == (int)value->data.number) {
                written += snprintf(buffer + written, remaining - written, "%d", (int)value->data.number);
            } else {
                written += snprintf(buffer + written, remaining - written, "%g", value->data.number);
            }
            break;
            
        case NEXUS_JSON_STRING:
            written += snprintf(buffer + written, remaining - written, "\"");
            // Escape special characters
            for (const char* s = value->data.string; *s; s++) {
                if (*length + written + 10 > *capacity) {
                    *length += written;
                    *capacity = *capacity * 2 + 256;
                    *out = (char*)realloc(*out, *capacity);
                    buffer = *out + *length;
                    remaining = *capacity - *length;
                    written = 0;
                }
                
                switch (*s) {
                    case '"': written += snprintf(buffer + written, remaining - written, "\\\""); break;
                    case '\\': written += snprintf(buffer + written, remaining - written, "\\\\"); break;
                    case '\n': written += snprintf(buffer + written, remaining - written, "\\n"); break;
                    case '\r': written += snprintf(buffer + written, remaining - written, "\\r"); break;
                    case '\t': written += snprintf(buffer + written, remaining - written, "\\t"); break;
                    default: buffer[written++] = *s; break;
                }
            }
            written += snprintf(buffer + written, remaining - written, "\"");
            break;
            
        case NEXUS_JSON_ARRAY:
            written += snprintf(buffer + written, remaining - written, "[");
            *length += written;
            
            if (value->data.array.count > 0 && indent > 0) {
                written = 0;
                written += snprintf(buffer + written, remaining - written, "\n");
                *length += written;
                
                for (size_t i = 0; i < value->data.array.count; i++) {
                    nexus_json_write_to_string(value->data.array.items[i], out, length, capacity, 
                                           indent, current_indent + indent);
                    
                    if (i < value->data.array.count - 1) {
                        buffer = *out + *length;
                        remaining = *capacity - *length;
                        written = 0;
                        written += snprintf(buffer + written, remaining - written, ",\n");
                        *length += written;
                    } else {
                        buffer = *out + *length;
                        remaining = *capacity - *length;
                        written = 0;
                        written += snprintf(buffer + written, remaining - written, "\n");
                        *length += written;
                    }
                }
                
                buffer = *out + *length;
                remaining = *capacity - *length;
                written = 0;
                
                for (int i = 0; i < current_indent; i++) {
                    buffer[written++] = ' ';
                }
                
                written += snprintf(buffer + written, remaining - written, "]");
            } else {
                for (size_t i = 0; i < value->data.array.count; i++) {
                    buffer = *out + *length;
                    remaining = *capacity - *length;
                    written = 0;
                    
                    nexus_json_write_to_string(value->data.array.items[i], out, length, capacity, 0, 0);
                    
                    if (i < value->data.array.count - 1) {
                        buffer = *out + *length;
                        remaining = *capacity - *length;
                        written = 0;
                        written += snprintf(buffer + written, remaining - written, ",");
                        *length += written;
                    }
                }
                
                buffer = *out + *length;
                remaining = *capacity - *length;
                written = 0;
                written += snprintf(buffer + written, remaining - written, "]");
            }
            
            break;
            
        case NEXUS_JSON_OBJECT:
            written += snprintf(buffer + written, remaining - written, "{");
            *length += written;
            
            if (value->data.object.count > 0 && indent > 0) {
                written = 0;
                written += snprintf(buffer + written, remaining - written, "\n");
                *length += written;
                
                for (size_t i = 0; i < value->data.object.count; i++) {
                    // Write key with indentation
                    buffer = *out + *length;
                    remaining = *capacity - *length;
                    written = 0;
                    
                    for (int j = 0; j < current_indent + indent; j++) {
                        buffer[written++] = ' ';
                    }
                    
                    written += snprintf(buffer + written, remaining - written, "\"%s\": ", 
                                     value->data.object.keys[i]);
                    *length += written;
                    
                    // Write value
                    nexus_json_write_to_string(value->data.object.values[i], out, length, capacity, 
                                           indent, current_indent + indent);
                    
                    if (i < value->data.object.count - 1) {
                        buffer = *out + *length;
                        remaining = *capacity - *length;
                        written = 0;
                        written += snprintf(buffer + written, remaining - written, ",\n");
                        *length += written;
                    } else {
                        buffer = *out + *length;
                        remaining = *capacity - *length;
                        written = 0;
                        written += snprintf(buffer + written, remaining - written, "\n");
                        *length += written;
                    }
                }
                
                buffer = *out + *length;
                remaining = *capacity - *length;
                written = 0;
                
                for (int i = 0; i < current_indent; i++) {
                    buffer[written++] = ' ';
                }
                
                written += snprintf(buffer + written, remaining - written, "}");
            } else {
                for (size_t i = 0; i < value->data.object.count; i++) {
                    buffer = *out + *length;
                    remaining = *capacity - *length;
                    written = 0;
                    
                    written += snprintf(buffer + written, remaining - written, "\"%s\":", 
                                     value->data.object.keys[i]);
                    *length += written;
                    
                    nexus_json_write_to_string(value->data.object.values[i], out, length, capacity, 0, 0);
                    
                    if (i < value->data.object.count - 1) {
                        buffer = *out + *length;
                        remaining = *capacity - *length;
                        written = 0;
                        written += snprintf(buffer + written, remaining - written, ",");
                        *length += written;
                    }
                }
                
                buffer = *out + *length;
                remaining = *capacity - *length;
                written = 0;
                written += snprintf(buffer + written, remaining - written, "}");
            }
            
            break;
    }
    
    *length += written;
}

// Convert a JSON value to a string
char* nexus_json_to_string(const NexusJsonValue* value, bool pretty) {
    size_t length = 0;
    size_t capacity = 256;
    char* buffer = (char*)malloc(capacity);
    
    nexus_json_write_to_string(value, &buffer, &length, &capacity, pretty ? 2 : 0, 0);
    
    // Ensure null termination
    if (length + 1 > capacity) {
        capacity = length + 1;
        buffer = (char*)realloc(buffer, capacity);
    }
    
    buffer[length] = '\0';
    
    return buffer;
}

// Write a JSON value to a file
bool nexus_json_write_file(const NexusJsonValue* value, const char* filename, bool pretty) {
    char* json = nexus_json_to_string(value, pretty);
    
    FILE* file = fopen(filename, "w");
    if (!file) {
        free(json);
        return false;
    }
    
    fprintf(file, "%s", json);
    fclose(file);
    
    free(json);
    return true;
}