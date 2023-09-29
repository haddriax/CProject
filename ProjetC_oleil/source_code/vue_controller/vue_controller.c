#include "vue_controller.h"

#define PLAYER_SIZE 10

const struct SDL_Color red = { 0xFF, 0x00, 0x00, SDL_ALPHA_OPAQUE };
const struct SDL_Color green = { 0x00, 0xFF, 0x00, SDL_ALPHA_OPAQUE };
const struct SDL_Color blue = { 0x00, 0x00, 0xFF, SDL_ALPHA_OPAQUE };

int handle_inputs()
{
	// Static <=> this variable exist only ONE time in app.
	static SDL_Event e;

	SDL_GetMouseState(&app.mouse_x, &app.mouse_y);
	// printf("Mouse coordinates: x:%i%|y:%i\n", app.mouse_x, app.mouse_y);

	// Input loop
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		case SDL_QUIT:
			return 0;
			break;
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
	switch (key_event->keysym.sym)
	{
		case SDLK_RIGHT:
			app.player->thrust.x = 1;
			break;
		case SDLK_LEFT:
			app.player->thrust.x = -1;
			break;	
		case SDLK_UP:
			app.player->thrust.y = -1;
			break;	
		case SDLK_DOWN:
			app.player->thrust.y = 1;
			break;
		default:
			break;
	}
}

void keyboard_key_up(const SDL_KeyboardEvent* key_event)
{
	switch (key_event->keysym.sym)
	{
		case SDLK_RIGHT:
		case SDLK_LEFT:
			app.player->thrust.x = 0.f;
			app.player->thrust.x = 0.f;
			break;
		case SDLK_UP:
		case SDLK_DOWN:
			app.player->thrust.y = 0.f;
			app.player->thrust.y = 0.f;
			break;
		default:
			break;
	}
}

void update_window_name(const RenderWindow *rw, char* name, const float framerate)
{
	static char buffer[256];
	// sprintf(buffer, "Projet C : Mohammed - Maelys - Gael | fps:%g", framerate);
	sprintf(buffer, "Projet C : %g", framerate);
	SDL_SetWindowTitle(rw->sdl_win, buffer);
}

void draw_rectangle(const RenderWindow* rw,int coord_x, int coord_y, int width, int height, const SDL_Color* color)
{
	const SDL_Rect fill_rect = { coord_x, coord_y, width, height };
	SDL_SetRenderDrawColor(rw->sdl_renderer, color->r, color->g, color->b, color->a);
	SDL_RenderFillRect(rw->sdl_renderer, &fill_rect);
}

int draw_line(int x_begin, int y_begin, int x_end, int y_end, const SDL_Color* color)
{
	SDL_SetRenderDrawColor(render_window.sdl_renderer, color->r, color->g, color->b, color->a);
	return SDL_RenderDrawLine(render_window.sdl_renderer, x_begin, y_begin, x_end, y_end);
}

int draw_line_v(const Vector2i *begin, const Vector2i *end, const SDL_Color *color)
{
	SDL_SetRenderDrawColor(render_window.sdl_renderer, color->r, color->g, color->b, color->a);
	return SDL_RenderDrawLine(render_window.sdl_renderer, begin->x, begin->y, end->x, end->y);
}

int draw_line_debug(const Vector2i* begin, const Vector2i* end)
{
	SDL_SetRenderDrawColor(render_window.sdl_renderer, red.r, red.g, red.b, red.a);
	return SDL_RenderDrawLine(render_window.sdl_renderer, begin->x, begin->y, end->x, end->y);
}


void render_planets(RenderWindow* rw)
{

}

void render_player(const RenderWindow* rw)
{
	draw_rectangle(rw,
		(float)app.player->location.x,
		(float)app.player->location.y, 
		PLAYER_SIZE, 
		PLAYER_SIZE,
		&red
	);
}

void render(RenderWindow* rw)
{
	if ( rw != NULL )
	{
		if (rw->sdl_renderer != NULL)
		{
			SDL_SetRenderDrawColor(rw->sdl_renderer, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
			SDL_RenderClear(rw->sdl_renderer);
			// SDL_FillRect(rw->sdl_surface, NULL, SDL_MapRGB(rw->sdl_surface->format, 0x00, 0xFF, 0x00));

			SDL_UpdateWindowSurface(rw->sdl_win);

			render_planets(rw);

			render_player(rw);

			// TEST DRAW LINE
			SDL_SetRenderDrawColor(rw->sdl_renderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);

			if (SDL_RenderDrawLine(rw->sdl_renderer, 0, 0, 450, 200) > 0)
				printf("Failed drawing line: %s\n", SDL_GetError());

			// draw_line(rw, 50, 200, 450, 200, &green);

			SDL_RenderPresent(rw->sdl_renderer);
		}
		else
		{
			printf("Error in rendering, pointer to SDL_Renderer is NULL.");
		}
	}
	else
	{
		printf("Error in rendering, pointer to RenderWindow is NULL.");
	}	
}