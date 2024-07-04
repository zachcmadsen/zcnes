cmake_minimum_required(VERSION 3.27)

project(zcnes)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(src/core)
add_subdirectory(src/cpu)

include(CTest)
enable_testing()
add_subdirectory(tests)