#include "vue_controller/vue_controller.h"

void quit_app(void)
{
    SDL_DestroyWindow(render_window.sdl_win);
    SDL_DestroyRenderer(render_window.sdl_renderer);
    SDL_Quit();


    if (app.entities->end) free(app.entities->end);
    if (app.entities->player) free(app.entities->player);
    if (app.entities->solar_systems) free(app.entities->solar_systems);
    if (app.entities) free(app.entities);
}

int main (int argc, char** argv)
{
    // Remember : await config.txt path to be first user passed argument.
    init_app(argc, argv);

    Clock clock;
    clock.last_time = SDL_GetTicks64();
    // clock.current_time = 0;
    // clock.delta_ticks = 0;

    // Expected time for a frame, in milliseconds.
    const Uint64 frame_duration_ms = (1000 / 60);

    while ( handle_inputs() )
    {
        clock.current_time = SDL_GetTicks64();
        clock.delta_ticks = (clock.current_time - clock.last_time);

        // If the last time we updated is old enough, so we respect targeted framerate.
        // i.e. code in this if() section is frame capped.
        if (frame_duration_ms < clock.delta_ticks)
        {
            // Display FPS in the render_window title.
            // update_window_name(&render_window, "Hello: ", (float)clock.delta_ticks);
            // Gameplay update
            game_loop();
            // Drawing shapes
            render();

            // Remember the time at which this frame end.
            clock.last_time = SDL_GetTicks64();
        }
    }

    quit_app();

    return 0;
}
