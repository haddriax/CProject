//
// Created by hadriax on 22/09/23.
//

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
			
			break;

		case SDL_KEYUP:
			
			break;
		default:
			break;
		}
	}

	return 1;
}

void update_window_name(RenderWindow *rw, char* name, float framerate)
{
	static char buffer[256];
	// sprintf(buffer, "Projet C : Mohammed - Maelys - Gael | fps:%g", framerate);
	sprintf(buffer, "Projet C : %g", framerate);
	SDL_SetWindowTitle(rw->sdl_win, buffer);
}

void draw_rectangle(RenderWindow* rw,int coord_x, int coord_y, int width, int height, const SDL_Color* color)
{
	SDL_Rect fillRect = { coord_x, coord_y, width, height };
	SDL_SetRenderDrawColor(rw->sdl_renderer, color->r, color->g, color->b, color->a);
	SDL_RenderFillRect(rw->sdl_renderer, &fillRect);
}

void draw_line(RenderWindow* rw, int x_begin, int y_begin, int x_end, int y_end, const SDL_Color* color)
{
	SDL_SetRenderDrawColor(rw->sdl_renderer, color->r, color->g, color->b, color->a);
	SDL_RenderDrawLine(rw, x_begin, y_begin, x_end, y_end);
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

			// Render Player
			draw_rectangle(rw, app.player->location.x, app.player->location.y, PLAYER_SIZE, PLAYER_SIZE, &red);

			// TEST DRAW LINE
			SDL_SetRenderDrawColor(rw->sdl_renderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
			if (!SDL_RenderDrawLine(rw, 0, 0, 450, 200))
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