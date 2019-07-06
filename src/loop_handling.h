// Liam Wynn, 5/11/2018, Raycore

/*
	These functions are the meat and potatos of the system.
	It is where we handle the main rendering steps (initialize,
	update, and render). Pretty much all of the action is handled
	here.
*/

#ifndef LOOP_HANDLER
#define LOOP_HANDLER
#include <SDL2/SDL.h>

#define IDEAL_FRAMES_PER_SECOND	30

// Handles any pre-loop logic we need.
void initialize(SDL_Renderer* renderer);

/*
	Performs a single step of non-rendering game logic.

	PRECONDITIONS:
		initialize needs to be called.

	POSTCONDITIONS:
		Several game-specific objects will likely be modified.

	ARGUMENTS:
		keep_running: A flag that we pass in to tell us if
		we should keep running the game. If 0, it will end the game.
		Otherwise, we keep going.
*/
void update(int* keep_running);

// Handles per-frame rendering logic.
void render(SDL_Renderer* renderer);

// The game loop itself
void do_loop(SDL_Renderer* renderer);

// Cleans up map data and whatnot.
void clean_up();
#endif
