cmake_minimum_required(VERSION 3.27)

project(zcnes)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_subdirectory(ext)
add_subdirectory(src)

enable_testing()
add_subdirectory(tests)
