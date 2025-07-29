# TargetGuards.cmake - Prevent target duplication
include_guard(GLOBAL)

# Global registry for custom targets
set_property(GLOBAL PROPERTY NLINK_CUSTOM_TARGETS "")

function(nlink_add_guarded_target TARGET_NAME)
    # Check if target already exists
    get_property(EXISTING_TARGETS GLOBAL PROPERTY NLINK_CUSTOM_TARGETS)
    
    if(TARGET ${TARGET_NAME} OR "${TARGET_NAME}" IN_LIST EXISTING_TARGETS)
        message(STATUS "Target '${TARGET_NAME}' already exists - skipping")
        return()
    endif()
    
    # Parse remaining arguments as add_custom_target parameters
    add_custom_target(${TARGET_NAME} ${ARGN})
    
    # Register target globally
    set_property(GLOBAL APPEND PROPERTY NLINK_CUSTOM_TARGETS ${TARGET_NAME})
endfunction()