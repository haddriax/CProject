#include "vue_controller.h"


const struct SDL_Color red = {0xFF, 0x00, 0x00, SDL_ALPHA_OPAQUE};
const struct SDL_Color green = {0x00, 0xFF, 0x00, SDL_ALPHA_OPAQUE};
const struct SDL_Color blue = {0x00, 0x00, 0xFF, SDL_ALPHA_OPAQUE};
const struct SDL_Color cyan = {0x00, 0xFF, 0xFF, SDL_ALPHA_OPAQUE};
const struct SDL_Color black = {0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE};
const struct SDL_Color white = {0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE};
const struct SDL_Color grey = {0x40, 0x40, 0x40, SDL_ALPHA_OPAQUE};
const struct SDL_Color yellow = {0xFA, 0xFA, 0x37, SDL_ALPHA_OPAQUE};

KeyFlags key_flags = {0, 0, 0, 0, 0};

int handle_inputs(void) {
    // Static <=> this variable exists only ONE time in the app.
    static SDL_Event e;

    SDL_GetMouseState(&app.mouse_x, &app.mouse_y);

    // Input loop
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_QUIT:
                return 0;
            case SDL_KEYDOWN:
                keyboard_key_down(&e.key);
                break;
            case SDL_KEYUP:
                keyboard_key_up(&e.key);
                break;
            default:
                break;
        }
    }
    return 1;
}

void keyboard_key_down(const SDL_KeyboardEvent *key_event) {
    Player *p = app.entities->player;
    switch (key_event->keysym.sym) {
        case SDLK_RIGHT:
        case SDLK_d:
            key_flags.right = 1;
            break;
        case SDLK_LEFT:
        case SDLK_q:
            key_flags.left = 1;
            break;
        case SDLK_UP:
            key_flags.up = 1;
            break;
        case SDLK_DOWN:
            key_flags.down = 1;
            break;
        case SDLK_ESCAPE:
            quit(Exit, NULL);
            break;
        case SDLK_SPACE:
            key_flags.space = 1;
            // On SPACE pressed, start the simulation.
            if (app.simulation_started == 0) start_simulation();
    }
}

void keyboard_key_up(const SDL_KeyboardEvent *key_event) {
    Player *p = app.entities->player;
    switch (key_event->keysym.sym) {
        case SDLK_RIGHT:
        case SDLK_d:
            key_flags.right = 0;
            break;
        case SDLK_LEFT:
        case SDLK_q:
            key_flags.left = 0;
            break;
        case SDLK_UP:
            key_flags.up = 0;
            break;
        case SDLK_DOWN:
            key_flags.down = 0;
            break;
        case SDLK_SPACE:
            key_flags.space = 0;
        default:
            break;
    }
}

void update_window_name(const int framerate) {
    static char win_name_buffer[128];
    if (sprintf_s(win_name_buffer, 128, "%s | FPS : %i | Score : %i", APP_DEFAULT_NAME, framerate, app.score))
        SDL_SetWindowTitle(render_window.sdl_win, win_name_buffer);
}

int draw_circle(SDL_Renderer *renderer, const int32_t centre_x, const int32_t centre_y, const int32_t radius) {
    const int32_t diameter = (radius * 2);

    int32_t x = (radius - 1);
    int32_t y = 0;
    int32_t tx = 1;
    int32_t ty = 1;
    int32_t error = (tx - diameter);

    while (x >= y) {
        //  Each of the following renders an octant of the circle
        SDL_RenderDrawPoint(renderer, centre_x + x, centre_y - y);
        SDL_RenderDrawPoint(renderer, centre_x + x, centre_y + y);
        SDL_RenderDrawPoint(renderer, centre_x - x, centre_y - y);
        SDL_RenderDrawPoint(renderer, centre_x - x, centre_y + y);
        SDL_RenderDrawPoint(renderer, centre_x + y, centre_y - x);
        SDL_RenderDrawPoint(renderer, centre_x + y, centre_y + x);
        SDL_RenderDrawPoint(renderer, centre_x - y, centre_y - x);
        SDL_RenderDrawPoint(renderer, centre_x - y, centre_y + x);

        if (error <= 0) {
            ++y;
            error += ty;
            ty += 2;
        }

        if (error > 0) {
            --x;
            tx += 2;
            error += (tx - diameter);
        }
    }

    return 1;
}

int render_fill_circle(SDL_Renderer *renderer, int x, int y, int radius) {
    int offsetx, offsety, d;
    int status;

    offsetx = 0;
    offsety = radius;
    d = radius - 1;
    status = 0;

    while (offsety >= offsetx) {

        status += SDL_RenderDrawLine(renderer, x - offsety, y + offsetx, x + offsety, y + offsetx);
        status += SDL_RenderDrawLine(renderer, x - offsetx, y + offsety, x + offsetx, y + offsety);
        status += SDL_RenderDrawLine(renderer, x - offsetx, y - offsety, x + offsetx, y - offsety);
        status += SDL_RenderDrawLine(renderer, x - offsety, y - offsetx, x + offsety, y - offsetx);

        if (status < 0) {
            status = -1;
            break;
        }

        if (d >= 2 * offsetx) {
            d -= 2 * offsetx + 1;
            offsetx += 1;
        } else if (d < 2 * (radius - offsety)) {
            d += 2 * offsety - 1;
            offsety -= 1;
        } else {
            d += 2 * (offsety - offsetx - 1);
            offsety -= 1;
            offsetx += 1;
        }
    }

    return status;
}

void render_systems(void) {
    int i = app.entities->nb_solar_systems;
    while (i-- > 0) {
        // Render Star
        const SolarSystem *s = (app.entities->solar_systems[i]);
        assert(s != NULL);
        SDL_SetRenderDrawColor(render_window.sdl_renderer, COLOR_PARAMS(yellow));
        render_fill_circle(render_window.sdl_renderer, (int) s->location.x, (int) s->location.y, (int) s->radius);

        // Render Planets & orbits
        for (int j = 0; j < s->nb_planets; ++j) {
            const Planet *p = &(s->planets[j]);
            assert(p != NULL);
            // Draw orbit path
            SDL_SetRenderDrawColor(render_window.sdl_renderer, COLOR_PARAMS(grey));
            draw_circle(render_window.sdl_renderer, (int) s->location.x, (int) s->location.y,
                        (int) (p->orbit < 0 ? (-p->orbit) : p->orbit));
            // Draw Planet
            SDL_SetRenderDrawColor(render_window.sdl_renderer, COLOR_PARAMS(cyan));
            render_fill_circle(render_window.sdl_renderer, (int) p->location.x, (int) p->location.y, (int) p->radius);
        }
    }
}

void render_player(void) {
    Player *p = app.entities->player;

    // Only if velocity vector approximate length is slightly bigger than zero.
    if (fabsf(p->velocity.x + p->velocity.y) >= 0.05f) {
        // Render velocity vector.
        SDL_SetRenderDrawColor(render_window.sdl_renderer, COLOR_PARAMS(blue));
        SDL_RenderDrawLineF(render_window.sdl_renderer, p->location.x, p->location.y,
                            (p->location.x + p->velocity.x * 25), (p->location.y + p->velocity.y * 25));


        // Render orthogonal vector - left.
        SDL_SetRenderDrawColor(render_window.sdl_renderer, COLOR_PARAMS(red));
        SDL_RenderDrawLineF(render_window.sdl_renderer, p->location.x, p->location.y,
                            (p->location.x + (p->velocity.y * 25)), (p->location.y - (p->velocity.x * 25)));
        // Render orthogonal vector - right.
        SDL_SetRenderDrawColor(render_window.sdl_renderer, COLOR_PARAMS(green));
        SDL_RenderDrawLineF(render_window.sdl_renderer, p->location.x, p->location.y,
                            (p->location.x - (p->velocity.y * 25)), (p->location.y + (p->velocity.x * 25)));
    }

    SDL_SetRenderDrawColor(render_window.sdl_renderer, COLOR_PARAMS(red));
    if (SDL_RenderFillRectF(render_window.sdl_renderer, &(p->draw_rect)) == -1) {
        fprintf(stderr, "%s: %s\n", "SDL could not render Player.",
                SDL_GetError());  // NOLINT(cert-err33-c) - Error Output
    }
}

void render_end(void) {
    SDL_SetRenderDrawColor(render_window.sdl_renderer, COLOR_PARAMS(white));
    SDL_RenderDrawRectF(render_window.sdl_renderer, app.entities->end); // Render Goal.
    // @todo check if offset for center is applied.
}

void render_border(void) {
    SDL_SetRenderDrawColor(render_window.sdl_renderer, COLOR_PARAMS(white));
    // Border w and h is -20, because it cumulates both the margin and the offset.
    SDL_Rect border = {10, 10, (int) app.config->window_size.x - 20, (int) app.config->window_size.y - 20};
    SDL_RenderDrawRect(render_window.sdl_renderer, &border); // Render Goal.
}

void render_clear(void) {
    SDL_SetRenderDrawColor(render_window.sdl_renderer, COLOR_PARAMS(black));
    SDL_RenderClear(render_window.sdl_renderer);
}

void render(void) {
    if (render_window.sdl_renderer != NULL) {
        render_clear();
        render_end();
        render_systems();
        render_border();
        render_player();
        // Finalize rendering.
        SDL_RenderPresent(render_window.sdl_renderer);
    } else {
        char error_log[128];
        sprintf(error_log, "%s\n",
                "Error in rendering, pointer to SDL_Renderer is NULL."); // NOLINT(cert-err33-c) - Error Output
        quit(Error, error_log);
    }
}

