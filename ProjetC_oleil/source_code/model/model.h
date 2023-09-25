#ifndef PROJECTC_OLEIL_MODEL_H
#define PROJECTC_OLEIL_MODEL_H

#include <SDL.h>
#include <stdio.h>

#pragma region Vector2f

/**
Float Vector of Dimension 2 of coordinates x and y.
*/
typedef struct Vector2f
{
	float x, y;
} Vector2f;
extern const Vector2f vector2f_zero;

/** @brief: Return a new Vector from the addition of the 2 parameters vectors.
@return: New Vector2f from (v1 + v2)
*/
struct Vector2f add_vector(const Vector2f* v1, const Vector2f* v2);

/** @brief: Return a new Vector from the substraction of the 2 parameters vectors.
@return: New Vector2f from (v1 - v2)
*/
struct Vector2f sub_vector(const Vector2f* v1, const Vector2f* v2);

/** @brief: Return the result of the dot product from v1 and v2.
@return: float (v1.v2)
*/
float dot_product(const Vector2f* v1, const Vector2f* v2);

/** @todo !
@brief: Scale v to a magnitude of 1
*/
void vector_normalize(Vector2f* v);

/** @todo !
@brief: Calculate the length of v. Calcul is heavy.
@return: Length of v.
*/
float vector_length(const Vector2f* v);

/** @todo !
@brief: Calculate the length² of v. Use it for comparison over vector_length
@return: Length of v.
*/
float vector_length_squared(const Vector2f* v);

#pragma endregion

typedef struct Clock
{
	Uint64 last_time; 
	Uint64 current_time;
	Uint64 delta_ticks;
};

/* @brief: Container for Player data */
typedef struct Player
{
	SDL_Point location;
	Vector2f thrust;

} Player;

/** @brief: Contain the data from the Config.txt file on disk */
typedef struct Config
{
	int window_width;
	int window_height;
} Config;

/** @brief: Contain all the SDL struct needed for window creation and display */
typedef struct RenderWindow
{
	SDL_Window *sdl_win;
	SDL_Surface *sdl_surface;
	SDL_Renderer *sdl_renderer;
} RenderWindow;

/** @brief Container for the application permanent data in it's entirety */
typedef struct App
{
	RenderWindow* rw;
	Config* config;
	Player* player;
	int mouse_x, mouse_y;
} App;
// Global variable, container for App wide data.
extern struct App app;

/** @brief Load and read the file from <file_name> file on disk 
 log found errors
 @return new struct containing the config for the game
*/
Config load_config(char* file_name);

/** @brief Initialize the window and SDL context. */
void init_RenderWindow(RenderWindow* rw, int w, int h);

void physic_update();

void gameloop();

#endif //PROJECTC_OLEIL_MODEL_H

