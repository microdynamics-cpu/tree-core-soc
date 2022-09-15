// #include <unistd.h>
#include <chrono>
#include <iostream>
#include <string>
namespace chrono = std::chrono;

#include "cxxopts.hpp"
#include "MediaWindow.hpp"
#ifdef DUMP_WAVE_VCD
#include <verilated_vcd_c.h>
#elif DUMP_WAVE_FST
#include <verilated_fst_c.h>
#endif
#include <verilated.h>
#include <VysyxSoCFull.h>

static int signal_received = 0;
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
    std::cout << "Emulator compiled at " << __DATE__ << ", " << __TIME__ << std::endl;
    if (signal(SIGINT, sig_handler) == SIG_ERR)
    {
        std::cout << "can't catch SIGINT" << std::endl;
    }
}

extern "C" void flash_init(const char *img);

class Emulator
{
public:
    Emulator(cxxopts::ParseResult &res)
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
            std::cout << "Image file unspecified. Use -i to provide the image of flash" << std::endl;
            exit(1);
        }

        std::cout << "Initializing flash with " << args.image << " ..." << std::endl;
        flash_init(args.image.c_str());

        if (args.simMode == "gui")
        {
            winPtr = new MediaWindow;
        }

        dutPtr = new VysyxSoCFull;
        reset();

        if (args.dumpWave)
        {
#ifdef DUMP_WAVE_VCD
            wavePtr = new VerilatedVcdC;
#elif DUMP_WAVE_FST
            wavePtr = new VerilatedFstC;
#endif
            Verilated::traceEverOn(true);
            std::cout << "`dump-wave` enabled, waves will be written to \"soc.wave\"." << std::endl;
            dutPtr->trace(wavePtr, 1);
            wavePtr->open("soc.wave");
            wavePtr->dump(0);
        }
    }
    ~Emulator()
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

    void reset()
    {
        std::cout << "Initializing and resetting DUT ..." << std::endl;
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

    void step()
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

    void state()
    {
        auto elapsed = chrono::duration_cast<chrono::seconds>(chrono::system_clock::now() - startTime);
        std::cout << "Simulation " << cycle << " cycles in " << elapsed.count() << "s" << std::endl;
    }

    bool getArriveTime()
    {
        auto elapsed = chrono::duration_cast<chrono::seconds>(chrono::system_clock::now() - startTime);
        if (elapsed.count() > args.simTime)
            return true;
        else
            return false;
    }

    void runSim()
    {
        if (winPtr)
        {
            winPtr->initFPS();
            winPtr->initFB();
        }

        while (!Verilated::gotFinish() && signal_received == 0 && !getArriveTime())
        {
            step();
            if (winPtr && !(winPtr->step()))
                break;
        }
        if (winPtr)
        {
            winPtr->calcFPS();
            winPtr->release();
        }

        dutPtr->final();
    }

private:
    unsigned long long cycle = 0;
    chrono::system_clock::time_point startTime;
    VysyxSoCFull *dutPtr = nullptr;
    MediaWindow *winPtr = nullptr;
    struct Args
    {
        bool dumpWave = false;
        unsigned long dumpBegin = 0UL;
        unsigned long dumpEnd = -1UL;
        unsigned long simTime = -1UL;
        std::string simMode = "";
        std::string image = "";
    } args;

#ifdef DUMP_WAVE_VCD
    VerilatedVcdC *wavePtr = nullptr;
#elif DUMP_WAVE_FST
    VerilatedFstC *wavePtr = nullptr;
#endif
};
