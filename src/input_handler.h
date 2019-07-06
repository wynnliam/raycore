// Liam Wynn, 7/6/2019, Raycore

#ifndef INPUT_HANDLER
#define INPUT_HANDLER

#include <SDL2/SDL.h>

// TODO: Add key_pressed_once command.

/**
	Simply updates the prev_keyboard_state curr_keyboard_state
	tables.

	PRECONDITIONS:
		SDL must be initialized

	POSTCONDITIONS:
		prev_keyboard_state and curr_keyboard_state values will
		be changed.

	ARGUMENTS:
		None

	RETURNS:
		None
*/
void handle_inputs();

/*
	Determines if a given key has been pressed. Credit to this forum for
	how to do this: https://forums.libsdl.org/viewtopic.php?t=11858

	Note that: SDL_SCANCODE_A = 4 and SDL_NUM_SCANCODES = 512

	PRECONDITIONS:
		SDL Must be initialized

	POSTCONDITIONS:
		None

	ARGUMENTS:
		key. Must be between SDL_SCANCODE_A and SDL_NUM_SCANCODES

	RETURNS:
		1 - key was pressed
		0 - key was not pressed OR key was invalid OR curr_keyboard_state is NULL.
*/
int key_pressed(const int key);

int key_pressed_once(const int key);

void save_prev_keyboard_state();


#endif

