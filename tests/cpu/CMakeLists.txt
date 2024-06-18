include(FetchContent)
include(${PROJECT_SOURCE_DIR}/cmake/zcnes.cmake)

FetchContent_Declare(
    reflect-cpp
    GIT_REPOSITORY https://github.com/getml/reflect-cpp
    GIT_TAG v0.11.1
    SYSTEM
)
message(STATUS "Fetching reflect-cpp...")
FetchContent_MakeAvailable(reflect-cpp)

add_executable(cpu-test json.cpp main.cpp processor.cpp)

target_include_directories(cpu-test PRIVATE .)

target_link_libraries(cpu-test PRIVATE cpu doctest::doctest fmt::fmt reflectcpp)

target_compile_definitions(cpu-test PRIVATE _ZCNES_TESTS_PATH="${CMAKE_SOURCE_DIR}/../zcnes-tests")

zcnes_compile_options(cpu-test)

include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
doctest_discover_tests(cpu-test)