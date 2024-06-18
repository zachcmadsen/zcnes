include(FetchContent)

FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest
    GIT_TAG v2.4.11
)
message(STATUS "Fetching doctest...")
FetchContent_MakeAvailable(doctest)

FetchContent_Declare(
    reflect-cpp
    GIT_REPOSITORY https://github.com/getml/reflect-cpp
    GIT_TAG v0.11.1
    SYSTEM
)
message(STATUS "Fetching reflect-cpp...")
FetchContent_MakeAvailable(reflect-cpp)

set(SOURCES main.cpp processor.cpp)

add_executable(zcnes-test)
target_sources(zcnes-test PRIVATE ${SOURCES})

# Include private include directories from zcnes to test internal classes.
get_target_property(PRIVATE_INCLUDE_DIRECTORIES zcnes INCLUDE_DIRECTORIES)
target_include_directories (zcnes-test PRIVATE ${PRIVATE_INCLUDE_DIRECTORIES})

target_link_libraries(zcnes-test PRIVATE zcnes doctest::doctest fmt::fmt reflectcpp)

target_compile_definitions(zcnes-test PRIVATE _ZCNES_TESTS_PATH="${CMAKE_SOURCE_DIR}/../zcnes-tests")

include(${PROJECT_SOURCE_DIR}/cmake/zcnes.cmake)
zcnes_compile_options(zcnes-test)

include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
doctest_discover_tests(zcnes-test)