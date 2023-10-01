#include "vue_controller/vue_controller.h"

int main (int argc, char** argv)
{
    // Remember : await config.txt path to be first user passed argument.
    init_app(argc, argv);
    assert(app.config != NULL);
    assert(app.player != NULL);

    Clock clock;
    clock.last_time = SDL_GetTicks64();
    clock.current_time = 0;
    clock.delta_ticks = 0;

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
            render(&render_window);

            // Remember the time at which this frame end.
            clock.last_time = SDL_GetTicks64();
        }
    }

    // Cleanly quit SDL.
    SDL_DestroyWindow(render_window.sdl_win);
    SDL_DestroyRenderer(render_window.sdl_renderer);
    SDL_Quit();

    free(app.player);

    return 0;
}
