function(zcnes_compile_options TARGET)
    if (ZCNES_C_FLAGS)
        # Convert ZCNES_C_FLAGS to a list to work with target_compile_options.
        separate_arguments(ZCNES_C_FLAGS_LIST NATIVE_COMMAND ${ZCNES_C_FLAGS})
        target_compile_options(${TARGET} PRIVATE ${ZCNES_C_FLAGS_LIST})
    endif()
endfunction()