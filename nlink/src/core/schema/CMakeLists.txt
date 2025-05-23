# Schema component CMakeLists.txt

# Install schema files
install(FILES nlink_schema.json
        DESTINATION share/nlink/schema)

# Copy schema to build directory for testing
configure_file(nlink_schema.json
               ${CMAKE_BINARY_DIR}/schema/nlink_schema.json
               COPYONLY)

# Create a custom target to represent the schema
add_custom_target(nexus_schema ALL
    DEPENDS ${CMAKE_BINARY_DIR}/schema/nlink_schema.json
    SOURCES nlink_schema.json)

# Add schema validation test if JSON schema validation tool is available
find_program(JSONSCHEMA_VALIDATOR jsonschema)
if(JSONSCHEMA_VALIDATOR)
    add_test(NAME validate_nlink_schema
             COMMAND ${JSONSCHEMA_VALIDATOR} -i ${CMAKE_BINARY_DIR}/schema/nlink_schema.json
             WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()