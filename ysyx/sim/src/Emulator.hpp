// #include <unistd.h>
#include <chrono>
#include <iostream>
#include <string>
namespace chrono = std::chrono;

#include "cxxopts.hpp"
#ifdef DUMP_WAVE_VCD
#include <verilated_vcd_c.h>
#elif DUMP_WAVE_FST
#include <verilated_fst_c.h>
#endif
#include <verilated.h>
#include <VysyxSoCFull.h>

extern "C"
{
    void flash_init(const char *img);
}

class Emulator
{
public:
    Emulator(cxxopts::ParseResult &res)
    {
        args.dumpWave = res["dump-wave"].as<bool>();
        args.dumpBegin = res["log-begin"].as<unsigned long>();
        auto tmp = res["log-end"].as<unsigned long>();
        if (tmp > 0) args.dumpEnd = tmp;

        tmp = res["sim-time"].as<unsigned long>();
        if (tmp > 0) args.simTime = tmp;

        args.image = res["image"].as<std::string>();

        startTime = chrono::system_clock::now();
        if (args.image == "")
        {
            std::cout << "Image file unspecified. Use -i to provide the image of flash" << std::endl;
            exit(1);
        }

        std::cout << "Initializing flash with " << args.image << " ..." << std::endl;
        flash_init(args.image.c_str());
        std::cout << "Initializing and resetting DUT ..." << std::endl;

        // set rst signal
        dutPtr = new VysyxSoCFull;
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
        if (args.dumpWave)
        {
            wavePtr->close();
            delete wavePtr;
        }
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

    unsigned long long get_cycle()
    {
        return cycle;
    }

    void state()
    {
        auto elapsed = chrono::duration_cast<chrono::seconds>(chrono::system_clock::now() - startTime);
        std::cout << "Simulation " << get_cycle() << " cycles in " << elapsed.count() << "s" << std::endl;
    }

    bool arrive_time()
    {
        auto elapsed = chrono::duration_cast<chrono::seconds>(chrono::system_clock::now() - startTime);
        if (elapsed.count() > args.simTime)
            return true;
        else
            return false;
    }

private:
    unsigned long long cycle = 0;
    chrono::system_clock::time_point startTime;
    VysyxSoCFull *dutPtr = nullptr;
    struct Args
    {
        bool dumpWave = false;
        unsigned long dumpBegin = 0UL;
        unsigned long dumpEnd = -1UL;
        unsigned long simTime = -1UL;
        std::string image = "";
    } args;

#ifdef DUMP_WAVE_VCD
    VerilatedVcdC *wavePtr = nullptr;
#elif DUMP_WAVE_FST
    VerilatedFstC *wavePtr = nullptr;
#endif
};
