include(FetchContent)

FetchContent_Declare(
    cista
    GIT_REPOSITORY https://github.com/felixguendling/cista
    GIT_TAG v0.15
)
message(STATUS "Fetching cista...")
FetchContent_MakeAvailable(cista)

add_executable(single-step single_step.cpp)
target_link_libraries(single-step PRIVATE cpu cista)
# single-step tests are too slow without some optimizations.
target_compile_options(single-step PRIVATE "$<$<CONFIG:Debug>:-O1>")

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
    add_test("single-step.${TRIMMED_HEX_OPCODE}" single-step "${CMAKE_SOURCE_DIR}/../zcnes-tests/ProcessorTests" ${OPCODE})
  endif()
endforeach()