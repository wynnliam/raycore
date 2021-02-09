// Liam Wynn, 5/11/2018, Raycore

#include "loop_handling.h"
#include "./input_handler.h"

#include "./states/state.h"
#include "./states/example_state.h"
#include "./states/main_menu_state.h"

#include <stdio.h>

static struct state* curr_state;
static struct state  state_example;
static struct state  state_main_menu;

static struct state* get_state(const int id);
static void change_state(const int curr_state_id, const int next_state_id);

/* GAME LOOP IMPLEMENTATION */
void do_loop(SDL_Renderer* renderer) {
	int keep_running_game_loop;
	int next_state_id;
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

		next_state_id = (*(curr_state->next_state))();
		if(next_state_id != STATE_ID_NONE)
			change_state(curr_state->id, next_state_id);


		endTicks = SDL_GetTicks();
		tickDiff = endTicks - startTicks;

		if(tickDiff < IDEAL_FRAMES_PER_SECOND)
			SDL_Delay(IDEAL_FRAMES_PER_SECOND - tickDiff);
	}
}

static struct state* get_state(const int id) {
	if(id == STATE_ID_MAIN_MENU)
		return &state_main_menu;
	else if(id == STATE_ID_EXAMPLE)
		return &state_example;
	else
		return NULL;
}

static void change_state(const int curr_state_id, const int next_state_id) {
	struct state* next_state = get_state(next_state_id);
	void* message;

	if(!next_state)
		return;

	message = (*(curr_state->get_pass_message))();

	(*(curr_state->leave))();
	curr_state = next_state;
	(*(curr_state->enter))(curr_state_id, message);
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
	state_example.next_state = &state_example_next_state;
	state_example.get_pass_message = &state_example_get_pass_message;
	state_example.id = STATE_ID_EXAMPLE;

	state_main_menu.initialize = &state_main_menu_initialize;
	state_main_menu.enter = &state_main_menu_enter;
	state_main_menu.leave = &state_main_menu_leave;
	state_main_menu.process_input = &state_main_menu_process_input;
	state_main_menu.update = &state_main_menu_update;
	state_main_menu.draw = &state_main_menu_draw;
	state_main_menu.clean_up = &state_main_menu_clean_up;
	state_main_menu.quit = &state_main_menu_quit;
	state_main_menu.next_state = &state_main_menu_next_state;
	state_main_menu.get_pass_message = &state_main_menu_get_pass_message;
	state_example.id = STATE_ID_MAIN_MENU;

	(*state_example.initialize)(renderer);
	(*state_main_menu.initialize)(renderer);

	//curr_state = &state_main_menu;
	curr_state = &state_main_menu;

	// Entering and leaving a state only make sense in the context of
	// other states, really. However, this here would be entering the
	// first state of the game.
	(*(curr_state->enter))(STATE_ID_NONE, NULL);
}

void process_input() {
	// Calling the input handler's function
	handle_inputs();
	(*(curr_state->process_input))();
}

/*UPDATE PROCEDURES*/
void update(int* keep_going) {
	// Uh oh, something terrible happened.
	if(!keep_going)
		return;

	(*(curr_state->update))();
	*keep_going = !(*(curr_state->quit))();
}

/*RENDERING PROCEDURES*/
void render(SDL_Renderer* renderer) {
	(*(curr_state->draw))(renderer);
}

void clean_up() {
	// Just like entering, leaving only makes sense in the
	// context of other states (leaving state A and entering state B).
	// However, when we clean up, we want to make sure we are not in
	// any particular state.
	(*(curr_state->leave))();

	(*state_main_menu.clean_up)();
	(*state_example.clean_up)();
}
