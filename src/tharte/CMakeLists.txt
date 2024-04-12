include(${PROJECT_SOURCE_DIR}/cmake/zcnes.cmake)

add_executable(tharte main.c tharte.c)
target_include_directories(tharte PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(tharte PRIVATE common jsonh)
zcnes_compile_options(tharte)
