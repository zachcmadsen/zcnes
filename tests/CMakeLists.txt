include(FetchContent)

FetchContent_Declare(
  zcnes-tests
  URL https://github.com/zachcmadsen/zcnes-tests/archive/refs/tags/v0.1.zip
)
message(STATUS "Fetching zcnes-tests...")
FetchContent_MakeAvailable(zcnes-tests)

add_executable(blargg)
target_sources(blargg PRIVATE blargg.cpp)
target_include_directories(blargg PRIVATE .)
target_link_libraries(blargg PRIVATE core)
target_compile_features(blargg PRIVATE cxx_std_20)

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

if (ZCNES_BUILD_SINGLE_STEP_TESTS)
  FetchContent_Declare(
    zcnes-single-step-tests
    URL https://github.com/zachcmadsen/zcnes-single-step-tests/archive/refs/tags/v0.1.zip
  )
  message(STATUS "Fetching zcnes-single-step-tests...")
  FetchContent_MakeAvailable(zcnes-single-step-tests)

  FetchContent_Declare(
      cista
      GIT_REPOSITORY https://github.com/felixguendling/cista
      GIT_TAG v0.15
  )
  message(STATUS "Fetching cista...")
  FetchContent_MakeAvailable(cista)

  add_executable(single-step)
  target_sources(single-step PRIVATE single_step.cpp)
  target_include_directories(single-step PRIVATE .)
  target_link_libraries(single-step PRIVATE core cista)
  target_compile_features(single-step PRIVATE cxx_std_20)

  # Single step tests are too slow without some optimizations.
  target_compile_options(single-step PRIVATE "$<$<CONFIG:Debug>:-O1>")

  # Include core's private include directories to test cpu.hpp.
  get_target_property (CORE_PRIVATE_INCLUDE_DIRECTORIES core INCLUDE_DIRECTORIES)
  target_include_directories(single-step PRIVATE ${CORE_PRIVATE_INCLUDE_DIRECTORIES})

  set(OPCODES_TO_SKIP "02" "12" "22" "32" "42" "52" "62" "72" "92" "b2" "d2" "f2" "8b" "ab")
  file(GLOB SINGLE_STEP_FILES "${zcnes-single-step-tests_SOURCE_DIR}/single_step/*.cista")
  foreach(SINGLE_STEP_FILE ${SINGLE_STEP_FILES})
    cmake_path(GET SINGLE_STEP_FILE STEM OPCODE)
    if (NOT "${OPCODE}" IN_LIST OPCODES_TO_SKIP)
      add_test("single-step.${OPCODE}" single-step ${SINGLE_STEP_FILE})
    endif()
  endforeach()
endif()