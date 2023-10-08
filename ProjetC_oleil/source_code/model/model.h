#ifndef PROJECTC_OLEIL_MODEL_H
#define PROJECTC_OLEIL_MODEL_H

#include <SDL.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define CONFIG_DEFAULT_NAME "config.txt"
#define APP_DEFAULT_NAME "Projet C"

#define CONFIG_NAME_WINDOW_SIZE "WIN_SIZE"
#define CONFIG_NAME_PLAYER_LOCATION "START"
#define CONFIG_NAME_GOAL_LOCATION "END"
#define CONFIG_NAME_NUMBER_OF_SYSTEMS "NB_SOLAR_SYSTEM"
#define CONFIG_NAME_STAR_POS "STAR_POS"
#define CONFIG_NAME_STAR_RADIUS "STAR_RADIUS"
#define CONFIG_NAME_NB_PLANET "NB_PLANET"
#define CONFIG_NAME_PLANET_RADIUS "PLANET_RADIUS"
#define CONFIG_NAME_PLANET_ORBIT "PLANET_ORBIT"
#define SEPARATOR_SPACE ' '
#define SEPARATOR_SUBSTRACT '-'

#define COLOR_PARAMS(x) x.r, x.g, x.b, x.a

#define GRAVITY = 9.81f

#define CONFIG_BUFFER_MAX_SIZE 32

#define PLAYER_SIZE 10

/**
 * \brief Identifier for any line in config.txt
 */
typedef enum config_type
{
	window_size = 1,
	player_location,
	goal_location,
	nb_solar_system,
	star_pos,
	star_radius,
	nb_planet,
	planet_radius,
	planet_orbit,
	none = 0
} config_type;

enum
{
	max_config_lines = 64,
	max_config_large_buffer_size = 256
};

#define FILE_MODE_READONLY "r"

#pragma region Vector2f

/**
 * \brief Integer Vector of Dimension 2; Coordinates x and y.
 */
typedef struct Vector2f
{
	float x, y;
} Vector2f;
extern const Vector2f vector2f_zero;

/**
 * \brief Integer Vector of Dimension 2; Coordinates x and y.
 */
typedef struct Vector2i
{
	int x, y;
} Vector2i;
extern const Vector2i vector2i_zero;

/**
 * \brief Return a new Vector from the addition of the 2 parameters vectors.
 * \param v1 
 * \param v2 
 * \return New Vector2f from (v1 + v2)
 */
Vector2i vector_add(const Vector2i* v1, const Vector2i* v2);

/**
 * \brief Return a new Vector from the addition of the 2 parameters vectors.
 * \param v1
 * \param v2
 * \return New Vector2f from (v1 + v2)
 */
Vector2f vectorf_add(const Vector2f* v1, const Vector2f* v2);

/**
 * \brief Return a new Vector from the addition of the 2 parameters vectors (by copy).
 * \param v1
 * \param v2
 * \return New Vector2f from (v1 + v2)
 */
Vector2f vectorf_add_cp(const Vector2f v1, const Vector2f v2);

/**
* \brief Return a new Vector from the substraction of the 2 parameters vectors.
* \return New Vector2f from (v1 - v2)
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

/**
* @todo !
* @brief: Calculate the (length*length) of v. Use it for comparison over vector_length
* @return: Length of v.
*/
float vector_length_squared(const Vector2i* v);

#pragma endregion

typedef struct Clock
{
	Uint64 last_time; 
	Uint64 current_time;
	Uint64 delta_ticks;
} Clock;

/**
 * \brief Container for Player data
 */
typedef struct Player
{
	/**
	 * \brief Player location (center of the rectangle)
	 */
	SDL_FPoint location;

	/**
	 * \brief Rectangle for rendering and collision.
	 */
	SDL_FRect draw_rect;

	Vector2f velocity;

	float mass;

} Player;

typedef struct Planet
{
	SDL_FPoint location;
	int radius;
	int orbit;
    struct SolarSystem* parent_system;
} Planet;

typedef struct SolarSystem
{
	SDL_Point location;
	int radius;
	int nb_planets;
	float mass;
	Planet* planets;

} SolarSystem;

typedef struct Entities
{
	Player* player;
	SDL_Rect* end;
	SolarSystem** solar_systems;
	int nb_solar_systems;
	
} Entities;

/** @brief: Contain the data from the Config.txt file on disk */
typedef struct Config
{
	Vector2i window_size;
	SDL_FPoint player_start;
	SDL_FPoint goal_end;
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
	Entities* entities;
	int mouse_x, mouse_y;
	int simulation_started;
} App;
// Global variable, container for App wide data.
extern App app;

#pragma region File_Reading

/**
 * \brief Find the name of the config file passed as a command argument.
 * \param argv Collection of command arguments passed to the program
 * \return Name of config file
 */
char* get_config_file_name(char **argv);

/**
 * \brief Load and read the file from <file_name> file on disk. Abort program on error.
 * \param file_name Name of the file to be loaded, include extension.
 * \return Config struct from the data read in file.
 */
Config load_config(const char* file_name);

/**
 * \brief Find the name of a config from a line.
 * \param line line to process
 * \param out_config_name config name of this line, as an out parameter.
 * \return length of the config name
 */
int find_config_line_name(const char* line, char* out_config_name);

/**
 * \brief Given a config_name, check if it match a config_type
 * \param config_name Name of the config
 * \return config_type if matching, config_type::none if there is no match
 */
config_type find_config_type(const char* config_name);

/**
 * \brief Get the data substring from line as a copy
 * \param line Line to get the data from
 * \param data_start Index where to start the substring
 * \return alloc of char* containing data. Must be freed once used.
 */
char* get_data_from_line(const char* line, const int data_start);

/**
 * \brief Ensure the data char* contain only alphanumerical char and no spaces.
 * \param data data to validate.
 * \return 1 Success, 0 otherwise.
 */
int validate_is_int(const char* data);

/**
 * \brief Ensure the data char* contain only alphanumerical char, no spaces but eventual char 0 to be '-' .
 * \param data data to validate.
 * \return 1 Success, 0 otherwise.
 */
int validate_is_signed_int(const char* data);

/**
 * \brief Ensure the data char* contain only numerical char and exactly 1 space.
 * \param data data to validate.
 * \return 1 Success, 0 otherwise.
 */
int validate_is_vector(const char* data);

/**
 * \brief Verify that the data seems to match the expected config_type
 * \param data data to check
 * \param t config_type to expect
 * \return 1 for success, 0 if data validation failed.
 */
int validate_config_line(const char* data, const config_type t);

/**
 * \brief Read data and select appropriate function to process it
 * \param data data as char*
 * \param type config_type to select data process
 * \param file File where config are read from. Used multiple line reading.
 * \param line_index Incremented index when reading line from file in argument.
 */
void process_data(const char* data, const config_type type, FILE* file, int* line_index);

/**
 * \brief Read a Vector2i from a char*. X and Y values must be separated by a space.
 * \param data Input data
 * \return Resulting Vector2i
 */
Vector2i read_vector(const char* data);

/**
 * \brief Read a SDL_FPoint from a char*. X and Y values must be INTEGERS and separated by a space.
 * \note Config can only store Int, so this function expects to find Integers and cast them to floats.
 * \param data Input data
 * \return Resulting SDL_FPoint
 */
SDL_FPoint read_float_point(const char* data);

/**
 * \brief Read an Integer from a char*.
 * \param data Input data
 * \return Resulting int
 */
int read_int(const char* data);

/**
 * \brief Read a signed Integer from a char*.
 * \param data Input data
 * \return Resulting int
 */
int read_signed_int(const char* data);
#pragma endregion

#pragma region Initializer

/**
 * \brief Alloc and generate a solar system, expect file current line to be STAR_POS.
 * ` Auto insert newly created SolarSystem into entities list.
 * \param file config file
 * \param line_index update line index
 * \param spawn_location
 * \return Pointer to newly created SolarSystem
 */
SolarSystem* build_system(FILE* file, int* line_index, Vector2i spawn_location);

Planet *build_planets(SolarSystem *parent_system, int orbit, int radius, int index);

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
void init_render_window(int width, int height, const char *name);
#pragma endregion

/** @brief Loop the player through the window border, so it stays in the window. */
void keep_player_on_screen(void);

/**
 * \brief Read the Player coordinates and update Its rectangle coordinates (add offset to center location with rect).
 */
void player_update(void);

void apply_player_velocity(void);

/**
 * \brief Compute physic (forces and movement) and apply new position to entities.
 */
void physic_update(void);

void game_loop(void);

#endif //PROJECTC_OLEIL_MODEL_H

