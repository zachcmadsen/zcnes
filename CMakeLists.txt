cmake_minimum_required(VERSION 3.27)

project(
    zcnes
    LANGUAGES C CXX
)

# TODO: Remove this option once the frontend is functional.
option(ZCNES_BUILD_FRONTEND "Build ZCNES frontend" OFF)

add_subdirectory(src/core)

if (ZCNES_BUILD_FRONTEND)
    add_subdirectory(src/frontend)
endif()

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    option(ZCNES_BUILD_TESTS "Build ZCNES tests" ON)
    # The single step tests are disabled by default because they download a
    # large file.
    option(ZCNES_BUILD_SINGLE_STEP_TESTS "Build ZCNES single step tests" OFF)

    if (ZCNES_BUILD_TESTS)
        include(CTest)
        enable_testing()
        add_subdirectory(tests)
    endif()
endif()