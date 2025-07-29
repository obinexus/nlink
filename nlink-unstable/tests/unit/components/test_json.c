/**
 * @file test_json.c
 * @brief Unit tests for JSON handling
 * 
 * Tests the JSON parsing and manipulation functionality.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nexus_json.h"
 #include "../common/test_common.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 
 void test_json_creation() {
     // Test creating JSON values
     NexusJsonValue* null_val = nexus_json_null();
     test_assert("Create null value", null_val != NULL && null_val->type == NEXUS_JSON_NULL);
     
     NexusJsonValue* bool_val = nexus_json_bool(true);
     test_assert("Create boolean value", 
                 bool_val != NULL && 
                 bool_val->type == NEXUS_JSON_BOOL && 
                 bool_val->data.boolean == true);
     
     NexusJsonValue* num_val = nexus_json_number(42.5);
     test_assert("Create number value", 
                 num_val != NULL && 
                 num_val->type == NEXUS_JSON_NUMBER && 
                 num_val->data.number == 42.5);
     
     NexusJsonValue* str_val = nexus_json_string("test string");
     test_assert("Create string value", 
                 str_val != NULL && 
                 str_val->type == NEXUS_JSON_STRING && 
                 strcmp(str_val->data.string, "test string") == 0);
     
     NexusJsonValue* arr_val = nexus_json_array();
     test_assert("Create array value", 
                 arr_val != NULL && 
                 arr_val->type == NEXUS_JSON_ARRAY && 
                 arr_val->data.array.count == 0);
     
     NexusJsonValue* obj_val = nexus_json_object();
     test_assert("Create object value", 
                 obj_val != NULL && 
                 obj_val->type == NEXUS_JSON_OBJECT && 
                 obj_val->data.object.count == 0);
     
     // Clean up
     nexus_json_free(null_val);
     nexus_json_free(bool_val);
     nexus_json_free(num_val);
     nexus_json_free(str_val);
     nexus_json_free(arr_val);
     nexus_json_free(obj_val);
 }
 
 void test_json_array_operations() {
     // Create array and add items
     NexusJsonValue* array = nexus_json_array();
     
     nexus_json_array_add(array, nexus_json_number(1));
     nexus_json_array_add(array, nexus_json_string("two"));
     nexus_json_array_add(array, nexus_json_bool(true));
     
     test_assert("Array size after adding items", array->data.array.count == 3);
     test_assert("Array item 0 type", array->data.array.items[0]->type == NEXUS_JSON_NUMBER);
     test_assert("Array item 1 type", array->data.array.items[1]->type == NEXUS_JSON_STRING);
     test_assert("Array item 2 type", array->data.array.items[2]->type == NEXUS_JSON_BOOL);
     
     test_assert("Array item 0 value", array->data.array.items[0]->data.number == 1);
     test_assert("Array item 1 value", strcmp(array->data.array.items[1]->data.string, "two") == 0);
     test_assert("Array item 2 value", array->data.array.items[2]->data.boolean == true);
     
     // Clean up
     nexus_json_free(array);
 }
 
 void test_json_object_operations() {
     // Create object and add properties
     NexusJsonValue* object = nexus_json_object();
     
     nexus_json_object_add(object, "number", nexus_json_number(42));
     nexus_json_object_add(object, "string", nexus_json_string("value"));
     nexus_json_object_add(object, "boolean", nexus_json_bool(false));
     nexus_json_object_add(object, "null", nexus_json_null());
     
     test_assert("Object size after adding properties", object->data.object.count == 4);
     
     // Get properties by key
     NexusJsonValue* num_prop = nexus_json_object_get(object, "number");
     test_assert("Get number property", 
                 num_prop != NULL && 
                 num_prop->type == NEXUS_JSON_NUMBER && 
                 num_prop->data.number == 42);
     
     NexusJsonValue* str_prop = nexus_json_object_get(object, "string");
     test_assert("Get string property", 
                 str_prop != NULL && 
                 str_prop->type == NEXUS_JSON_STRING && 
                 strcmp(str_prop->data.string, "value") == 0);
     
     NexusJsonValue* bool_prop = nexus_json_object_get(object, "boolean");
     test_assert("Get boolean property", 
                 bool_prop != NULL && 
                 bool_prop->type == NEXUS_JSON_BOOL && 
                 bool_prop->data.boolean == false);
     
     NexusJsonValue* null_prop = nexus_json_object_get(object, "null");
     test_assert("Get null property", 
                 null_prop != NULL && 
                 null_prop->type == NEXUS_JSON_NULL);
     
     // Get non-existent property
     NexusJsonValue* missing_prop = nexus_json_object_get(object, "nonexistent");
     test_assert("Get non-existent property", missing_prop == NULL);
     
     // Helper functions
     const char* str_val = nexus_json_object_get_string(object, "string", "default");
     test_assert("Get string value", strcmp(str_val, "value") == 0);
     
     const char* missing_str = nexus_json_object_get_string(object, "nonexistent", "default");
     test_assert("Get missing string value with default", strcmp(missing_str, "default") == 0);
     
     double num_val = nexus_json_object_get_number(object, "number", 0);
     test_assert("Get number value", num_val == 42);
     
     double missing_num = nexus_json_object_get_number(object, "nonexistent", 99.9);
     test_assert("Get missing number value with default", missing_num == 99.9);
     
     bool bool_val = nexus_json_object_get_bool(object, "boolean", true);
     test_assert("Get boolean value", bool_val == false);
     
     bool missing_bool = nexus_json_object_get_bool(object, "nonexistent", true);
     test_assert("Get missing boolean value with default", missing_bool == true);
     
     // Clean up
     nexus_json_free(object);
 }
 
 void test_json_parsing() {
     // Test parsing simple values
     NexusJsonValue* null_val = nexus_json_parse("null");
     test_assert("Parse null", null_val != NULL && null_val->type == NEXUS_JSON_NULL);
     nexus_json_free(null_val);
     
     NexusJsonValue* true_val = nexus_json_parse("true");
     test_assert("Parse true", 
                 true_val != NULL && 
                 true_val->type == NEXUS_JSON_BOOL && 
                 true_val->data.boolean == true);
     nexus_json_free(true_val);
     
     NexusJsonValue* false_val = nexus_json_parse("false");
     test_assert("Parse false", 
                 false_val != NULL && 
                 false_val->type == NEXUS_JSON_BOOL && 
                 false_val->data.boolean == false);
     nexus_json_free(false_val);
     
     NexusJsonValue* num_val = nexus_json_parse("42.5");
     test_assert("Parse number", 
                 num_val != NULL && 
                 num_val->type == NEXUS_JSON_NUMBER && 
                 num_val->data.number == 42.5);
     nexus_json_free(num_val);
     
     NexusJsonValue* str_val = nexus_json_parse("\"test string\"");
     test_assert("Parse string", 
                 str_val != NULL && 
                 str_val->type == NEXUS_JSON_STRING && 
                 strcmp(str_val->data.string, "test string") == 0);
     nexus_json_free(str_val);
     
     // Test parsing array
     NexusJsonValue* arr_val = nexus_json_parse("[1, \"two\", true]");
     test_assert("Parse array", 
                 arr_val != NULL && 
                 arr_val->type == NEXUS_JSON_ARRAY && 
                 arr_val->data.array.count == 3 &&
                 arr_val->data.array.items[0]->type == NEXUS_JSON_NUMBER &&
                 arr_val->data.array.items[1]->type == NEXUS_JSON_STRING &&
                 arr_val->data.array.items[2]->type == NEXUS_JSON_BOOL);
     nexus_json_free(arr_val);
     
     // Test parsing object
     NexusJsonValue* obj_val = nexus_json_parse("{\"number\": 42, \"string\": \"value\", \"boolean\": false}");
     test_assert("Parse object", 
                 obj_val != NULL && 
                 obj_val->type == NEXUS_JSON_OBJECT && 
                 obj_val->data.object.count == 3);
     
     if (obj_val) {
         NexusJsonValue* num_prop = nexus_json_object_get(obj_val, "number");
         test_assert("Parsed object property 'number'", 
                     num_prop != NULL && num_prop->type == NEXUS_JSON_NUMBER);
         
         NexusJsonValue* str_prop = nexus_json_object_get(obj_val, "string");
         test_assert("Parsed object property 'string'", 
                     str_prop != NULL && str_prop->type == NEXUS_JSON_STRING);
         
         NexusJsonValue* bool_prop = nexus_json_object_get(obj_val, "boolean");
         test_assert("Parsed object property 'boolean'", 
                     bool_prop != NULL && bool_prop->type == NEXUS_JSON_BOOL);
     }
     
     nexus_json_free(obj_val);
     
     // Test parsing nested structure
     const char* nested_json = "{"
                               "  \"name\": \"test_component\","
                               "  \"version\": \"1.0.0\","
                               "  \"dependencies\": ["
                               "    {\"id\": \"dep1\", \"version\": \"^1.0.0\"},"
                               "    {\"id\": \"dep2\", \"version\": \"~2.0.0\"}"
                               "  ]"
                               "}";
     
     NexusJsonValue* nested_val = nexus_json_parse(nested_json);
     test_assert("Parse nested structure", nested_val != NULL && nested_val->type == NEXUS_JSON_OBJECT);
     
     if (nested_val) {
         const char* name = nexus_json_object_get_string(nested_val, "name", NULL);
         test_assert("Get nested name", name != NULL && strcmp(name, "test_component") == 0);
         
         const char* version = nexus_json_object_get_string(nested_val, "version", NULL);
         test_assert("Get nested version", version != NULL && strcmp(version, "1.0.0") == 0);
         
         NexusJsonValue* deps = nexus_json_object_get(nested_val, "dependencies");
         test_assert("Get nested dependencies", 
                     deps != NULL && 
                     deps->type == NEXUS_JSON_ARRAY && 
                     deps->data.array.count == 2);
         
         if (deps && deps->type == NEXUS_JSON_ARRAY && deps->data.array.count >= 2) {
             NexusJsonValue* dep1 = deps->data.array.items[0];
             test_assert("First dependency is object", 
                         dep1 != NULL && dep1->type == NEXUS_JSON_OBJECT);
             
             if (dep1 && dep1->type == NEXUS_JSON_OBJECT) {
                 const char* id = nexus_json_object_get_string(dep1, "id", NULL);
                 test_assert("First dependency id", id != NULL && strcmp(id, "dep1") == 0);
                 
                 const char* ver = nexus_json_object_get_string(dep1, "version", NULL);
                 test_assert("First dependency version", ver != NULL && strcmp(ver, "^1.0.0") == 0);
             }
         }
     }
     
     nexus_json_free(nested_val);
 }
 
 void test_json_file_operations() {
     // Create a JSON object
     NexusJsonValue* obj = nexus_json_object();
     nexus_json_object_add(obj, "name", nexus_json_string("test_component"));
     nexus_json_object_add(obj, "version", nexus_json_string("1.0.0"));
     
     NexusJsonValue* deps = nexus_json_array();
     
     NexusJsonValue* dep1 = nexus_json_object();
     nexus_json_object_add(dep1, "id", nexus_json_string("dep1"));
     nexus_json_object_add(dep1, "version", nexus_json_string("^1.0.0"));
     nexus_json_array_add(deps, dep1);
     
     NexusJsonValue* dep2 = nexus_json_object();
     nexus_json_object_add(dep2, "id", nexus_json_string("dep2"));
     nexus_json_object_add(dep2, "version", nexus_json_string("~2.0.0"));
     nexus_json_array_add(deps, dep2);
     
     nexus_json_object_add(obj, "dependencies", deps);
     
     // Write to a temporary file
     char* filename = create_temp_file(NULL);
     test_assert("Create temporary file", filename != NULL);
     
     if (filename) {
         // Write the file
         bool write_result = nexus_json_write_file(obj, filename, true);
         test_assert("Write JSON to file", write_result);
         
         // Read the file back
         NexusJsonValue* read_obj = nexus_json_parse_file(filename);
         test_assert("Read JSON from file", read_obj != NULL && read_obj->type == NEXUS_JSON_OBJECT);
         
         if (read_obj && read_obj->type == NEXUS_JSON_OBJECT) {
             // Verify contents
             const char* name = nexus_json_object_get_string(read_obj, "name", NULL);
             test_assert("Read name property", name != NULL && strcmp(name, "test_component") == 0);
             
             const char* version = nexus_json_object_get_string(read_obj, "version", NULL);
             test_assert("Read version property", version != NULL && strcmp(version, "1.0.0") == 0);
             
             NexusJsonValue* read_deps = nexus_json_object_get(read_obj, "dependencies");
             test_assert("Read dependencies property", 
                         read_deps != NULL && 
                         read_deps->type == NEXUS_JSON_ARRAY && 
                         read_deps->data.array.count == 2);
         }
         
         // Clean up
         nexus_json_free(read_obj);
         unlink(filename);  // Delete the temporary file
         free(filename);
     }
     
     nexus_json_free(obj);
 }
 
 int main() {
     test_init("JSON Handling");
     
     test_json_creation();
     test_json_array_operations();
     test_json_object_operations();
     test_json_parsing();
     test_json_file_operations();
     
     return test_finalize();
 }