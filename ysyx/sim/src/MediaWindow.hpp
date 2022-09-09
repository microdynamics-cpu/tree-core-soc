#include <ctime>
#include <iostream>
#include <SDL2/SDL.h>

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
                               0,
                               2500,
                               400,
                               300,
                               SDL_WINDOW_SHOWN);
        rdr = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
        std::memset(fb, 0, sizeof(fb));
    }

    void run()
    {
        bool gameIsRunning = true;
        int y = 10;
        while (gameIsRunning)
        {
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                {
                    gameIsRunning = false;
                }
                const Uint8 *kdbState = SDL_GetKeyboardState(NULL);
                if (kdbState[SDL_SCANCODE_RIGHT])
                {
                    std::cout << "right arrow key is pressed" << std::endl;
                }
            }

            SDL_SetRenderDrawColor(rdr, 255, 0, 0, SDL_ALPHA_OPAQUE);
            SDL_RenderClear(rdr);

            start = std::clock();
            SDL_SetRenderDrawColor(rdr, std::rand() % 256, std::rand() % 256, std::rand() % 256, SDL_ALPHA_OPAQUE);
            for (int i = 0; i < 400; ++i)
            {
                for (int j = 0; j < 300; ++j)
                {
                    SDL_RenderDrawPoint(rdr, i, j);
                }
            }

            SDL_RenderPresent(rdr);

            std::cout << "pref: "<< int (CLOCKS_PER_SEC * 1.0 / double(std::clock() - start)) << "fps" <<std::endl;
        }

        SDL_DestroyWindow(win);
        SDL_Delay(3000);
        SDL_Quit();
    }

private:
    SDL_Window *win = nullptr;
    SDL_Renderer *rdr = nullptr;
    clock_t start;
    int fb[640][48];
};