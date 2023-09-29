#include "model.h"

// Default variable for easy [0;0] vector creation by copy.
const Vector2i vector2i_zero = { 0, 0 };

// Global container for the app.
RenderWindow render_window = { NULL, NULL, NULL };
App app = { NULL, NULL, 0, 0, NULL};

char* get_config_name(char** argv)
{
	if (argv[1])
		return argv[1];
	else
		return NULL;
}

Config load_config(const char *file_name)
{
	Config c = {{720, 480}, 0, 0, 0, 0, 2, 10 };
	app.config = &c;

	assert(file_name);
	if (file_name != NULL)
	{
		// Open file.
		FILE* file = NULL;
		const errno_t e = fopen_s(&file, file_name, FILE_MODE_READONLY);
		if ( e ) // ERROR HANDLING - OPENING FAILED.
		{		
			printf("%s\n","Error when opening config file.");
			abort();
		}

		// Read from file.
		if (file != NULL)
		{
			// Buffer for reading line from the file.
			char line[max_config_buffer_size];
			int line_index = 0;

			while ( fgets(line, max_config_lines, file) )
			{
				char w1[16];
				char w2[16];
				char w3[16];

				// Process line.

				// 1/ Get words.
				int char_index = 0;
				while ( line[char_index] != ' ' && char_index < 16)
					++char_index;
				const size_t config_name_length = SDL_strlcpy(w1, line, ++char_index);
				printf("Loading config: %s\n", w1);

				// @todo CONFIG DATA PROCESSING
				// 2/ Identify the data type we want to load, using config name

				// 3/ Validate Data on the line, ex: matching type with our expectation

				// 3bis/ Facultative : Validate data with line number 

				// 4/ Load data from line, validate data integrity (value are sane)

				// 5/ Put data into the config structure.

				// process_config_line(w1, w2);
			}
			const int close = fclose(file);
		}
	}
	return c;
}

void process_config_line(const char* config_name, const char* data)
{
	// Put the "looping" lines (like planets data) first.

	// Compare the config_name argument with the config expected names.
	if ( SDL_strcmp(config_name, CONFIG_NAME_WINDOW_SIZE) != 0)
	{
		app.config->window_size = read_vector(data);
	}
	else
	{
		printf("%s%s\n", config_name, ": Processing this config is not handled. Check the name.");
	}
}

Vector2i read_vector(char* line)
{
	printf("%s|%s|\n", "Reading vector2...",line);

	return vector2i_zero;
}

void keep_player_on_screen(void)
{
	Player* p = app.player;
	const int player_width = app.config->player_size; // @todo change -10 to Player width

	// Left (x) boundary.
	if (p->location.x < 0)
		p->location.x = app.config->window_size.x - player_width;

	// Right (x) boundary.
	if (p->location.x + player_width > app.config->window_size.x)
		p->location.x = 0;

	// Up (y) boundary.
	if (p->location.y < 0)
		p->location.y = app.config->window_size.y - player_width;

	// Down (y) boundary.
	if (p->location.y + player_width > app.config->window_size.y)
		p->location.y = 0;
}


void init_app(const int n_args, char ** argv)
{
	if (n_args < 1)
	{
		printf("No config file argument detected.");
		abort();
	}

	Player p = { {0, 0}, vector2i_zero };
	app.player = &p;


	if (argv != NULL)
	{
		const char *config_file_name = get_config_name(argv);
		init_config(config_file_name);
	}

	//Create and initialize display. @TODO parameters should be read from config.txt
	init_render_window(&render_window, 720, 480, APP_DEFAULT_NAME);
}

void init_config(const char* file_name)
{
	if (file_name == NULL)
	{
		printf("config_file_name in init_app was NULL");
		abort();
	}
	const Config c = load_config(file_name);

	assert(c.window_size.x > 0);
	assert(c.window_size.y > 0);
	assert(c.nb_solar_systems > 0);
	assert(c.player_size > 0);

	// app.config = &c; // For now, done in load_config
}

void init_render_window(RenderWindow *rw, const int width, const int height, const char *name)
{
	// Set pointer to RenderWindow in Game;   
	rw->sdl_win = NULL;
	rw->sdl_surface = NULL;
	rw->sdl_renderer = NULL;

	if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
	{
		printf(" 1 SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		abort();
	}
	else
	{
		SDL_Window* window = rw->sdl_win = 
			SDL_CreateWindow(
				name, 
				SDL_WINDOWPOS_CENTERED, 
				SDL_WINDOWPOS_CENTERED, 
				width,
				height,
			SDL_WINDOW_SHOWN
			);

		if (window == NULL)
		{
			printf("2 SDL could not initialize! SDL_CreateWindow returned NULL: %s\n", SDL_GetError());
			abort();
		}

		const SDL_Renderer *renderer = rw->sdl_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
		if (renderer == NULL)
		{
			printf("3 SDL could not initialize! SDL_CreateRenderer returned NULL: %s\n", SDL_GetError());
			abort();
		}

		const SDL_Surface *surface = rw->sdl_surface = SDL_GetWindowSurface(window);
		if (surface == NULL)
		{
			printf("4 SDL could not initialize! SDL_GetWindowSurface returned NULL: %s\n", SDL_GetError());
			abort();
		}
	}
}

struct Vector2i vector_add(const Vector2i *v1, const Vector2i *v2)
{
	Vector2i vec;
	vec.x = (v1->x + v2->x);
	vec.y = (v1->y + v2->y);
	return vec;
};

struct Vector2i vector_sub(const Vector2i *v1, const Vector2i *v2)
{
	Vector2i vec;
	vec.x = (v1->x - v2->x);
	vec.y = (v1->y - v2->y);
	return vec;
}

float dot_product(const Vector2i *v1, const Vector2i *v2)
{
	return (float)((v1->x * v2->x) + (v1->y * v2->y));
}

void game_loop(void)
{
	physic_update();
}

void physic_update()
{
	app.player->location.x += (app.player->thrust.x * 5);
	app.player->location.y += (app.player->thrust.y * 6);

	keep_player_on_screen();
}

