#include "vue_controller/vue_controller.h"

const char ProjectName[] = "CProject";
const int default_width = 720;
const int default_height = 480;

int main (int argc, char** argv)
{;
    Player p = { 0, 0, vector2f_zero };
    app.player = &p;
    init_player(&p);

    struct Clock clock;

    //Create and initialize display. @TODO parameters should be read from config.txt
    RenderWindow rw = { NULL, NULL, NULL };
    init_RenderWindow(&rw, 720, 480, "Projet C");
    
    // Variables used for Clock
    clock.last_time = SDL_GetTicks64();
    clock.current_time = 0;
    clock.delta_ticks = 0;

    // Expected time for a frame, in milliseconds.
    const Uint64 frame_duration_ms = (1000 / 60);

    int index = 4;
    while (index-- > 0)
        printf("%i\n", index);

    while (handle_inputs())
    {
        clock.current_time = SDL_GetTicks64();
        clock.delta_ticks = (clock.current_time - clock.last_time);

        // If the last time we udapted is old enough., so we respect targeted framerate.
        // i.e. code in this if() section is frame capped.
        if (frame_duration_ms < clock.delta_ticks)
        {
            // Display FPS in the window title.
            update_window_name(&rw, "Hello: ", (float)clock.delta_ticks);

            // Game update
            gameloop();

            // Drawing shapes
            render(&rw);

            // Remember the time at which this frame end.
            clock.last_time = SDL_GetTicks64();
        }
    }

    // Cleanly quit SDL.
    SDL_DestroyWindow(rw.sdl_win);
    SDL_DestroyRenderer(rw.sdl_renderer);
    SDL_Quit();
}