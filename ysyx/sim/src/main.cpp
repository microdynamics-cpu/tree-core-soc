#include <csignal>
#include <iostream>

#include <SDL2/SDL.h>
#include "cxxopts.hpp"

#include <Emulator.hpp>

#include "verilated.h"
#include "VysyxSoCFull.h"

static Emulator *emu = nullptr;
cxxopts::Options args("emu", "OSCPU Seasion 4 SoC Emulator");

void release()
{
    if (emu != nullptr)
    {
        emu->state();
        delete emu;
    }
}

void parseArgs(int argc, char *argv[])
{
    // clang-format off
    args.add_options()
        ("h,help", "print usage")
        ("d,dump-wave", "dump vcd(fst) format waveform when log is enabled", cxxopts::value<bool>()->default_value("false"))
        ("b,log-begin", "display log from NUM th cycle", cxxopts::value<unsigned long>()->default_value("0"))
        ("e,log-end", "stop display log at NUM th cycle", cxxopts::value<unsigned long>()->default_value("0"))
        ("t,sim-time", "stop simulation after NUM seconds", cxxopts::value<unsigned long>()->default_value("0"))
        ("i,image", "run with this image file", cxxopts::value<std::string>());
    // clang-format on
    Verilated::commandArgs(argc, argv);
}

int main(int argc, char *argv[])
{
    env_init();
    atexit(release);
    parseArgs(argc, argv);
    auto res = args.parse(argc, argv);
    if (res.count("help"))
    {
        std::cout << args.help() << std::endl;
        exit(0);
    }

    emu = new Emulator(res);
    std::cout << "[verilator]start simulating ..." << std::endl;
    emu->run_sim();

    return 0;
}