set(SOURCES src/bus.cpp src/core.cpp )

add_library(zcnes STATIC)
target_sources(zcnes PRIVATE ${SOURCES})
target_include_directories(zcnes PRIVATE src PUBLIC include)

include(${PROJECT_SOURCE_DIR}/cmake/zcnes.cmake)
zcnes_compile_options(zcnes)