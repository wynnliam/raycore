// Liam Wynn, 7/6/2019, Raycore

#include "./input_handler.h"

#include <stdlib.h>
#include <stdio.h>

static int num_scancodes;
static const Uint8* curr_keyboard_state = NULL;
static Uint8* prev_keyboard_state = NULL;

void handle_inputs() {
	SDL_PumpEvents();

	curr_keyboard_state = SDL_GetKeyboardState(&num_scancodes);
}

int key_pressed(const int key) {
	if(key < SDL_SCANCODE_A || key >= SDL_NUM_SCANCODES || !curr_keyboard_state)
		return 0;

	return curr_keyboard_state[key];
}

int key_pressed_once(const int key) {
	if(key < SDL_SCANCODE_A || key >= SDL_NUM_SCANCODES || !curr_keyboard_state || !prev_keyboard_state)
		return 0;

	return curr_keyboard_state[key] && !prev_keyboard_state[key];
}

void save_prev_keyboard_state() {
	if(prev_keyboard_state)
		free(prev_keyboard_state);

	if(!curr_keyboard_state || num_scancodes < 1) {
		prev_keyboard_state = NULL;
		return;
	}

	prev_keyboard_state = (Uint8*)malloc(sizeof(Uint8) * num_scancodes);

	int i;
	for(i = 0; i < num_scancodes; i++)
		prev_keyboard_state[i] = curr_keyboard_state[i];
}
