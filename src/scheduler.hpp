#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

namespace zcnes
{

enum class EventKind
{
    Reset,
    VBlank
};

class Scheduler
{
  public:
    void add(EventKind kind)
    {
        add(0, kind);
    }

    void add(std::uint64_t rel_timestamp, EventKind kind)
    {
        // TODO: Set a maximum number of events?
        events.emplace_back(rel_timestamp + master_clock, kind);
        find_next_event();
    }

    template <std::invocable<EventKind> T> void check(T &&f)
    {
        while (next_event_timestamp <= master_clock)
        {
            const auto next_event = events[next_event_index];
            std::forward<T>(f)(next_event.kind);

            events[next_event_index] = events.back();
            events.pop_back();

            next_event_timestamp = std::numeric_limits<std::uint64_t>::max();
            find_next_event();
        }
    }

    void tick()
    {
        constexpr std::uint64_t cpu_master_clock_divider = 12;
        master_clock += cpu_master_clock_divider / 2;
    }

    std::uint64_t ticks() const
    {
        return master_clock;
    }

  private:
    struct Event
    {
        std::uint64_t timestamp;
        EventKind kind;
    };

    std::vector<Event> events{};

    std::uint64_t next_event_timestamp{std::numeric_limits<std::uint64_t>::max()};
    std::size_t next_event_index{};

    std::uint64_t master_clock{2};

    void find_next_event()
    {
        for (std::size_t i = 0; i < events.size(); ++i)
        {
            if (events[i].timestamp < next_event_timestamp)
            {
                next_event_timestamp = events[i].timestamp;
                next_event_index = i;
            }
        }
    }
};

} // namespace zcnes
