// Liam Wynn, 7/7/2019, Raycore

#ifndef STATE
#define STATE

#include <SDL2/SDL.h>

#define STATE_ID_NONE		-1
#define STATE_ID_MAIN_MENU	0
#define STATE_ID_EXAMPLE	1

/*
	At any given moment, a game is in some kind of state.
	Each state is something akin to its own program. Depending
	on the state we are in, we will do input handling, updating,
	rendering, etc differently.

	Each state will have the following functions accompanying it:
		-initialize: Any pre-loop initialization that needs to be done

		-enter: When we enter the state, stuff we need to do.

		-leave: Anything we need to do when we leave the state.

		-process_input: each state will handle input differently, and
		this is where it's done.

		-update: Handles non-rendering frame logic for the current state.

		-draw: Rendering frame logic.

		-clean_up: Similar to leave (potentially), but what is called when
		we quit the game.

		-quit: Returns 1 if we want to quit the game. Returns 0 if not.

		-next_state: Returns -1 if we want to remain in this state,
		otherwise returns an integer specifying the state we want.

		-get_pass_message: Returns some kind of data the state we are
		leaving wants to give the state we are entering. If we do not
		want to pass data, simply return NULL. Note that when the next state
		recieves the message, it becomes the next state's responsibility.
*/

struct state {
	void (*initialize)(SDL_Renderer*);
	void (*enter)(const int from_state, void* message);
	void (*leave)();
	void (*process_input)();
	void (*update)();
	void (*draw)(SDL_Renderer*);
	void (*clean_up)();

	int (*quit)();
	int (*next_state)();
	void* (*get_pass_message)();

	int id;
};

#endif
