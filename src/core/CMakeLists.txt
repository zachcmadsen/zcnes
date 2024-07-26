include(FetchContent)

FetchContent_Declare(
    nes-snd-emu
    GIT_REPOSITORY https://github.com/zachcmadsen/Nes_Snd_Emu
    GIT_TAG main
)
message(STATUS "Fetching Nes_Snd_Emu...")
FetchContent_MakeAvailable(nes-snd-emu)

add_library(core STATIC)
target_sources(core PRIVATE apu.cpp bus.cpp cart.cpp core.cpp ppu.cpp)
target_include_directories(core PUBLIC .)
target_link_libraries(core PUBLIC nes-snd-emu::nes-snd-emu)