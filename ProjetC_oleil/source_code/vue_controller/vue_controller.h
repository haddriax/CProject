#ifndef PROJECTC_OLEIL_VUE_CONTROLLER_H
#define PROJECTC_OLEIL_VUE_CONTROLLER_H

#include "../model/model.h"
#include <SDL2_gfxPrimitives.h>

extern const struct SDL_Color red;
extern const struct SDL_Color green;
extern const struct SDL_Color blue;

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
void draw_rectangle(const RenderWindow* rw, int coord_x, int coord_y, int width, int height, const SDL_Color* color);
int draw_line(int x_begin, int y_begin, int x_end, int y_end, const SDL_Color* color);
int draw_line_v(const Vector2i* begin, const Vector2i* end, const SDL_Color* color);
int draw_line_debug(const Vector2i* begin, const Vector2i* end);
#pragma endregion

void render_planets(RenderWindow* rw);
void render_player(const RenderWindow* rw);

// Execute all the rendering. Loop each frame.
void render(RenderWindow *rw);

#endif //PROJECTC_OLEIL_VUE_CONTROLLER_H
