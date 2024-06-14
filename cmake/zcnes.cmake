# Add compile options for TARGET.
function(zcnes_compile_options TARGET)
    if (ZCNES_CXX_FLAGS)
        separate_arguments(ZCNES_CXX_FLAGS_LIST NATIVE_COMMAND ${ZCNES_CXX_FLAGS})
        target_compile_options(${TARGET} PRIVATE ${ZCNES_CXX_FLAGS_LIST})
    endif()
endfunction()