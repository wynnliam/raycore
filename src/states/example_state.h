// Liam Wynn, 7/8/2019, Raycore

#ifndef STATE_EXAMPLE
#define STATE_EXAMPLE

#include <SDL2/SDL.h>

void state_example_initialize(SDL_Renderer* renderer);
void state_example_enter();
void state_example_leave();
void state_example_process_input();
void state_example_update();
void state_example_draw(SDL_Renderer* renderer);
void state_example_clean_up();

int state_example_quit();

#endif
