include(FetchContent)

FetchContent_Declare(
    nes-snd-emu
    GIT_REPOSITORY https://github.com/zachcmadsen/Nes_Snd_Emu
    GIT_TAG main
)
FetchContent_MakeAvailable(nes-snd-emu)

add_library(core STATIC)
target_sources(core PRIVATE src/apu.cpp src/bus.cpp src/cart.cpp src/core.cpp src/ppu.cpp)
target_include_directories(core PRIVATE src PUBLIC include)
target_link_libraries(core PRIVATE nes-snd-emu::nes-snd-emu)