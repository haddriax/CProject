#ifndef PROJECTC_OLEIL_VUE_CONTROLLER_H
#define PROJECTC_OLEIL_VUE_CONTROLLER_H

#include "../model/model.h"
// #include <SDL2_gfxPrimitives.h>

extern const struct SDL_Color red;
extern const struct SDL_Color green;
extern const struct SDL_Color blue;
extern const struct SDL_Color white;
extern const struct SDL_Color grey;
extern const struct SDL_Color black;

typedef struct KeyFlags
{
	int left, right, up, down;
	int space;

} KeyFlags;

/* Loop.Check Inputs queue and call appropriate function if any is detected.
* @return: 0 if user request to exit app, 1 otherwise.
*/
int handle_inputs(void);

/* Handle user key down event.
* @param: key down event to process.
*/
void keyboard_key_down(const SDL_KeyboardEvent* key_event);

/* Handle user key up event.
* @param: key up event to process.
*/
void keyboard_key_up(const SDL_KeyboardEvent* key_event);

// @todo
void update_window_name(const RenderWindow *rw, char *name, float framerate);

#pragma region Drawing_fuction
/**
 * \brief Draw a rectangle from integer coordinates, width and height.
 * \param coord_x x coordinate of the top left corner.
 * \param coord_y y coordinate of the top left corner.
 * \param width Width of the rectangle
 * \param height Height of the rectangle
 * \param color Filling color
 */
void draw_rectangle(int coord_x, int coord_y, int width, int height, const SDL_Color* color);
/**
 * \brief Draw a line from 4 integer coordinates
 * \param x_begin x coordinate of begin point
 * \param y_begin y coordinate of begin point
 * \param x_end x coordinate of end point
 * \param y_end y coordinate of end point
 * \param color Line color
 * \return O on success, or a negative code on failure.
 */
int draw_line(int x_begin, int y_begin, int x_end, int y_end, const SDL_Color* color);
/**
 * \brief Draw a line from 2 Vector2i
 * \param begin Coordinates of Begin point
 * \param end Coordinates of End point
 * \param color Line color
 * \return O on success, or a negative code on failure.
 */
int draw_line_p(const SDL_FPoint* begin, const SDL_FPoint* end, const SDL_Color* color);
/**
 * \brief Draw a red line from vector parameters
 * \param begin Begin point
 * \param end End point
 * \return O on success, or a negative code on failure.
 */
int draw_line_debug(const Vector2i* begin, const Vector2i* end);

/**
 * \brief Imported code - Replacement for glx
 * \param renderer 
 * \param centre_x 
 * \param centre_y 
 * \param radius 
 * \return 
 */
int draw_circle(SDL_Renderer* renderer, int32_t centre_x, int32_t centre_y, int32_t radius);

/**
 * \brief Imported code - Replacement for glx
 * \param renderer 
 * \param x 
 * \param y 
 * \param radius 
 * \return 
 */
int render_fill_circle(SDL_Renderer* renderer, int x, int y, int radius);
#pragma endregion

void render_systems(void);

/**
 * \brief Render player as a filled red rectangle using Its SDL_FRect.
 */
void render_player(void);

// Execute all the rendering. Loop each frame.
void render(void);

#endif //PROJECTC_OLEIL_VUE_CONTROLLER_H
