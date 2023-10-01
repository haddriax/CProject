#include "model.h"

// Default variable for easy [0;0] vector creation by copy.
const Vector2i vector2i_zero = { 0, 0 };
const Vector2f vector2f_zero = { 0.f, 0.f};

// Global container for the app.
RenderWindow render_window = { NULL, NULL, NULL };
App app = { NULL, NULL, 0, 0, NULL};

char* get_config_file_name(char** argv)
{
	return (argv && argv[1]) ? argv[1] : NULL;
}

Config load_config(const char *file_name)
{
	Config c = {{720, 480}, 100, 100, 0, 0, 2, 10 };
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
				char config_name[CONFIG_BUFFER_MAX_SIZE];
				
				// Process line
				// printf("%s\n", line);

				// 1/ Get config name.
				const int name_length = find_config_line_name(line, config_name);

				// 2/ Identify the data type we want to load, using config name
				const config_type t = find_config_type(config_name);

				// Get the data
				char *data = get_data_from_line(line, name_length);

				// 3/ Validate Data on the line, ex: matching type with our expectation
				// 3bis/ Facultative : Validate data with line number 
				const int data_validated = validate_config_line(data, t);
				if ( data_validated )
				{
					process_data(data, t);
					// 4/ Load data from line, validate data integrity (value are sane)

					
					// 5/ Put data into the config structure.

					// process_data(w1, w2);
				}
				else
				{
					// printf("%s\n", "Not valid.");
				}

				free(data);
			}
			const int close = fclose(file);
		}
	}

	return c;
}

int find_config_line_name(const char* line, char* out_config_name)
{
	int char_index = 0;
	while ( line[char_index] != SEPARATOR_SPACE && char_index < CONFIG_BUFFER_MAX_SIZE )
		++char_index;
	const size_t config_name_length = SDL_strlcpy(out_config_name, line, ++char_index);

	return char_index;
}

config_type find_config_type(const char* config_name)
{
	if (SDL_strcmp(config_name, CONFIG_NAME_WINDOW_SIZE) == 0)
		return window_size;
	if (SDL_strcmp(config_name, CONFIG_NAME_PLAYER_LOCATION) == 0)
		return player_location;
	if (SDL_strcmp(config_name, CONFIG_NAME_GOAL_LOCATION) == 0)
		return goal_location;
	else
	{
		printf("%s%s\n", config_name, ": Config not handled yet.");
	}
	return none;
}

char* get_data_from_line(const char* line, const int data_start)
{
	const size_t data_length = (SDL_strlen(line) - data_start - 1); // Minus 1 to remove '\n' char at then end
	char* data = calloc(data_length + 1, sizeof(char)); // Allocate + 1 to add '\0' later
	if (data != NULL)
	{
		SDL_memcpy(data, &line[data_start], data_length); // Copy line data
		data[data_length] = '\0'; // Add the end of string character
	}
	else
	{
		printf("%s: %s\n", line, "could not copy data from configs.");
		return 0;
	}

	return data;
}

int validate_config_line(const char* data, const config_type t)
{		
	switch (t)
	{
	case window_size:
	case player_location:
	case goal_location:
		int space_counter = 0, validate_char = 1;
		for (size_t i = 0; i < SDL_strlen(data); ++i)
		{
			if (SDL_isspace(data[i])) // Want exactly one space
				++space_counter;

			if (SDL_isalpha(data[i])) // Want no alpha char
				validate_char = 0;
		}
		return (space_counter == 1) && validate_char;
	case nb_solar_system:
		break;
	case star_pos:
		break;
	case star_radius:
		break;
	case nb_planet:
		break;
	case planet_radius:
		break;
	case planet_orbit:
		break;
	case none:
		break;
	default:
		return 1;
	}

	return 0;
}

void process_data(const char* data, const config_type t)
{
	switch (t)
	{
	case window_size:
		app.config->window_size = read_vector(data);
		printf("Window size set to %i:%i\n", app.config->window_size.x, app.config->window_size.y);
		break;
	case player_location:
		app.config->player_start = read_vector(data);
		printf("Player start set to %i:%i\n", app.config->player_start.x, app.config->player_start.y);
		break;
	case goal_location:
		break;
	case nb_solar_system:
		break;
	case star_pos:
		break;
	case star_radius:
		break;
	case nb_planet:
		break;
	case planet_radius:
		break;
	case planet_orbit:
		break;
	case none:
		break;
	default: ;
	}
}

Vector2i read_vector(char* data)
{
	// Find space separating x and y value
	int char_index = 0;
	while (data[char_index] != SEPARATOR_SPACE && char_index < CONFIG_BUFFER_MAX_SIZE)
		++char_index;

	const int data_length = (int)SDL_strlen(data);

	// Get char until separator index is found.
	char x[8];
	int i = 0;
	for (; i < char_index; ++i)
		x[i] = data[i];
	x[i + 1] = '\0';

	// Get char until end of string is found.
	char y[8];
	int j = 0;
	for (; j < (data_length - char_index + 1); ++j)
		y[j] = data[(char_index + 1) + j];
	y[j + 1] = '\0';

	// Convert to numeric value (long) and cast to int.
	const int x_coords = (int)SDL_strtol(x, NULL, 10);
	const int y_coords = (int)SDL_strtol(y, NULL, 10);

	const Vector2i v = { x_coords, y_coords };
	return v;
}

void keep_player_on_screen(void)
{
	Player* p = app.player;
	const int player_width = app.config->player_size;

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

	// Player struct creation & assignation
	// Player p = { {250, 250}, vector2i_zero };
	Player* p_ptr = (Player*)calloc(1, sizeof(Player));
	if (p_ptr != NULL)
	{
		p_ptr->location.x = 500;
		p_ptr->location.y = 450;
		p_ptr->thrust = vector2f_zero;
		app.player = p_ptr;
	}
	else
	{
		printf("%s\n"," Error when allocating memory for Player struct");
		abort();
	}

	if (argv != NULL)
	{
		const char *config_file_name = get_config_file_name(argv);
		init_config(config_file_name);
		app.player->location = app.config->player_start;
	}

	//Create and initialize display. @TODO parameters should be read from config.txt
	init_render_window(
		&render_window,
		app.config->window_size.x, 
		app.config->window_size.x,
		APP_DEFAULT_NAME
	);
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
}

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

void physic_update(void)
{
	// keep_player_on_screen();
}

