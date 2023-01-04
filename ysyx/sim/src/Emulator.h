#pragma once

#include <chrono>
#include <iostream>
#include <string>
namespace chrono = std::chrono;

#include <verilated.h>
#include <VysyxSoCFull.h>
#ifdef DUMP_WAVE_VCD
#include <verilated_vcd_c.h>
#endif
#ifdef DUMP_WAVE_FST
#include <verilated_fst_c.h>
#endif
#include "cxxopts.hpp"

#include "MediaWindow.h"

class Emulator
{
public:
    Emulator(cxxopts::ParseResult &res);
    ~Emulator();
    void reset();
    void step();
    void state();
    std::string getAppName(std::string str);
    bool getArriveTime();
    void runSim();

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
        std::string appName = "";
    } args;

#ifdef DUMP_WAVE_VCD
    VerilatedVcdC *wavePtr = nullptr;
#endif
#ifdef DUMP_WAVE_FST
    VerilatedFstC *wavePtr = nullptr;
#endif
};