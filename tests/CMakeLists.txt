add_executable(tharte tharte.c)
target_link_libraries(tharte PRIVATE common core jsonh)
zcnes_options(tharte)

add_test(NAME ProcessorTests.A5 COMMAND tharte ${CMAKE_SOURCE_DIR}/../zcnes-tests/ProcessorTests/a5.json)
add_test(NAME ProcessorTests.A9 COMMAND tharte ${CMAKE_SOURCE_DIR}/../zcnes-tests/ProcessorTests/a9.json)
add_test(NAME ProcessorTests.AD COMMAND tharte ${CMAKE_SOURCE_DIR}/../zcnes-tests/ProcessorTests/ad.json)
