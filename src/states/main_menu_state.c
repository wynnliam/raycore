// Liam Wynn, 7/9/2019, Raycore

#include "./main_menu_state.h"

#include "../input_handler.h"

static int quit = 0;

void state_main_menu_initialize(SDL_Renderer* renderer) {
}

void state_main_menu_enter() {
}

void state_main_menu_leave() {
}

void state_main_menu_process_input() {
	if(key_pressed(SDL_SCANCODE_P)) {
		quit = 1;
	}
}

void state_main_menu_update() {
}

void state_main_menu_draw(SDL_Renderer* renderer) {
	SDL_SetRenderDrawColor(renderer, 125, 78, 145, 255);
	SDL_RenderClear(renderer);

	// ...

	SDL_RenderPresent(renderer);
}

void state_main_menu_clean_up() {
}

int state_main_menu_quit() {
	return quit;
}
