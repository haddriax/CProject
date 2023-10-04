#include "vue_controller.h"


const struct SDL_Color red = { 0xFF, 0x00, 0x00, SDL_ALPHA_OPAQUE };
const struct SDL_Color green = { 0x00, 0xFF, 0x00, SDL_ALPHA_OPAQUE };
const struct SDL_Color blue = { 0x00, 0x00, 0xFF, SDL_ALPHA_OPAQUE };
const struct SDL_Color black = { 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE };
const struct SDL_Color white = { 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE };

int handle_inputs(void)
{
	// Static <=> this variable exist only ONE time in app.
	static SDL_Event e;

	SDL_GetMouseState(&app.mouse_x, &app.mouse_y);

	// Input loop
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
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

void keyboard_key_down(const SDL_KeyboardEvent* key_event)
{
	Player* p = app.entities->player;
	switch (key_event->keysym.sym)
	{
		case SDLK_RIGHT:
			p->velocity.x = 1;
			break;
		case SDLK_LEFT:
			p->velocity.x = -1;
			break;	
		case SDLK_UP:
			p->velocity.y = -1;
			break;	
		case SDLK_DOWN:
			p->velocity.y = 1;
			break;
	case SDLK_SPACE:
		app.simulation_started = 1; // On SPACE pressed, start the simulation.
		default:
			break;
	}
}

void keyboard_key_up(const SDL_KeyboardEvent* key_event)
{
	Player* p = app.entities->player;
	switch (key_event->keysym.sym)
	{
		case SDLK_RIGHT:
		case SDLK_LEFT:
			p->velocity.x = 0.f;
			p->velocity.x = 0.f;
			break;
		case SDLK_UP:
		case SDLK_DOWN:
			p->velocity.y = 0.f;
			p->velocity.y = 0.f;
			break;
		default:
			break;
	}
}

void update_window_name(const RenderWindow *rw, char* name, const float framerate)
{
	static char buffer[256];
	if (sprintf_s(buffer, 256, "Projet C : %g", framerate))
		SDL_SetWindowTitle(rw->sdl_win, buffer);
}

void draw_rectangle(int coord_x, int coord_y, int width, int height, const SDL_Color* color)
{
	const SDL_Rect fill_rect = { coord_x, coord_y, width, height };
	SDL_SetRenderDrawColor(render_window.sdl_renderer, color->r, color->g, color->b, color->a);
	SDL_RenderFillRect(render_window.sdl_renderer, &fill_rect);
}

int draw_line(int x_begin, int y_begin, int x_end, int y_end, const SDL_Color* color)
{
	SDL_SetRenderDrawColor(render_window.sdl_renderer, color->r, color->g, color->b, color->a);
	return SDL_RenderDrawLine(render_window.sdl_renderer, x_begin, y_begin, x_end, y_end);
}

int draw_line_p(const SDL_FPoint* begin, const SDL_FPoint* end, const SDL_Color *color)
{
	SDL_SetRenderDrawColor(render_window.sdl_renderer, color->r, color->g, color->b, color->a);
	return SDL_RenderDrawLineF(render_window.sdl_renderer, begin->x, begin->y, end->x, end->y);
}

int draw_line_debug(const Vector2i* begin, const Vector2i* end)
{
	SDL_SetRenderDrawColor(render_window.sdl_renderer, COLOR_PARAMS(red));
	return SDL_RenderDrawLine(render_window.sdl_renderer, begin->x, begin->y, end->x, end->y);
}


void render_planets(void)
{

}

void render_player(void)
{
	SDL_SetRenderDrawColor(render_window.sdl_renderer, COLOR_PARAMS(red));
	if ( SDL_RenderFillRectF(render_window.sdl_renderer, &app.entities->player->draw_rect) == -1)
	{
		printf("SDL could not render Player. %s\n", SDL_GetError());
	}
}

void render(void)
{
		if (render_window.sdl_renderer != NULL)
		{
			SDL_SetRenderDrawColor(render_window.sdl_renderer, COLOR_PARAMS(black));
			SDL_RenderClear(render_window.sdl_renderer);

			SDL_UpdateWindowSurface(render_window.sdl_win);

			render_planets();

			render_player();

			SDL_SetRenderDrawColor(render_window.sdl_renderer, COLOR_PARAMS(white));
			SDL_RenderFillRect(render_window.sdl_renderer, app.entities->end); // Render Goal. @todo add offset to center it's coord.

			// TEST DRAW LINE
			SDL_RenderPresent(render_window.sdl_renderer);
		}
		else
		{
			printf("Error in rendering, pointer to SDL_Renderer is NULL.");
		}
}