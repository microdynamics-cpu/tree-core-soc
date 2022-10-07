#include <csignal>

#include "rang.hpp"

#include "MediaWindow.h"
#include "Emulator.h"

static int signal_received = 0;
static std::string app_name[] = {"kdb", "vga"};

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

void env_init()
{
    std::cout << rang::fg::magenta << "Emulator compiled at " << __DATE__ << ", " << __TIME__ << rang::fg::reset << std::endl;
    if (signal(SIGINT, sig_handler) == SIG_ERR)
    {
        std::cout << "can't catch SIGINT" << std::endl;
    }
}

extern "C" void flash_init(const char *img);

Emulator::Emulator(cxxopts::ParseResult &res)
{
    args.dumpWave = res["dump-wave"].as<bool>();
    args.dumpBegin = res["log-begin"].as<unsigned long>();
    auto tmp = res["log-end"].as<unsigned long>();
    if (tmp > 0)
        args.dumpEnd = tmp;

    tmp = res["sim-time"].as<unsigned long>();
    if (tmp > 0)
        args.simTime = tmp;

    args.simMode = res["sim-mode"].as<std::string>();
    args.image = res["image"].as<std::string>();

    startTime = chrono::system_clock::now();
    if (args.image == "")
    {
        std::cout << rang::fg::red << "Image file unspecified. Use -i to provide the image of flash" << rang::fg::reset << std::endl;
        exit(1);
    }

    args.appName = getAppName(args.image);

    std::cout << rang::fg::green << "Initializing flash with " << args.image << " ..." << rang::fg::reset << std::endl;
    flash_init(args.image.c_str());

    if (args.simMode == "gui")
    {
        winPtr = new MediaWindow;
    }

    dutPtr = new VysyxSoCFull;
    reset();
    svSetScope(svGetScopeFromName("TOP.ysyxSoCFull.screen"));

    if (args.dumpWave)
    {
#ifdef DUMP_WAVE_VCD
        wavePtr = new VerilatedVcdC;
#endif
#ifdef DUMP_WAVE_FST
        wavePtr = new VerilatedFstC;
#endif
        Verilated::traceEverOn(true);
        std::cout << rang::fg::yellow << "`dump-wave` enabled, waves will be written to \"soc.wave\"." << rang::fg::reset << std::endl;
        dutPtr->trace(wavePtr, 1);
        wavePtr->open("soc.wave");
        wavePtr->dump(0);
    }
}

Emulator::~Emulator()
{
    if (wavePtr)
    {
        wavePtr->close();
        delete wavePtr;
    }
    if (winPtr)
    {
        delete winPtr;
    }
}

void Emulator::reset()
{
    std::cout << rang::fg::yellow << "Initializing and resetting DUT ..." << rang::fg::reset << std::endl;
    dutPtr->reset = 1;
    for (int i = 0; i < 10; i++)
    {
        dutPtr->clock = 0;
        dutPtr->eval();
        dutPtr->clock = 1;
        dutPtr->eval();
    }
    dutPtr->clock = 0;
    dutPtr->reset = 0;
    dutPtr->eval();
}

void Emulator::step()
{
    dutPtr->clock = 1;
    dutPtr->eval();
    ++cycle;
    if (args.dumpWave && args.dumpBegin <= cycle && cycle <= args.dumpEnd)
    {
        wavePtr->dump((vluint64_t)cycle);
    }
    dutPtr->clock = 0;
    dutPtr->eval();
}

void Emulator::state()
{
    auto elapsed = chrono::duration_cast<chrono::seconds>(chrono::system_clock::now() - startTime);
    std::cout << rang::fg::yellow << "Simulation " << cycle << " cycles in " << elapsed.count() << "s" << rang::fg::reset << std::endl;
}

std::string Emulator::getAppName(std::string str)
{
    for (auto v : app_name)
    {
        if (str.find(v) != std::string::npos)
        {
            return v;
        }
    }
    return "";
}

bool Emulator::getArriveTime()
{
    auto elapsed = chrono::duration_cast<chrono::seconds>(chrono::system_clock::now() - startTime);
    if (elapsed.count() > args.simTime)
        return true;
    else
        return false;
}

extern void dump_gpr();
void Emulator::runSim()
{
    if (winPtr)
    {
        winPtr->initFPS();
        winPtr->initFB();
        if (args.appName == "kdb")
        {
            winPtr->initImage();
        }
        else if (args.appName == "vga")
        {
            winPtr->initVideo();
        }
    }

    while (!Verilated::gotFinish() && signal_received == 0 && !getArriveTime())
    {
        step();
        if (winPtr && !(winPtr->step(args.appName)))
            break;
        dump_gpr();
    }
    if (winPtr)
    {
        winPtr->calcFPS();
        winPtr->release(args.appName);
    }

    dutPtr->final();
}
