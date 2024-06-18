add_library(core core.cpp)

target_include_directories(core PRIVATE .)

target_link_libraries(core PRIVATE cpu)