add_executable(tharte tharte.c)
target_link_libraries(tharte PRIVATE core jsonh)
zcnes_options(tharte)

add_test(NAME ProcessorTests.A5 COMMAND tharte ${CMAKE_CURRENT_SOURCE_DIR}/a5.json)
add_test(NAME ProcessorTests.A9 COMMAND tharte ${CMAKE_CURRENT_SOURCE_DIR}/a9.json)
add_test(NAME ProcessorTests.AD COMMAND tharte ${CMAKE_CURRENT_SOURCE_DIR}/ad.json)
