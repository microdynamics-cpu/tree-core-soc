#include <csignal>
#include <iostream>

#include <SDL2/SDL.h>
#include "cxxopts.hpp"

#include <Emulator.hpp>
#include <MediaWindow.hpp>

#include "verilated.h"
#include "VysyxSoCFull.h"

static int signal_received = 0;
static Emulator *emu = nullptr;
cxxopts::Options args("emu", "OSCPU Seasion 4 SoC Emulator");

void sig_handler(int signo)
{
    if (signal_received != 0)
    {
        std::cout << "SIGINT received, forcely shutting down." << std::endl;
        exit(1);
    }
    std::cout << "\nSIGINT received, gracefully shutting down... Type Ctrl+C again to stop forcely." << std::endl;
    signal_received = signo;
}

void release()
{
    if (emu != nullptr)
    {
        emu->state();
        delete emu;
    }
}

void env_init()
{
    std::cout << "Emulator compiled at " << __DATE__ << ", " << __TIME__ << std::endl;
    if (signal(SIGINT, sig_handler) == SIG_ERR)
    {
        std::cout << "can't catch SIGINT" << std::endl;
    }
    atexit(release);
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
    parseArgs(argc, argv);
    auto res = args.parse(argc, argv);
    if (res.count("help"))
    {
        std::cout << args.help() << std::endl;
        exit(0);
    }

    emu = new Emulator(res);
    // auto win = new MediaWindow();
    // win->run();
    // delete win;
    std::cout << "Start simulating ..." << std::endl;
    while (!Verilated::gotFinish() && signal_received == 0 && !emu->arrive_time())
    {
        emu->step();
    }

    return 0;
}