#ifndef PROJECTC_OLEIL_VUE_CONTROLLER_H
#define PROJECTC_OLEIL_VUE_CONTROLLER_H

#include "../model/model.h"
// #include <SDL2_gfxPrimitives.h>

extern const struct SDL_Color red;
extern const struct SDL_Color green;
extern const struct SDL_Color blue;
extern const struct SDL_Color cyan;
extern const struct SDL_Color white;
extern const struct SDL_Color grey;
extern const struct SDL_Color black;

typedef struct KeyFlags {
    int left, right, up, down;
    int space;

} KeyFlags;

/**
 * \brief Loop.Check Inputs queue and call appropriate function if any is detected.
 * \return: 0 if user request to exit app, 1 otherwise.
*/
int handle_inputs(void);

/**
 * \brief Handle user key down event.
 * \param: key down event to process.
*/
void keyboard_key_down(const SDL_KeyboardEvent *key_event);

/**
 * \brief Handle user key up event.
 * \param: key up event to process.
*/
void keyboard_key_up(const SDL_KeyboardEvent *key_event);

// @todo documentation
void update_window_name(const int framerate);

#pragma region Drawing_fuction

/**
 * \brief Imported code - Replacement for glx
 * \param renderer 
 * \param centre_x 
 * \param centre_y 
 * \param radius 
 * \return 
 */
int draw_circle(SDL_Renderer *renderer, int32_t centre_x, int32_t centre_y, int32_t radius);

/**
 * \brief Imported code - Replacement for glx
 * \param renderer 
 * \param x 
 * \param y 
 * \param radius 
 * \return 
 */
int render_fill_circle(SDL_Renderer *renderer, int x, int y, int radius);

#pragma endregion

void render_systems(void);

/**
 * \brief Render player as a filled red rectangle using Its SDL_FRect.
 */
void render_player(void);

/**
 * \brief Execute all the rendering. Loop each frame.
 */
void render(void);

#endif //PROJECTC_OLEIL_VUE_CONTROLLER_H
