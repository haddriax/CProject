#ifndef PROJECTC_OLEIL_MODEL_H
#define PROJECTC_OLEIL_MODEL_H

#include <SDL.h>
#include <stdio.h>
#include <assert.h>

#define CONFIG_DEFAULT_NAME "config.txt"
#define APP_DEFAULT_NAME "Projet C"

#define CONFIG_NAME_WINDOW_SIZE "WIN_SIZE"
#define CONFIG_NAME_PLAYER_LOCATION "START"
#define CONFIG_NAME_GOAL_LOCATION "END"

enum
{
	max_config_lines = 64,
	max_config_buffer_size = 256
};

#define FILE_MODE_READONLY "r"

#pragma region Vector2f

/**
Float Vector of Dimension 2 of coordinates x and y.
*/
typedef struct Vector2i
{
	int32_t x, y;
} Vector2i;
extern const Vector2i vector2i_zero;

/** @brief: Return a new Vector from the addition of the 2 parameters vectors.
@return: New Vector2f from (v1 + v2)
*/
Vector2i vector_add(const Vector2i* v1, const Vector2i* v2);

/** @brief: Return a new Vector from the substraction of the 2 parameters vectors.
@return: New Vector2f from (v1 - v2)
*/
Vector2i vector_sub(const Vector2i* v1, const Vector2i* v2);

/** @brief: Return the result of the dot product from v1 and v2.
@return: float (v1.v2)
*/
float dot_product(const Vector2i* v1, const Vector2i* v2);

/** @todo !
@brief: Scale v to a magnitude of 1
*/
void vector_normalize(Vector2i* v);

/** @todo !
@brief: Calculate the length of v. Calcul is heavy.
@return: Length of v.
*/
float vector_length(const Vector2i* v);

/** @todo !
@brief: Calculate the length² of v. Use it for comparison over vector_length
@return: Length of v.
*/
float vector_length_squared(const Vector2i* v);

#pragma endregion

typedef struct Clock
{
	Uint64 last_time; 
	Uint64 current_time;
	Uint64 delta_ticks;
} Clock;

/* @brief: Container for Player data */
typedef struct Player
{
	Vector2i location;
	Vector2i thrust;

} Player;

typedef struct Planet
{
	SDL_Point location;
	int radius;
	int orbit;

} Planet;

typedef struct SolarSystem
{
	SDL_Point location;
	int radius;
	int nb_planets;
	Planet* planets;

} SolarSystem;


/** @brief: Contain the data from the Config.txt file on disk */
typedef struct Config
{
	Vector2i window_size;
	int player_start_x, player_start_y;
	int goal_end_x, goal_end_y;
	int nb_solar_systems;
	int player_size;

} Config;

/** @brief: Contain all the SDL struct needed for window creation and display */
typedef struct RenderWindow
{
	SDL_Window *sdl_win;
	SDL_Surface *sdl_surface;
	SDL_Renderer *sdl_renderer;
} RenderWindow;
extern RenderWindow render_window;

/** @brief Container for the application permanent data in it's entirety */
typedef struct App
{
	Config* config;
	Player* player;
	int mouse_x, mouse_y;
	SolarSystem** solar_systems;
} App;
// Global variable, container for App wide data.
extern App app;

#pragma region File_Reading

char* get_config_name(char **argv);

/**
 * \brief Load and read the file from <file_name> file on disk. Abort program on error.
 * \param file_name Name of the file to be loaded, include extension.
 * \return Config struct from the data read in file.
 */
Config load_config(const char* file_name);

/**
 * \brief Read config name and select appropriate function to treat the line data.
 * \param config_name Name of the expected.
 * \param data C string only containing the data to process.
 */
void process_config_line(const char* config_name, const char* data);

/*@brief Get a vector from a char* in config file.
*/
Vector2i read_vector(const char* line);
#pragma endregion

#pragma region Initializer

/**
 * \brief Initialize application, reading config file 
 * \param n_args Number of arguments passed to program
 * \param argv Arguments passed to program.
 */
void init_app(int n_args, char** argv);

/**
 * \brief Initialize config data and set it.
 * \param file_name Name of the file to read the config data from.
 */
void init_config(const char* file_name);

/** @brief Initialize the window and SDL context. */
void init_render_window(RenderWindow* rw, int width, int height, const char *name);
#pragma endregion

/** @brief Loop the player through the window border, so it stays in the window. */
void keep_player_on_screen(void);

/**
 * \brief Compute physic (forces and movement) and apply new position to entities.
 */
void physic_update(void);

void game_loop(void);

#endif //PROJECTC_OLEIL_MODEL_H

