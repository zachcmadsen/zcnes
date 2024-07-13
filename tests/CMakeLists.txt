include(FetchContent)

FetchContent_Declare(
  zcnes-tests
  URL https://github.com/zachcmadsen/zcnes-tests/archive/refs/tags/v0.1.zip
)
message(STATUS "Fetching zcnes-tests...")
FetchContent_MakeAvailable(zcnes-tests)

add_subdirectory(common)
add_subdirectory(core)

if (ZCNES_BUILD_SINGLE_STEP_TESTS)
  add_subdirectory(cpu)
endif()