#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <ios>
#include <iostream>
#include <iterator>
#include <memory>
#include <span>
#include <string>
#include <vector>

#include <common/common.h>
#include <core/core.hpp>


constexpr std::uint16_t status_addr = 0x6000;
constexpr std::uint16_t output_addr = 0x6004;
constexpr std::uint8_t running_status = 0x80;

void run(const char *path)
{
    std::ifstream ifs{path, std::ios::binary};
    require(ifs.is_open());
    const std::vector<std::uint8_t> rom{std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>()};

    auto core = zcnes::make_core(rom);

    auto status = core->peek(status_addr).value();
    while (status != running_status)
    {
        core->step();
        status = core->peek(status_addr).value();
    }

    while (status == running_status)
    {
        core->step();
        status = core->peek(status_addr).value();
    }

    std::vector<std::uint8_t> output{};
    auto addr = output_addr;
    auto c = core->peek(addr).value();
    while (c != '\0')
    {
        output.push_back(c);
        addr += 1;
        c = core->peek(addr).value();
    }

    std::string str{output.begin(), output.end()};
    const auto passed = str.find("Passed") != std::string::npos;
    if (!passed)
    {
        std::cerr << str << '\n';
    }
    require(passed);
}

int main(int argc, char *argv[])
{
    std::span<char *> args{argv, static_cast<std::size_t>(argc)};
    if (args.size() < 2)
    {
        std::cerr << "error: no input file\n";
        return EXIT_FAILURE;
    }

    try
    {
        const auto *path = args[1];
        run(path);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
