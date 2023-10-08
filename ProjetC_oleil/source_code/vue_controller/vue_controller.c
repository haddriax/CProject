#include "vue_controller.h"


const struct SDL_Color red = { 0xFF, 0x00, 0x00, SDL_ALPHA_OPAQUE };
const struct SDL_Color green = { 0x00, 0xFF, 0x00, SDL_ALPHA_OPAQUE };
const struct SDL_Color blue = { 0x00, 0x00, 0xFF, SDL_ALPHA_OPAQUE };
const struct SDL_Color black = { 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE };
const struct SDL_Color white = { 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE };
const struct SDL_Color yellow = { 0xFA, 0xFA, 0x37, SDL_ALPHA_OPAQUE };

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

int draw_circle(SDL_Renderer* renderer, const int32_t centre_x, const int32_t centre_y, const int32_t radius)
{
	const int32_t diameter = (radius * 2);

	int32_t x = (radius - 1);
	int32_t y = 0;
	int32_t tx = 1;
	int32_t ty = 1;
	int32_t error = (tx - diameter);

	while (x >= y)
	{
		//  Each of the following renders an octant of the circle
		SDL_RenderDrawPoint(renderer, centre_x + x, centre_y - y);
		SDL_RenderDrawPoint(renderer, centre_x + x, centre_y + y);
		SDL_RenderDrawPoint(renderer, centre_x - x, centre_y - y);
		SDL_RenderDrawPoint(renderer, centre_x - x, centre_y + y);
		SDL_RenderDrawPoint(renderer, centre_x + y, centre_y - x);
		SDL_RenderDrawPoint(renderer, centre_x + y, centre_y + x);
		SDL_RenderDrawPoint(renderer, centre_x - y, centre_y - x);
		SDL_RenderDrawPoint(renderer, centre_x - y, centre_y + x);

		if (error <= 0)
		{
			++y;
			error += ty;
			ty += 2;
		}

		if (error > 0)
		{
			--x;
			tx += 2;
			error += (tx - diameter);
		}
	}

	return 1;
}


int render_fill_circle(SDL_Renderer* renderer, int x, int y, int radius)
{
	int offsetx, offsety, d;
	int status;

	offsetx = 0;
	offsety = radius;
	d = radius - 1;
	status = 0;

	while (offsety >= offsetx) {

		status += SDL_RenderDrawLine(renderer, x - offsety, y + offsetx,
			x + offsety, y + offsetx);
		status += SDL_RenderDrawLine(renderer, x - offsetx, y + offsety,
			x + offsetx, y + offsety);
		status += SDL_RenderDrawLine(renderer, x - offsetx, y - offsety,
			x + offsetx, y - offsety);
		status += SDL_RenderDrawLine(renderer, x - offsety, y - offsetx,
			x + offsety, y - offsetx);

		if (status < 0) {
			status = -1;
			break;
		}

		if (d >= 2 * offsetx) {
			d -= 2 * offsetx + 1;
			offsetx += 1;
		}
		else if (d < 2 * (radius - offsety)) {
			d += 2 * offsety - 1;
			offsety -= 1;
		}
		else {
			d += 2 * (offsety - offsetx - 1);
			offsety -= 1;
			offsetx += 1;
		}
	}

	return status;
}

void render_systems(void)
{
	int i = app.entities->nb_solar_systems;
	while (i-->0)
	{
		assert(app.entities->solar_systems[i] != NULL);
		SDL_SetRenderDrawColor(render_window.sdl_renderer, COLOR_PARAMS(yellow));
		render_fill_circle(
			render_window.sdl_renderer,
			app.entities->solar_systems[i]->location.x,
			app.entities->solar_systems[i]->location.y,
			app.entities->solar_systems[i]->radius);
	}
}

void render_planets(void)
{

}

void render_player(void)
{
	SDL_SetRenderDrawColor(render_window.sdl_renderer, COLOR_PARAMS(red));
	if ( SDL_RenderFillRectF(render_window.sdl_renderer, &app.entities->player->draw_rect) == -1)
	{
		fprintf(stderr, "%s: %s\n", "SDL could not render Player.", SDL_GetError());  // NOLINT(cert-err33-c) - Error Output
	}
}

void render(void)
{
		if (render_window.sdl_renderer != NULL)
		{
			SDL_SetRenderDrawColor(render_window.sdl_renderer, COLOR_PARAMS(black));
			SDL_RenderClear(render_window.sdl_renderer);

			SDL_UpdateWindowSurface(render_window.sdl_win);

			render_systems();

			render_planets();

			render_player();

			SDL_SetRenderDrawColor(render_window.sdl_renderer, COLOR_PARAMS(white));
			SDL_RenderFillRect(render_window.sdl_renderer, app.entities->end); // Render Goal. @todo add offset to center it's coord
			
			SDL_RenderPresent(render_window.sdl_renderer);
		}
		else
		{
			fprintf(stderr, "%s\n", "Error in rendering, pointer to SDL_Renderer is NULL.");  // NOLINT(cert-err33-c) - Error Output
		}
}