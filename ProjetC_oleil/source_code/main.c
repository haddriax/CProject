#include "vue_controller/vue_controller.h"

// Expected time for a frame, in milliseconds.
#define TARGET_FRAME_DURATION (1000 / 60)

int main (int argc, char** argv)
{
    // Counter to avoid changing the window name each frame.
    int frame_counter_fps_display;

    // Remember : await config.txt path to be first user passed argument.
    init_app(argc, argv);

    uint64_t t_end_frame = (SDL_GetTicks64() - TARGET_FRAME_DURATION);

    // Keep checking if there is any inputs, even outside of rendering.
    while ( handle_inputs() )
    {
        // Store begin frame time, in ms since app launched.
        uint64_t t_begin_frame = SDL_GetTicks64();

        // Skip the frame if it does not match our target framerate.
        // i.e. if we compare <last frame end time> with <new time>.
        if (t_begin_frame - t_end_frame < TARGET_FRAME_DURATION)
            continue;

        // Update app delta time. (value used in calculations)
        app.delta_time = (int)(t_begin_frame - t_end_frame);

        if (++frame_counter_fps_display == 5)
        {
            // Display FPS in the render_window title, every 5 frames.
            const int framerate = 1000 /(int)(t_begin_frame - t_end_frame);
            update_window_name(framerate);
            frame_counter_fps_display = 0;
        }

        // Gameplay update
        game_loop((float)app.delta_time);

        // Drawing shapes
        render();

        // Store end frame time, in ms since app launched.
        t_end_frame = SDL_GetTicks64();
    }

    quit(Exit, NULL);
    return 0;
}
