
#include <cstdint>
#include <simdjson.h>

// struct CpuState
// {
//     std::uint16_t pc;
//     std::uint16_t s;
//     std::uint16_t a;
//     std::uint16_t x;
//     std::uint16_t y;
//     std::uint16_t p;
//     std::vector<std::array<std::uint16_t, 2>> ram;
// };

// struct Test
// {
//     std::string_view name;
//     CpuState initial;
//     CpuState final;
// };

int main(int argc, char *argv[])
{
    simdjson::ondemand::parser parser;
    simdjson::padded_string json = simdjson::padded_string::load("a9.json");
    simdjson::ondemand::document doc = parser.iterate(json);

    for (auto test : doc)
    {
        std::string_view name = test["name"];

        // For the best performance, refer to each array/object once and seek keys in the order in which they appear in
        // the document.
        simdjson::ondemand::object initial = test["initial"];
        auto pc = static_cast<std::uint16_t>(initial["pc"].get_uint64());
        auto s = static_cast<std::uint8_t>(initial["a"].get_uint64());
    }

    // std::cout << doc[0]["name"] << '\n';
}
