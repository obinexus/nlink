# NexusLink CLI Component Unit Tests

if(NLINK_USE_CLI)
    set(CLI_TEST_SOURCES
        test_cli_parser.c
        test_command_router.c
        test_command_registry.c
    )
    
    foreach(test_source ${CLI_TEST_SOURCES})
        get_filename_component(test_name ${test_source} NAME_WE)
        
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${test_source}")
            add_executable(${test_name} ${test_source})
            
            target_link_libraries(${test_name}
                PRIVATE
                    nlink_cli_static
                    nlink_test_framework
            )
            
            add_test(
                NAME cli_${test_name}
                COMMAND ${test_name}
            )
        endif()
    endforeach()
endif()
