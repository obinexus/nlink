# Test directory CMakeLists.txt

if(BUILD_TESTING)
    # Unit tests
    add_subdirectory(unit)
    
    # Integration tests
    add_subdirectory(integration)
    
    # QA Validation executable
    add_executable(qa_validator qa_validator.c)
    target_link_libraries(qa_validator nlink)
    
    # Register tests
    add_test(NAME qa_soundness COMMAND qa_validator)
endif()
