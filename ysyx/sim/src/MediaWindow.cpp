#include <SDL2/SDL_image.h>
#include "rang.hpp"

#include "MediaWindow.h"

extern "C" void kdb_read(char dat);

MediaWindow::MediaWindow()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << rang::fg::red << "SDL could not be initialized: " << SDL_GetError() << rang::fg::reset << std::endl;
    }
    else
    {
        std::cout << rang::fg::yellow << "SDL video system is ready to go" << rang::fg::reset << std::endl;
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
        std::cout << rang::fg::red << "Window creation failed: " << SDL_GetError() << rang::fg::reset << std::endl;
    }
    rdr = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (!rdr)
    {
        std::cout << rang::fg::red << "Renderer creation failed: " << SDL_GetError() << rang::fg::reset << std::endl;
    }
}

void MediaWindow::initFPS()
{
    startTick = SDL_GetPerformanceCounter();
    frameCnt = 0;
}

void MediaWindow::calcFPS()
{
    uint64_t endTick = SDL_GetPerformanceCounter();
    double duration = ((double)(endTick - startTick)) / SDL_GetPerformanceFrequency();
    double fps = (double)frameCnt / duration;
    std::cout << "Frames per second: " << fps << std::endl;
}

void MediaWindow::initFB()
{
    for (int cnt = 0; cnt < VGA_V_RES; ++cnt)
        for (int j = 0; j < VGA_H_RES; ++j)
        {
            auto p = &fb[cnt * VGA_H_RES + j];
            p->a = 0xFF;
            p->b = 0xFF;
            p->g = 0x00;
            p->r = 0x00;
        }
}

void MediaWindow::initImage()
{
    int flags = IMG_INIT_PNG;
    int initFlags = IMG_Init(flags);
    if ((initFlags & flags) != flags)
    {
        std::cout << rang::fg::red << "SDL2 format not available" << rang::fg::reset << std::endl;
        return;
    }

    char *basePath = SDL_GetBasePath();
    char imgPath[128];
    strcpy(imgPath, basePath);
    strcat(imgPath, "../asset/kdb.png");
    SDL_free(basePath);
    // std::cout << imgPath << std::endl;
    srf = IMG_Load(imgPath);
    if (!srf)
    {
        std::cout << rang::fg::red << "Image not loaded..." << rang::fg::reset << std::endl;
        return;
    }

    txr = SDL_CreateTextureFromSurface(rdr, srf);
    SDL_SetRenderDrawColor(rdr, 0, 0, 0xFF, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(rdr);
    SDL_RenderCopy(rdr, txr, NULL, NULL);
    SDL_RenderPresent(rdr);
    appInit = true;
}

void MediaWindow::initVideo()
{
    txr = SDL_CreateTexture(rdr, SDL_PIXELFORMAT_RGBA8888,
                            SDL_TEXTUREACCESS_TARGET, VGA_H_RES, VGA_V_RES);
    if (!txr)
    {
        std::cout << rang::fg::red << "Texture creation failed: " << SDL_GetError() << rang::fg::reset << std::endl;
    }
    appInit = true;
}

// now dont support break code
void MediaWindow::encode(int sdlCode, KdbCodeType codeType)
{
    int cnt = 0;
    auto info = keymap[sdlCode];
    switch (info.type)
    {
    case K_UNKNOWN:
    {
        break;
    }
    case K_NORMAL:
    {
        if (codeType == K_BREAK)
        {
            kdbCode[cnt++] = 0xF0;
        }
        kdbCode[cnt++] = info.code;
        break;
    }
    case K_EXTENDED:
    {
        kdbCode[cnt++] = 0xE0;
        if (codeType == K_BREAK)
        {
            kdbCode[cnt++] = 0xF0;
        }
        kdbCode[cnt++] = info.code;
        break;
    }
    case K_NUMLOCK:
    {
        // assumes num lock is always active
        if (codeType == K_MAKE)
        {
            // fake shift press
            kdbCode[cnt++] = 0xE0;
            kdbCode[cnt++] = 0x12;
            kdbCode[cnt++] = 0xE0;
            kdbCode[cnt++] = info.code;
        }
        else
        {
            kdbCode[cnt++] = 0xE0;
            kdbCode[cnt++] = 0xF0;
            kdbCode[cnt++] = info.code;
            // fake shift release
            kdbCode[cnt++] = 0xE0;
            kdbCode[cnt++] = 0xF0;
            kdbCode[cnt++] = 0x12;
        }
        break;
    }
    case K_SHIFT:
    {
        SDL_Keymod mod = SDL_GetModState();
        if (codeType == K_MAKE)
        {
            // fake shift release
            if (mod & KMOD_LSHIFT)
            {
                kdbCode[cnt++] = 0xE0;
                kdbCode[cnt++] = 0xF0;
                kdbCode[cnt++] = 0x12;
            }
            if (mod & KMOD_RSHIFT)
            {
                kdbCode[cnt++] = 0xE0;
                kdbCode[cnt++] = 0xF0;
                kdbCode[cnt++] = 0x59;
            }
            kdbCode[cnt++] = 0xE0;
            kdbCode[cnt++] = info.code;
        }
        else
        {
            kdbCode[cnt++] = 0xE0;
            kdbCode[cnt++] = 0xF0;
            kdbCode[cnt++] = info.code;
            // fake shift press
            if (mod & KMOD_RSHIFT)
            {
                kdbCode[cnt++] = 0xE0;
                kdbCode[cnt++] = 0x59;
            }
            if (mod & KMOD_LSHIFT)
            {
                kdbCode[cnt++] = 0xE0;
                kdbCode[cnt++] = 0x12;
            }
        }
        break;
    }
    }
}

bool MediaWindow::step(std::string app)
{
    SDL_Event event;
    if (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            return false;
        }
        const Uint8 *kdbState = SDL_GetKeyboardState(NULL);
        int keyLen = sizeof(keymap) / sizeof(KdbInfo);
        bool is_find = false;
        for (int i = 0; i < keyLen; ++i)
        {
            if (kdbState[i])
            {
                // std::cout << i << std::endl;
                encode(i);
                kdb_read(kdbCode[0]);
                is_find = true;
                break;
            }
        }

        if (!is_find)
        {
            kdb_read(0x00);
        }
    }
    else
    {
        kdb_read(0x00);
    }

    if (appInit)
    {
        if (app == "kdb")
        {
            // do nothing!
        }
        else if (app == "vga")
        {
            // NOTE: fake flush oper
            if (frameCnt % 120000 == 0)
            {
                SDL_UpdateTexture(txr, NULL, fb, VGA_H_RES * sizeof(Pixel));
                SDL_RenderClear(rdr);
                SDL_RenderCopy(rdr, txr, NULL, NULL);
                SDL_RenderPresent(rdr);
            }
        }
    }

    frameCnt++;
    return true;
}

void MediaWindow::release(std::string app)
{
    SDL_DestroyTexture(txr);
    SDL_DestroyRenderer(rdr);
    SDL_DestroyWindow(win);
    SDL_Delay(500);
    if (appInit && app == "kdb")
    {
        SDL_FreeSurface(srf);
        IMG_Quit();
    }
    SDL_Quit();
}