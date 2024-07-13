cmake_minimum_required(VERSION 3.27)

project(zcnes)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CMakeDependentOption)

option(ZCNES_BUILD_FRONTEND "Build ZCNES frontend" OFF)
option(ZCNES_BUILD_TESTS "Build ZCNES tests" ON)

cmake_dependent_option(ZCNES_BUILD_SINGLE_STEP_TESTS "Build ZCNES single step tests" OFF ZCNES_BUILD_TESTS OFF)

add_subdirectory(src/core)

if (ZCNES_BUILD_FRONTEND)
  add_subdirectory(src/frontend)
endif()

if (ZCNES_BUILD_TESTS)
  include(CTest)
  enable_testing()
  add_subdirectory(tests)
endif()