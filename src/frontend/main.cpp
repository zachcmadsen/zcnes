#include "SDL_audio.h"
#include "SDL_render.h"
#include "SDL_video.h"
#include <SDL.h>
#include <algorithm>
#include <array>
#include <atomic>
#include <bit>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <math.h>
#include <span>
#include <thread>
#include <vector>

#include <core.hpp>
#include <rtrb.h>

#include "triple_buffer.hpp"

std::atomic_flag flag = ATOMIC_FLAG_INIT;

std::atomic_flag exit_flag = ATOMIC_FLAG_INIT;

void SDLCALL audio_callback(void *user_data, std::uint8_t *stream, int len)
{
    auto *rb = static_cast<rtrb *>(user_data);

    auto read = rtrb_read(rb, stream, static_cast<std::size_t>(len));

    if (read != static_cast<std::size_t>(len))
    {
        std::cout << "not enough samples\n";
    }

    // Zero leftover bytes, if any.
    for (std::size_t i = read; i < static_cast<std::size_t>(len); ++i)
    {
        stream[i] = 0;
    }

    flag.test_and_set();
    flag.notify_one();
}

int main(int argc, char *argv[])
{
    int width = 1280;
    int height = 720;

    if (argc != 2)
    {
        std::cerr << "not enough arguments\n";
        return 1;
    }

    std::ifstream ifs{argv[1], std::ios::binary};
    if (!ifs.is_open())
    {
        std::cerr << "failed to open file";
        return 1;
    }
    const std::vector<std::uint8_t> rom{std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>()};

    rtrb *rb = rtrb_new(1024 * sizeof(std::int16_t));

    TripleBuffer<256 * 240 * sizeof(std::uint32_t)> triple_buffer;

    std::thread emu_thread(
        [&triple_buffer](rtrb *rb, std::span<const std::uint8_t> rom) {
            zcnes::Core core{rom};

            core.set_on_frame([&triple_buffer](std::span<const std::uint8_t> pixels) {
                auto write_buffer = triple_buffer.get_write_buffer();
                std::copy(pixels.begin(), pixels.end(), write_buffer.begin());
                triple_buffer.swap_write_buffer();
            });

            std::array<std::int16_t, 1024> sample_buffer{};

            while (!exit_flag.test())
            {
                auto available_bytes = rtrb_write_available(rb);
                auto samples_needed = available_bytes / sizeof(std::int16_t);

                std::span samples{sample_buffer.data(), samples_needed};
                core.fill(samples);

                rtrb_write(rb, std::bit_cast<std::uint8_t *>(sample_buffer.data()), available_bytes);

                flag.wait(false);
                flag.clear();
            }
        },
        rb, rom);

    // The video subsystem automatically initializes the events subsystem.
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);

    SDL_AudioSpec desired;
    std::memset(&desired, 0, sizeof(desired));
    desired.freq = 44100;
    desired.format = AUDIO_S16;
    desired.channels = 1;
    desired.samples = 1024;
    desired.callback = audio_callback;
    desired.userdata = rb;

    SDL_AudioSpec obtained;
    auto audio_device_id = SDL_OpenAudioDevice(nullptr,   // Request the most reasonable default device
                                               0,         // The device should be opened for playback
                                               &desired,  // a
                                               &obtained, // a
                                               0          // Don't allow changes
    );

    // An opened audio device starts out paused. We need to unpause it.
    SDL_PauseAudioDevice(audio_device_id, 0);

    SDL_Window *window =
        SDL_CreateWindow("SDL pixels", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 256, 240, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBX8888, SDL_TEXTUREACCESS_STREAMING, 256, 240);

    for (;;)
    {
        SDL_Event ev;
        while (SDL_PollEvent(&ev))
        {
            if (ev.type == SDL_QUIT)
            {
                goto cleanup;
            }
        }

        void *pixels;
        int pitch;
        SDL_LockTexture(texture, NULL, &pixels, &pitch);
        {
            auto read_buffer = triple_buffer.get_read_buffer();
            std::copy(read_buffer.begin(), read_buffer.end(), static_cast<std::uint8_t *>(pixels));
        }
        SDL_UnlockTexture(texture);

        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

cleanup:

    SDL_CloseAudioDevice(audio_device_id);

    exit_flag.test_and_set();

    flag.test_and_set();
    flag.notify_one();

    emu_thread.join();

    rtrb_free(rb);

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
