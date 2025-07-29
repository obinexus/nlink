# Testing configuration with QA principles

option(BUILD_TESTING "Build tests" ON)

if(BUILD_TESTING)
    enable_testing()
    
    # QA Testing Matrix
    # TP: True Positive - Correctly identified issues
    # TN: True Negative - Correctly passed valid cases  
    # FP: False Positive - Incorrectly flagged valid cases (MUST BE ZERO)
    # FN: False Negative - Missed actual issues
    
    set(QA_TEST_CATEGORIES
        "unit"
        "integration"
        "soundness"
        "performance"
    )
    
    # Test compilation flags
    set(TEST_COMPILE_FLAGS "${COMMON_FLAGS} -DTESTING")
    
    # Create test targets
    function(add_qa_test test_name test_source)
        add_executable(${test_name} ${test_source})
        target_link_libraries(${test_name} nlink)
        target_compile_options(${test_name} PRIVATE ${TEST_COMPILE_FLAGS})
        add_test(NAME ${test_name} COMMAND ${test_name})
        
        # Add to CTest categories
        set_property(TEST ${test_name} PROPERTY LABELS ${ARGN})
    endfunction()
endif()
