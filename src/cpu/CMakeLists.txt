add_library(cpu INTERFACE)

target_sources(cpu
    INTERFACE
    FILE_SET HEADERS
    FILES cpu.h)