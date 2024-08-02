include(FetchContent)

FetchContent_Declare(
    nes-snd-emu
    URL https://github.com/zachcmadsen/Nes_Snd_Emu/releases/download/v0.1.0/nes-snd-emu-src.tar.gz
)
message(STATUS "Fetching Nes_Snd_Emu...")
FetchContent_MakeAvailable(nes-snd-emu)

add_library(core STATIC)
target_sources(core PRIVATE apu.cpp bus.cpp cart.cpp core.cpp ppu.cpp)
target_include_directories(core PUBLIC .)
target_link_libraries(core PUBLIC nes-snd-emu::nes-snd-emu)