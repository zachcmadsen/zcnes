add_library(zcnes bus.cpp cart.cpp core.cpp ppu.cpp)
target_include_directories(zcnes PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(zcnes PRIVATE .)