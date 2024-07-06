include(FetchContent)

FetchContent_Declare(
    cista
    GIT_REPOSITORY https://github.com/felixguendling/cista
    GIT_TAG v0.15
)
message(STATUS "Fetching cista...")
FetchContent_MakeAvailable(cista)

add_executable(single-step)
target_sources(single-step PRIVATE single_step.cpp)
target_link_libraries(single-step PRIVATE cista common cpu)
# Single step tests are too slow without some optimizations.
target_compile_options(single-step PRIVATE "$<$<CONFIG:Debug>:-O1>")

set(OPCODES_TO_SKIP "02" "12" "22" "32" "42" "52" "62" "72" "92" "b2" "d2" "f2" "8b" "ab")
file(GLOB SINGLE_STEP_FILES "${zcnes-tests_SOURCE_DIR}/single_step/*.cista")
foreach(SINGLE_STEP_FILE ${SINGLE_STEP_FILES})
  cmake_path(GET SINGLE_STEP_FILE STEM OPCODE)
  if (NOT "${OPCODE}" IN_LIST OPCODES_TO_SKIP)
    add_test("single-step.${OPCODE}" single-step ${SINGLE_STEP_FILE})
  endif()
endforeach()