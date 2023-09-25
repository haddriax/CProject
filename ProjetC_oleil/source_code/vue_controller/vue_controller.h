#ifndef PROJECTC_OLEIL_VUE_CONTROLLER_H
#define PROJECTC_OLEIL_VUE_CONTROLLER_H

#include "../model/model.h"

extern const struct SDL_Color red;
extern const struct SDL_Color green;
extern const struct SDL_Color blue;

/* Loop.Check Inpus queue and call appropriate function if any is detected.
* @return: 0 if user request to exit app, 1 otherwise.
*/
int handle_inputs();
// @todo
void update_window_name(RenderWindow *rw, char *name, float framerate);

#pragma region Drawing_fuction
void draw_rectangle(RenderWindow* rw, int coord_x, int coord_y, int width, int height, const SDL_Color* color);
void draw_line(RenderWindow* rw, int x_begin, int y_begin, int x_end, int y_end, const SDL_Color* color);
#pragma endregion


// Execute all the rendering. Loop each frame.
void render(RenderWindow *rw);

#endif //PROJECTC_OLEIL_VUE_CONTROLLER_H
