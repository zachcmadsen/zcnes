include(${PROJECT_SOURCE_DIR}/cmake/zcnes.cmake)

add_library(core core.cc)

target_include_directories(core PRIVATE .)

target_link_libraries(core PRIVATE cpu)

zcnes_compile_options(core)