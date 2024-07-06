add_executable(blargg)
target_sources(blargg PRIVATE blargg.cpp)
target_include_directories(blargg PRIVATE .)
target_link_libraries(blargg PRIVATE common core)

function(add_blargg_test TEST_NAME)
  file(GLOB TEST_FILES "${zcnes-tests_SOURCE_DIR}/${TEST_NAME}/*.nes")
  foreach(TEST_FILE ${TEST_FILES})
    cmake_path(GET TEST_FILE STEM TEST_FILE_STEM)
    # Trim the number prefix from the file stem, e.g., 01-basics -> basics.
    string(SUBSTRING "${TEST_FILE_STEM}" 3 -1 TRIMMED_TEST_FILE_STEM)
    add_test("${TEST_NAME}.${TRIMMED_TEST_FILE_STEM}" blargg ${TEST_FILE})
  endforeach()
endfunction()

add_blargg_test(instr_test-v5)
add_blargg_test(ppu_vbl_nmi)