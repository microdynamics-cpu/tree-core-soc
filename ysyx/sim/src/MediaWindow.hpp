#include <ctime>
#include <iostream>
#include <SDL2/SDL.h>

// screen res
#define VGA_H_RES 640
#define VGA_V_RES 480

// for SDL texture
typedef struct Pixel
{
    uint8_t a; // transparency
    uint8_t b; // blue
    uint8_t g; // green
    uint8_t r; // red
} Pixel;

class MediaWindow
{
public:
    MediaWindow()
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            std::cout << "SDL could not be initialized: " << SDL_GetError();
        }
        else
        {
            std::cout << "SDL video system is ready to go" << std::endl;
        }

        win = SDL_CreateWindow("PS/2 & VGA Test",
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               VGA_H_RES,
                               VGA_V_RES,
                               SDL_WINDOW_SHOWN);

        // NOTE:  need to handle excpt cond
        if (!win)
        {
            printf("Window creation failed: %s\n", SDL_GetError());
        }
        rdr = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
        if (!rdr)
        {
            printf("Renderer creation failed: %s\n", SDL_GetError());
        }
        txr = SDL_CreateTexture(rdr, SDL_PIXELFORMAT_RGBA8888,
                                SDL_TEXTUREACCESS_TARGET, VGA_H_RES, VGA_V_RES);
        if (!txr)
        {
            printf("Texture creation failed: %s\n", SDL_GetError());
        }
    }

    void initFPS()
    {
        startTick = SDL_GetPerformanceCounter();
        frameCnt = 0;
    }

    void calcFPS()
    {
        uint64_t endTick = SDL_GetPerformanceCounter();
        double duration = ((double)(endTick - startTick)) / SDL_GetPerformanceFrequency();
        double fps = (double)frameCnt / duration;
        std::cout << "Frames per second: " << fps << std::endl;
    }

    void initFB()
    {
        for(int i = 0; i < VGA_V_RES; ++i)
            for(int j = 0; j < VGA_H_RES; ++j)
            {
                auto p = &fb[i*VGA_H_RES+j];
                p->a = 0xFF;
                p->b = 0xFF;
                p->g = 0x00;
                p->r = 0x00;
            }
    }

    bool step()
    {
        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                return false;
            }
            const Uint8 *kdbState = SDL_GetKeyboardState(NULL);
            if (kdbState[SDL_SCANCODE_RIGHT])
            {
                std::cout << "right arrow key is pressed" << std::endl;
            }
        }

        SDL_UpdateTexture(txr, NULL, fb, VGA_H_RES * sizeof(Pixel));
        SDL_RenderClear(rdr);
        SDL_RenderCopy(rdr, txr, NULL, NULL);
        SDL_RenderPresent(rdr);
        frameCnt++;
        return true;
    }

    void release()
    {
        SDL_DestroyTexture(txr);
        SDL_DestroyRenderer(rdr);
        SDL_DestroyWindow(win);
        SDL_Delay(500);
        SDL_Quit();
    }

private:
    SDL_Window *win = nullptr;
    SDL_Renderer *rdr = nullptr;
    SDL_Texture *txr = nullptr;
    clock_t start;
    Pixel fb[VGA_H_RES * VGA_V_RES];
    uint64_t startTick = 0;
    uint64_t frameCnt = 0;
};