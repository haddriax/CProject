// ReSharper disable CppClangTidyModernizeMacroToEnum
#ifndef PROJECTC_OLEIL_MODEL_H
#define PROJECTC_OLEIL_MODEL_H

#ifdef __unix__
#define OS_Windows 0
#include <unistd.h>
#elif defined(_WIN32) || defined(WIN32)
#define OS_Windows 1

#include <windows.h>

#endif

#include <SDL.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define PRINT_CONFIG_CREATION 0

#define APP_DEFAULT_NAME "Projet C"

#pragma region Configs_Names
#define CONFIG_DEFAULT_NAME "config.txt"
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
#pragma endregion // Configs_Names

#define COLOR_PARAMS(x) (x).r, (x).g, (x).b, (x).a

#define INITIAL_SPEED 4.f
#define MAX_SPEED_VALUE 8.0f
#define MIN_SPEED_VALUE 2.0f

#define GRAVITY_CONST 1000.0f
#define GRAV_DISTANCE_WEIGHTING 1.f
#define GRAV_GENERAL_WEIGHTING 0.4f
#define THRUST_SPEED_WEIGHTING 0.6f

#define CONFIG_BUFFER_MAX_SIZE 32
#define PLAYER_SIZE 10

/**
 * \brief Identifier for any line in config.txt
 */
typedef enum config_type {
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

typedef enum quit_code {
    Error = -1, Exit = 1, PlayerDied = 2, Victory = 3
} quit_code;

enum {
    max_config_lines = 64, max_config_large_buffer_size = 256
};

#define FILE_MODE_READONLY "r"

typedef struct KeyFlags {
    int left, right, up, down;
    int space;

} KeyFlags;
extern KeyFlags key_flags;

#pragma region Vector2f

/**
 * \brief Integer Vector of Dimension 2; Coordinates x and y.
 */
typedef struct Vector2f {
    float x, y;
} Vector2f;
extern const Vector2f vector2f_zero;

#pragma endregion

#pragma region Typedef

/**
 * \brief Container for Player data
 */
typedef struct Player {
    /** \brief Player location (center of the rectangle) */
    SDL_FPoint location;
    /** \brief Rectangle for rendering and collision. */
    SDL_FRect draw_rect;
    /** \brief Movement applied each frame. */
    Vector2f velocity;
    /** \brief Force resulting from the player input. */
    Vector2f thrust;
    float mass;
    float speed;
    Vector2f direction;

} Player;

typedef struct Planet {
    SDL_FPoint location;
    float radius;
    float orbit;
    struct SolarSystem *parent_system;
} Planet;

typedef struct SolarSystem {
    SDL_FPoint location;
    float radius;
    int nb_planets;
    float mass;
    Planet *planets;

} SolarSystem;

typedef struct Entities {
    Player *player;
    SDL_FRect *end;
    SolarSystem **solar_systems;
    int nb_solar_systems;

} Entities;

/** @brief: Contain the data from the Config.txt file on disk */
typedef struct Config {
    Vector2f window_size;
    SDL_FPoint player_start;
    SDL_FPoint goal_end;
    int nb_solar_systems;
    int player_size;

} Config;

/** @brief: Contain all the SDL struct needed for window creation and display */
typedef struct RenderWindow {
    SDL_Window *sdl_win;
    SDL_Surface *sdl_surface;
    SDL_Renderer *sdl_renderer;
} RenderWindow;
extern RenderWindow render_window;

/** @brief Container for the application permanent data in it's entirety */
typedef struct App {
    Config *config;
    Entities *entities;
    int mouse_x, mouse_y;
    int simulation_started;
    int score;
    int delta_time;

    Vector2f *list_forces;
    int nb_forces;
} App;
// Global variable, container for App wide data.
extern App app;
#pragma endregion

#pragma region Collisions

int is_colliding_rect_circle(const SDL_FRect *rect, const SDL_FPoint *location, float radius);

int is_colliding_FRect_FRect(const SDL_FRect *r1, const SDL_FRect *r2);

#pragma endregion

#pragma region File_Reading

/**
 * \brief Find the name of the config file passed as a command argument.
 * \param argv Collection of command arguments passed to the program
 * \return Name of config file
 */
char *get_config_file_name(char **argv);

/**
 * \brief Load and read the file from <file_name> file on disk. Abort program on error.
 * \param file_name Name of the file to be loaded, include extension.
 * \return Config* struct from the data read in file.
 */
Config *load_config(const char *file_name);

/**
 * \brief Find the name of a config from a line.
 * \param line line to process
 * \param out_config_name config name of this line, as an out parameter.
 * \return length of the config name
 */
int find_config_line_name(const char *line, char *out_config_name);

/**
 * \brief Given a config_name, check if it match a config_type
 * \param config_name Name of the config
 * \return config_type if matching, config_type::none if there is no match
 */
config_type find_config_type(const char *config_name);

/**
 * \brief Get the data substring from line as a copy
 * \param line Line to get the data from
 * \param data_start Index where to start the substring
 * \return alloc of char* containing data. Must be freed once used.
 */
char *get_data_from_line(const char *line, const int data_start);

/**
 * \brief Ensure the data char* contain only alphanumerical char and no spaces.
 * \param data data to validate.
 * \return 1 Success, 0 otherwise.
 */
int validate_is_int(const char *data);

/**
 * \brief Ensure the data char* contain only alphanumerical char, no spaces but eventual char 0 to be '-' .
 * \param data data to validate.
 * \return 1 Success, 0 otherwise.
 */
int validate_is_signed_int(const char *data);

/**
 * \brief Ensure the data char* contain only numerical char and exactly 1 space.
 * \param data data to validate.
 * \return 1 Success, 0 otherwise.
 */
int validate_is_vector(const char *data);

/**
 * \brief Verify that the data seems to match the expected config_type
 * \param data data to check
 * \param t config_type to expect
 * \return 1 for success, 0 if data validation failed.
 */
int validate_config_line(const char *data, config_type t);

/**
 * \brief Read data and select appropriate function to process it
 * \param data data as char*
 * \param type config_type to select data process
 * \param file File where config are read from. Used multiple line reading.
 * \param line_index Incremented index when reading line from file in argument.
 */
void process_data(const char *data, config_type type, FILE *file, int *line_index);

/**
 * \brief Read a Vector2f from a char*. X and Y values must be separated by a space.
 * \param data Input data
 * \return Resulting Vector2f
 */
Vector2f read_vector(const char *data);

/**
 * \brief Read a SDL_FPoint from a char*. X and Y values must be INTEGERS and separated by a space.
 * \note Config can only store Int, so this function expects to find Integers and cast them to floats.
 * \param data Input data
 * \return Resulting SDL_FPoint
 */
SDL_FPoint read_float_point(const char *data);

/**
 * \brief Read an Integer from a char*.
 * \param data Input data
 * \return Resulting int
 */
int read_int(const char *data);

/**
 * \brief Read a signed Integer from a char*.
 * \param data Input data
 * \return Resulting int
 */
int read_signed_int(const char *data);

#pragma endregion

#pragma region Initializers&Builders

/**
 * \brief Alloc and generate a new Solar System, expect file current line to be STAR_POS.
 * ` Auto insert newly created SolarSystem into entities list.
 * \param file config file
 * \param line_index update line index
 * \param spawn_location
 * \return Pointer to newly created SolarSystem
 */
SolarSystem *build_system(FILE *file, int *line_index, Vector2f spawn_location);

/**
 * \brief Generate a new Planet, located on its orbit at t=0
 * \param parent_system system "containing" the planet (also center of the orbit)
 * \param orbit planet orbit
 * \param radius planet orbit
 * \param index creation index (id in entities list)
 * \return Pointer to newly created Planet
 */
Planet *build_planet(SolarSystem *parent_system, float orbit, float radius, int index);

/**
 * \brief Initialize application, reading config file 
 * \param n_args Number of arguments passed to program
 * \param argv Arguments passed to program.
 */
void init_app(int n_args, char **argv);

/**
 * \brief Initialize config data and set it.
 * \param file_name Name of the file to read the config data from.
 */
void init_config(const char *file_name);

/** @brief Initialize the window and SDL context. */
void init_render_window(int width, int height, const char *name);

#pragma endregion

/** @brief Loop the player through the window border, so it stays in the window. */
void keep_player_on_screen(void);

/**
 * \brief Read the Player coordinates and update Its rectangle coordinates (add offset to center location with rect).
 */
void player_update(void);

/**
 * \brief Reading the Input flags, accelerate the player.
 */
void handle_move_input(void);

void clamp_vector(Vector2f *v, float min, float max);

/**
 * \brief Update the planets location.
 */
void planet_revolution_update(void);

/**
 * \brief Check if any planet or system collides with the player
 * \return 1 if there is a collision, 0 otherwise.
 */
int check_player_planets_collisions(void);

/**
 * \brief If end collides with the player, exit the game and log Victory
 */
void handle_player_end_collision(void);

/**
 * \brief If a Planet collides with the player, exit the game and log Game Over
 */
void handle_player_planets_collisions(void);

/** @todo Write documentation
 * \brief 
 */
void start_simulation(void);

/**
 * \brief Compute physic (forces and movement) and apply new position to entities each frames.
 */
void physic_update(void);

/** @todo Write documentation.
 * \brief 
 * \param v 
 * \param divisor 
 * \return 
 */
Vector2f vector_divi(const Vector2f *v, float divisor);

/** @todo Write documentation.
 * \brief 
 * \param v1 
 * \param v2 
 * \return 
 */
float dot_product(const Vector2f *v1, const Vector2f *v2);

/** @todo Write documentation.
 * \brief 
 * \param v 
 * \param divisor 
 * \return 
 */
Vector2f vector_divi(const Vector2f *v, float divisor);

/**
* \brief Normalize of vector passed as parameter
* \param v vector to normalize
* \return 0 if OK, -1 if magnitude was too small
*/
int vector_normalize(Vector2f *v);

/**
* \brief Return a new Vector from the difference of the 2 parameters vectors
* \return New Vector2f from (v1 - v2)
*/
Vector2f vector_sub(const Vector2f *v1, const Vector2f *v2);

/**
 * \brief Return a new Vector from the addition of the 2 parameters vectors
 * \param v1
 * \param v2
 * \return New Vector2f from (v1 + v2)
 */
Vector2f vectorf_add(const Vector2f *v1, const Vector2f *v2);

Vector2f direction_from_FPoint(const SDL_FPoint *p1, const SDL_FPoint *p2);

/**
 *  \brief Returns the distance btw a star and the ship
 */
float calculate_distance(const SolarSystem *s);

/**
 * \brief Sum of a list of Vector2f of size length
 * \param vector_list list of vector to sum
 * \param length length of the list
 */
Vector2f sum_forces(const Vector2f *vector_list, int length);

/**
 * \brief Returns the vector of attraction between a star and the ship
 */
Vector2f grav_force(const SolarSystem *s);

/**
 * \brief Compute all attraction forces on the player, and update his velocity accordingly
 */
void apply_forces(void);

/**
 * \brief Loop running each frame
 * \param delta_time time elapsed in ms since last update
 */
void game_loop(float delta_time);

/**
 * \brief Quit the app and clear memory. Log error in needed
 * \param code [1] Normal, [-1] Error. Define message output stream
 * \param message message to log into. Keep to NULL if no logs are needed
 */
void quit(quit_code code, const char *message);

void on_left_arrow(void);

void on_right_arrow(void);

#endif //PROJECTC_OLEIL_MODEL_H