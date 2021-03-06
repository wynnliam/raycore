// Liam Wynn, 7/9/2019, Raycore

#ifndef STATE_MAIN_MENU
#define STATE_MAIN_MENU

#include <SDL2/SDL.h>
#include "./state.h"

void state_main_menu_initialize(SDL_Renderer* renderer);
void state_main_menu_enter(const int from_state, void* message);
void state_main_menu_leave();
void state_main_menu_process_input();
void state_main_menu_update();
void state_main_menu_draw(SDL_Renderer* renderer);
void state_main_menu_clean_up();

int state_main_menu_quit();
int state_main_menu_next_state();
void* state_main_menu_get_pass_message();

#endif

