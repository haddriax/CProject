#include "model.h"

#define MAX_CONFIG_LINES 64
#define MAX_CONFIG_BUFFER_SIZE 256

// Default variable for easy [0;0] vector creation by copy.
const Vector2f vector2f_zero = { 0.0f, 0.0f };

// Global container for the app.
App app = { NULL, NULL, NULL, 0, 0 };

Config load_config(char* file_name)
{
	Config c;
	if (file_name != NULL)
	{
		FILE* f = NULL;
		f = fopen(file_name, 'r');

		if (f != NULL)
		{
			// Buffer for reading line from the file.
			char buffer[MAX_CONFIG_BUFFER_SIZE];

			while (fgets(buffer, MAX_CONFIG_LINES, f))
			{

			}

			fclose(f);
		}

	}
	return c;
}

void init_RenderWindow(RenderWindow *rw, int w, int h, char *name)
{
	// Set pointer to RenderWindow in Game;   
	rw->sdl_win = NULL;
	rw->sdl_surface = NULL;
	rw->sdl_renderer = NULL;

	if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
	{
		printf(" 1 SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}
	else
	{
		SDL_Window *window = rw->sdl_win = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
		if (window == NULL)
		{
			printf("2 SDL could not initialize! SDL_CreateWindow returned NULL: %s\n", SDL_GetError());
			exit(1);
		}

		SDL_Renderer *renderer = rw->sdl_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
		if (renderer == NULL)
		{
			printf("3 SDL could not initialize! SDL_CreateRenderer returned NULL: %s\n", SDL_GetError());
			exit(1);
		}

		SDL_Surface *surface = rw->sdl_surface = SDL_GetWindowSurface(window);
		if (surface == NULL)
		{
			printf("4 SDL could not initialize! SDL_GetWindowSurface returned NULL: %s\n", SDL_GetError());
			exit(1);
		}
	}
}

struct Vector2f vector_add(const Vector2f *v1, const Vector2f *v2)
{
	Vector2f vec;
	vec.x = (v1->x + v2->x);
	vec.y = (v1->y + v2->y);
	return vec;
};

struct Vector2f sub_vector(const Vector2f *v1, const Vector2f *v2)
{
	Vector2f vec;
	vec.x = (v1->x - v2->x);
	vec.y = (v1->y - v2->y);
	return vec;
}

float dot_product(const Vector2f *v1, const Vector2f *v2)
{
	return ((v1->x * v2->x) + (v1->y * v2->y));
}

void gameloop()
{
	physic_update();
}

void physic_update()
{
	app.player->location.x += app.player->thrust.x;
	app.player->location.y += app.player->thrust.y;
}

