cmake_minimum_required(VERSION 3.27)

project(zcnes)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt
    GIT_TAG 10.2.1
)
message(STATUS "Fetching fmt...")
FetchContent_MakeAvailable(fmt)

add_subdirectory(src/zcnes)

include(CTest)
enable_testing()
add_subdirectory(tests)