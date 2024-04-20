include(${PROJECT_SOURCE_DIR}/cmake/zcnes.cmake)

add_library(core STATIC log.c nes.c)
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
zcnes_options(core)
