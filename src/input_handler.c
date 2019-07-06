// Liam Wynn, 7/6/2019, Raycore

#include "./input_handler.h"

#include <stdlib.h>
#include <stdio.h>

static const Uint8* curr_keyboard_state = NULL;

void handle_inputs() {
	SDL_PumpEvents();

	curr_keyboard_state = SDL_GetKeyboardState(NULL);
}

int key_pressed(const int key) {
	if(key < SDL_SCANCODE_A || key >= SDL_NUM_SCANCODES || !curr_keyboard_state)
		return 0;

	return curr_keyboard_state[key];
}
