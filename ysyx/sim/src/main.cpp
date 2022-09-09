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
    auto win = new MediaWindow();
    win->run();
    delete win;
    std::cout << "Start simulating ..." << std::endl;
    while (!Verilated::gotFinish() && signal_received == 0 && !emu->arrive_time())
    {
        emu->step();
    }

    return 0;
}

// int main(int argc, char* argv[]){

//     // Create a window data type
//     // This pointer will point to the 
//     // window that is allocated from SDL_CreateWindow
//     SDL_Window* window=nullptr;

//     // Initialize the video subsystem.
//     // If it returns less than 1, then an
//     // error code will be received.
//     if(SDL_Init(SDL_INIT_VIDEO) < 0){
//         std::cout << "SDL could not be initialized: " <<
//                   SDL_GetError();
//     }else{
//         std::cout << "SDL video system is ready to go\n";
//     }

//     // Request a window to be created for our platform
//     // The parameters are for the title, x and y position,
//     // and the width and height of the window.
//     window = SDL_CreateWindow("C++ SDL2 Window",
//             0,
//             2500,
//             640,
//             480,
//             SDL_WINDOW_SHOWN);

//     // Infinite loop for our application
//     bool gameIsRunning = true;
//     while(gameIsRunning){
//         SDL_Event event;
//         // Start our event loop
//         while(SDL_PollEvent(&event)){
//             // Handle each specific event
//             if(event.type == SDL_QUIT){
//                 gameIsRunning= false;
//             }
//             if(event.type == SDL_MOUSEMOTION){
//                 std::cout << "mouse has been moved\n";
//             }
//             if(event.type == SDL_KEYDOWN){
//                 std::cout << "a key has been pressed\n";
//                 if(event.key.keysym.sym == SDLK_0){
//                     std::cout << "0 was pressed\n";
//                 }else{
//                     std::cout << "0 was not pressed\n";
//                 }
//             }
//             // Retrieve the state of all of the keys
//             // Then we can query the scan code of one or more
//             // keys at a time
//             const Uint8* state = SDL_GetKeyboardState(NULL);
//             if(state[SDL_SCANCODE_RIGHT]){
//                 std::cout << "right arrow key is pressed\n";
//             }

//         }
//     }

//     // We destroy our window. We are passing in the pointer
//     // that points to the memory allocated by the 
//     // 'SDL_CreateWindow' function. Remember, this is
//     // a 'C-style' API, we don't have destructors.
//     SDL_DestroyWindow(window);
    
//     // We safely uninitialize SDL2, that is, we are
//     // taking down the subsystems here before we exit
//     // We add a delay in order to see that our window
//     // has successfully popped up.
//     SDL_Delay(3000);
//     // We add a delay in order to see that our window
//     // has successfully popped up.
//     SDL_Delay(3000);
//     // We add a delay in order to see that our window
//     // has successfully popped up.
//     SDL_Delay(3000);
//     // We add a delay in order to see that our window
//     // has successfully popped up.
//     SDL_Delay(3000);
//     // We add a delay in order to see that our window
//     // has successfully popped up.
//     SDL_Delay(3000);
//     // our program.
//     SDL_Quit();
//     return 0;
// }

