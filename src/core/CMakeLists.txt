add_library(core STATIC)
target_sources(core PRIVATE src/bus.cpp src/cart.cpp src/core.cpp src/ppu.cpp)
target_include_directories(core PRIVATE src PUBLIC include)
target_link_libraries(core PRIVATE cpu)