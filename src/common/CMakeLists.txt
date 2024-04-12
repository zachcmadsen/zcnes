include(${PROJECT_SOURCE_DIR}/cmake/zcnes.cmake)

add_library(common STATIC log.c)
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
zcnes_compile_options(common)
