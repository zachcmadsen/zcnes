add_executable(tharte tharte.c)
target_link_libraries(tharte PRIVATE core jsonh)
zcnes_compile_options(tharte)

add_test(NAME ProcessorTests.A9 COMMAND tharte a9.json)
