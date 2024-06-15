set(SOURCES src/emu.cpp src/bus.cpp)

add_library(core STATIC)
target_sources(core PRIVATE ${SOURCES})
target_include_directories(core PRIVATE src PUBLIC include)

target_link_libraries(core PRIVATE cpu)