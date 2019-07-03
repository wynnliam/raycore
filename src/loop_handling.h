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

// Handles any non-rendering logic we need
// per frame. Returns 1 if we want to do another
// loop iteration, and 0 if we want to quit.
int update();

// Handles per-frame rendering logic.
void render(SDL_Renderer* renderer);

// The game loop itself
void do_loop(SDL_Renderer* renderer);

// Cleans up map data and whatnot.
void clean_up();
#endif
