include(FetchContent)
include(${PROJECT_SOURCE_DIR}/cmake/zcnes.cmake)
include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt
    GIT_TAG 10.2.1
)
message(STATUS "Fetching fmt...")
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
    reflect-cpp
    GIT_REPOSITORY https://github.com/getml/reflect-cpp
    GIT_TAG v0.11.1
    SYSTEM
)
message(STATUS "Fetching reflect-cpp...")
FetchContent_MakeAvailable(reflect-cpp)

set(SOURCES main.cpp processor.cpp util.cpp)

add_executable(cpu-test)
target_sources(cpu-test PRIVATE ${SOURCES})
target_include_directories(cpu-test PRIVATE .)

target_link_libraries(cpu-test PRIVATE cpu doctest::doctest fmt::fmt reflectcpp)

target_compile_definitions(cpu-test PRIVATE _ZCNES_TESTS_PATH="${CMAKE_SOURCE_DIR}/../zcnes-tests")
zcnes_compile_options(cpu-test)

doctest_discover_tests(cpu-test)