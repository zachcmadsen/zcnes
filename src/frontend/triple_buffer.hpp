// TODO: I think the interface could be better. My Rust version has writer and
// reader objects with an Arc (shared_ptr) to the buffers. I'm not sure if that
// pattern is idiomatic in C++ though.
//
// TODO: Use more efficient memory orderings. I'm fine with seq_cst for now
// since I don't understand the memory orderings that well.

#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>

template <std::size_t n> class TripleBuffer
{
  public:
    TripleBuffer()
        : buffers{std::make_unique<std::uint8_t[]>(n), std::make_unique<std::uint8_t[]>(n),
                  std::make_unique<std::uint8_t[]>(n)}
    {
    }

    std::span<std::uint8_t, n> get_write_buffer()
    {
        return std::span<std::uint8_t, n>{buffers[write_buffer_index].get(), n};
    }

    void swap_write_buffer()
    {
        const auto new_back_buffer_index = write_buffer_index | FRESH_FLAG;

        const auto old_back_buffer_index = back_buffer_index.exchange(new_back_buffer_index);
        write_buffer_index = old_back_buffer_index & INDEX_MASK;
    }

    std::span<const std::uint8_t, n> get_read_buffer()
    {
        // Swap the read and back buffers if the back buffer is fresh.
        if ((back_buffer_index.load() & FRESH_FLAG) != 0)
        {
            const auto new_back_buffer_index = read_buffer_index;
            const auto old_back_buffer_index = back_buffer_index.exchange(new_back_buffer_index);
            read_buffer_index = old_back_buffer_index & INDEX_MASK;
        }

        return std::span<std::uint8_t, n>{buffers[read_buffer_index].get(), n};
    }

  private:
    static constexpr std::uint8_t INDEX_MASK = 0b0000'0011;
    static constexpr std::uint8_t FRESH_FLAG = 0b0000'0100;

    // TODO: Look into avoiding false sharing.
    std::array<std::unique_ptr<std::uint8_t[]>, 3> buffers;

    std::atomic<std::uint8_t> back_buffer_index{0};
    static_assert(decltype(back_buffer_index)::is_always_lock_free);

    std::uint8_t write_buffer_index{1};
    std::uint8_t read_buffer_index{2};
};
