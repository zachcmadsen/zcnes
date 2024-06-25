add_library(zcnes core.cpp)

target_include_directories(zcnes PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(zcnes PRIVATE .)