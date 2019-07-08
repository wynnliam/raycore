// Liam Wynn, 5/11/2018, Raycore

#include "loop_handling.h"
#include "./input_handler.h"

#include "./states/state.h"
#include "./states/example_state.h"

#include <stdio.h>

static struct state state_example;


/* GAME LOOP IMPLEMENTATION */
void do_loop(SDL_Renderer* renderer) {
	int keep_running_game_loop;
	unsigned int startTicks, endTicks;
	unsigned int tickDiff;

	while(1) {
		startTicks = SDL_GetTicks();

		// Grab all keyboard and mouse inputs.
		process_input();
		// Things like keyboard input and user movement.
		update(&keep_running_game_loop);
		// Do a ray-casting rendering step.
		render(renderer);

		save_prev_keyboard_state();

		if(!keep_running_game_loop)
			break;

		endTicks = SDL_GetTicks();
		tickDiff = endTicks - startTicks;

		if(tickDiff < IDEAL_FRAMES_PER_SECOND)
			SDL_Delay(IDEAL_FRAMES_PER_SECOND - tickDiff);
	}
}

/*INITIALIZATION PROCEDURES*/

void initialize(SDL_Renderer* renderer) {
	// TODO: Move state creation elsewhere
	state_example.initialize = &state_example_initialize;
	state_example.enter = &state_example_enter;
	state_example.leave = &state_example_leave;
	state_example.process_input = &state_example_process_input;
	state_example.update = &state_example_update;
	state_example.draw = &state_example_draw;
	state_example.clean_up = &state_example_clean_up;
	state_example.quit = &state_example_quit;
	state_example.id = 0;

	(*state_example.initialize)(renderer);
	// Entering and leaving a state only make sense in the context of
	// other states, really. However, this here would be entering the
	// first state of the game.
	(*state_example.enter)();
}

void process_input() {
	// Calling the input handler's function
	handle_inputs();
	(*state_example.process_input)();
}

/*UPDATE PROCEDURES*/
// TODO: Clean up this!
void update(int* keep_going) {
	// Uh oh, something terrible happened.
	if(!keep_going)
		return;

	(*state_example.update)();
	*keep_going = !(*state_example.quit)();

}


/*RENDERING PROCEDURES*/
void render(SDL_Renderer* renderer) {
	(*state_example.draw)(renderer);
}

void clean_up() {
	// Just like entering, leaving only makes sense in the
	// context of other states (leaving state A and entering state B).
	// However, when we clean up, we want to make sure we are not in
	// any particular state.
	(*state_example.leave)();
	(*state_example.clean_up)();
}
