include(FetchContent)

include(FetchContent)
FetchContent_Declare(
  zcnes-tests
  URL https://github.com/zachcmadsen/zcnes-tests/archive/refs/tags/v0.1.zip
)
message(STATUS "Fetching zcnes-tests")
FetchContent_MakeAvailable(zcnes-tests)

FetchContent_Declare(
    cista
    GIT_REPOSITORY https://github.com/felixguendling/cista
    GIT_TAG v0.15
)
message(STATUS "Fetching cista...")
FetchContent_MakeAvailable(cista)

add_executable(single-step single_step.cpp)
target_include_directories(single-step PRIVATE .)
target_link_libraries(single-step PRIVATE zcnes cista)

# Single step tests are too slow without some optimizations.
target_compile_options(single-step PRIVATE "$<$<CONFIG:Debug>:-O1>")

# Use zcnes's private include directories to test the CPU implementation.
get_target_property (ZCNES_PRIVATE_INCLUDE_DIRS zcnes INCLUDE_DIRECTORIES)
target_include_directories (single-step PRIVATE ${ZCNES_PRIVATE_INCLUDE_DIRS})

set(OPCODES_TO_SKIP "02" "12" "22" "32" "42" "52" "62" "72" "92" "b2" "d2" "f2" "8b" "ab")
foreach(OPCODE RANGE 255)
  math(EXPR HEX_OPCODE ${OPCODE} OUTPUT_FORMAT HEXADECIMAL)

  # Trim '0x' from HEX_OPCODE.
  string(SUBSTRING "${HEX_OPCODE}" 2 -1 TRIMMED_HEX_OPCODE)

  # Add a leading zero to one digit numbers.
  string(LENGTH ${TRIMMED_HEX_OPCODE} TRIMMED_HEX_OPCODE_LENGTH)
  if (${TRIMMED_HEX_OPCODE_LENGTH} EQUAL 1)
    set(TRIMMED_HEX_OPCODE "0${TRIMMED_HEX_OPCODE}")
  endif()

  if (NOT "${TRIMMED_HEX_OPCODE}" IN_LIST OPCODES_TO_SKIP)
    add_test("single-step.${TRIMMED_HEX_OPCODE}" single-step "${zcnes-tests_SOURCE_DIR}/single_step" ${OPCODE})
  endif()
endforeach()

add_executable(blargg blargg.cpp)
target_include_directories(blargg PRIVATE .)
target_link_libraries(blargg PRIVATE zcnes)

file(GLOB files "${zcnes-tests_SOURCE_DIR}/instr_test-v5/*.nes")
foreach(file ${files})
  cmake_path(GET file STEM TEST_NAME)
  string(SUBSTRING "${TEST_NAME}" 3 -1 TRIMMED_TEST_NAME)
  add_test("instr_test-v5.${TRIMMED_TEST_NAME}" blargg ${file})
endforeach()