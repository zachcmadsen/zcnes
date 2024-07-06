include(FetchContent)

include(FetchContent)
FetchContent_Declare(
  zcnes-tests
  URL https://github.com/zachcmadsen/zcnes-tests/archive/refs/tags/v0.2.zip
)
message(STATUS "Fetching zcnes-tests...")
FetchContent_MakeAvailable(zcnes-tests)

add_subdirectory(common)
add_subdirectory(core)
add_subdirectory(cpu)