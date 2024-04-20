# Add ZCNES compile and linker options, if they're defined.
#
# Note: The flag variables have to be converted to lists to work with
# target_compile_options and target_link_libraries.
function(zcnes_options TARGET)
    if (ZCNES_C_FLAGS)
        separate_arguments(ZCNES_C_FLAGS_LIST NATIVE_COMMAND ${ZCNES_C_FLAGS})
        target_compile_options(${TARGET} PRIVATE ${ZCNES_C_FLAGS_LIST})
    endif()

    if (ZCNES_LD_FLAGS)
        separate_arguments(ZCNES_LD_FLAGS_LIST NATIVE_COMMAND ${ZCNES_LD_FLAGS})
        target_link_libraries(${TARGET} PRIVATE ${ZCNES_LD_FLAGS_LIST})
    endif()
endfunction()
