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
#include <SDL2/SDL_net.h>

#define IDEAL_FRAMES_PER_SECOND	30

// Handles any pre-loop logic we need.
void initialize(SDL_Renderer* renderer);

/*
	For the current frame, checks and handles any input
	changes.

	PRECONDITIONS:
		initialize is called.

	POSTCONDITIONS:
		-Input handler will have new inputs registered.
		-Current game state will be modified.

	ARGUMENTS:
		-None

	RETURNS:
		-None
*/
void process_input();

/*
	Performs a single step of non-rendering game logic.

	PRECONDITIONS:
		initialize needs to be called.

	POSTCONDITIONS:
		-Current game state will be modified.

	ARGUMENTS:
		keep_running: A flag that we pass in to tell us if
		we should keep running the game. If 0, it will end the game.
		Otherwise, we keep going.
*/
void update(int* keep_running);

/*
	For the current frame, performs a single step of rendering
	logic.

	PRECONDITIONS:
		-initialize is called

	POSTCONDITIONS:
		-Current state rendering logic will be applied to screen.

	ARGUMENTS:
		-renderer: Need this to render to screen.
*/
void render(SDL_Renderer* renderer);

/*
	Leaves the current game state, and cleans it up (as well as
	all other states).

	PRECONDITIONS:
		-initialize is called. It would not make sense to clean up
		something if it doesn't exist.

	POSTCONDITIONS:
		-No longer in any state.
		-All states cleaned.
		-All game memory cleaned up

	ARGUMENTS:
		-None

	RETURNS:
		-None
*/
void clean_up();

// The game loop itself
void do_loop(SDL_Renderer* renderer);
#endif
