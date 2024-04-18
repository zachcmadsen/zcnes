include(${PROJECT_SOURCE_DIR}/cmake/zcnes.cmake)

add_library(libzcnes STATIC cpu.c)
target_include_directories(libzcnes PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
zcnes_compile_options(libzcnes)
