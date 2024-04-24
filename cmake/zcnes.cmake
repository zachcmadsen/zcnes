# Add ZCNES compile and linker options, if they're defined.
#
# Note: The flag variables have to be converted to lists to work with
# target_compile_options and target_link_libraries.
function(zcnes_options TARGET)
    if (ZCNES_C_FLAGS)
        separate_arguments(ZCNES_C_FLAGS_LIST NATIVE_COMMAND ${ZCNES_C_FLAGS})
        target_compile_options(${TARGET} PRIVATE ${ZCNES_C_FLAGS_LIST})
    endif()

    if (ZCNES_C_FLAGS_SANITIZE)
        separate_arguments(ZCNES_C_FLAGS_SANITIZE_LIST NATIVE_COMMAND ${ZCNES_C_FLAGS_SANITIZE})
        target_compile_options(${TARGET} PRIVATE ${ZCNES_C_FLAGS_SANITIZE_LIST})
    endif()
    if (ZCNES_LINKER_FLAGS_SANITIZE)
        separate_arguments(ZCNES_LINKER_FLAGS_SANITIZE_LIST NATIVE_COMMAND ${ZCNES_LINKER_FLAGS_SANITIZE})
        target_link_libraries(${TARGET} PRIVATE ${ZCNES_LINKER_FLAGS_SANITIZE_LIST})
    endif()
endfunction()

function(zcnes_coverage TARGET)
    if (ZCNES_C_FLAGS_COVERAGE)
        separate_arguments(ZCNES_C_FLAGS_COVERAGE_LIST NATIVE_COMMAND ${ZCNES_C_FLAGS_COVERAGE})
        target_compile_options(${TARGET} PRIVATE ${ZCNES_C_FLAGS_COVERAGE_LIST})
    endif()
    if (ZCNES_LINKER_FLAGS_COVERAGE)
        separate_arguments(ZCNES_LINKER_FLAGS_COVERAGE_LIST NATIVE_COMMAND ${ZCNES_LINKER_FLAGS_COVERAGE})
        target_link_libraries(${TARGET} PRIVATE ${ZCNES_LINKER_FLAGS_COVERAGE_LIST})
    endif()
endfunction()
