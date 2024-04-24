# Add ZCNES compile and linker options, if they're defined.
#
# Note: The flag variables have to be converted to lists to work with
# target_compile_options and target_link_libraries.
function(zcnes_options TARGET)
    if (ZCNES_C_FLAGS)
        separate_arguments(ZCNES_C_FLAGS_LIST NATIVE_COMMAND ${ZCNES_C_FLAGS})
        list(APPEND ZCNES_COMPILE_OPTIONS ${ZCNES_C_FLAGS_LIST})
    endif()

    if (ZCNES_C_FLAGS_SANITIZE)
        separate_arguments(ZCNES_C_FLAGS_SANITIZE_LIST NATIVE_COMMAND ${ZCNES_C_FLAGS_SANITIZE})
        list(APPEND ZCNES_COMPILE_OPTIONS ${ZCNES_C_FLAGS_SANITIZE_LIST})
    endif()

    if (ZCNES_LINKER_FLAGS_SANITIZE)
        separate_arguments(ZCNES_LINKER_FLAGS_SANITIZE_LIST NATIVE_COMMAND ${ZCNES_LINKER_FLAGS_SANITIZE})
        list(APPEND ZCNES_LINKER_OPTIONS ${ZCNES_LINKER_FLAGS_SANITIZE_LIST})
    endif()

    target_compile_options(${TARGET} PRIVATE ${ZCNES_COMPILE_OPTIONS})
    target_link_libraries(${TARGET} PRIVATE ${ZCNES_LINKER_OPTIONS})
endfunction()
