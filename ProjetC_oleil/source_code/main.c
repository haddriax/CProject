#include "SDL2/SDL.h"

const char ProjectName[] = "CProject";
const int default_width = 720;
const int default_height = 480;

int main (int argc, char** argv)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        exit(1);
    }

    SDL_Window *window = SDL_CreateWindow(ProjectName,
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          default_width, default_height, 0);

    if (window == NULL)
    {
        exit(1);
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL)
    {
        exit(1);
    }
    SDL_RenderClear(renderer);

    while (1)
    {

    }
}