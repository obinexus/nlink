# Core component unit tests
set(CORE_TEST_SOURCES
    test_common.c
    test_symbols.c
    test_versioning.c
)

foreach(test_source ${CORE_TEST_SOURCES})
    get_filename_component(test_name ${test_source} NAME_WE)
    add_executable(${test_name} ${test_source})
    target_link_libraries(${test_name} 
        nlink_core 
        nlink_test_framework
    )
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
