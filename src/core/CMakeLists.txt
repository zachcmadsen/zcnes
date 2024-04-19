include(${PROJECT_SOURCE_DIR}/cmake/zcnes.cmake)

add_library(core STATIC cpu.c log.c)
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
zcnes_compile_options(core)
