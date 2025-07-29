# NexusLink Core Component Unit Tests

set(CORE_TEST_COMPONENTS
    common
    symbols
    versioning
    minimizer
    pipeline
    pattern
)

foreach(component ${CORE_TEST_COMPONENTS})
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test_${component}.c")
        add_executable(test_${component} test_${component}.c)
        
        target_link_libraries(test_${component}
            PRIVATE
                nlink_component_${component}
                nlink_test_framework
                Threads::Threads
        )
        
        target_include_directories(test_${component}
            PRIVATE
                ${CMAKE_SOURCE_DIR}/tests/include
                ${CMAKE_SOURCE_DIR}/src/core/${component}
        )
        
        add_test(
            NAME core_${component}
            COMMAND test_${component}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
        
        set_tests_properties(core_${component} PROPERTIES
            LABELS "core;${component}"
            TIMEOUT 30
        )
    endif()
endforeach()
