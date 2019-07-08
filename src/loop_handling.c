// Liam Wynn, 5/11/2018, Raycore

#include "loop_handling.h"
#include "./input_handler.h"
#include "./rendering/raycaster.h"

#include "./states/state.h"
#include "./states/example_state.h"

#include "./map/map_loading/map_loading.h"
#include "./map/map_loading/parse/parser.h"

#include <stdio.h>

static struct state state_example;

// Stores the player 
int player_x, player_y;
// The player rotation.
int player_rot;

// Temporary storage for map.
unsigned int curr_level;
struct mapdef* map;

void update_thing_type_0(struct mapdef* map, struct thingdef* thing);
void update_thing_type_1(struct mapdef* map, struct thingdef* thing);

void update_anim_class_0(struct thingdef* thing);
void update_anim_class_1(struct thingdef* thing);
void update_anim_class_2(struct thingdef* thing);

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

	player_x = 256;
	player_y = 256;
	player_rot = 0;

	// Raycaster Initialization

	// Initializes all the angle lookup tables.
	initialize_lookup_tables();
	// Intializes the rendering textures.
	initialize_render_textures(renderer);
	// Enables transparent pixel 
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	// Map Loading

	initialize_map_lookup_table();
	curr_level = 0;
	map = NULL;

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
	*keep_going = (*state_example.quit)();

	int result = 1;

	if(key_pressed_once(SDL_SCANCODE_1)) {
		free_map(&map);
		map = load_map(do_map_lookup(curr_level), &player_x, &player_y, &player_rot);

		curr_level++;
		if(curr_level >= get_num_loaded_maps())
			curr_level = 0;
	}

	if(key_pressed(SDL_SCANCODE_P)) {
		result = 0;
	}

	if(!map) {
		*keep_going = result;
		return;
	}

	if(key_pressed(SDL_SCANCODE_A)) {
		player_rot += 2;

		if(player_rot < 0)
			player_rot += 360;
		if(player_rot > 360)
			player_rot -= 360;
	}

	if(key_pressed(SDL_SCANCODE_D)) {
		player_rot -= 2;

		if(player_rot < 0)
			player_rot += 360;
		if(player_rot > 360)
			player_rot -= 360;
	}

	if(key_pressed(SDL_SCANCODE_W)) {
		player_y -= (sin128table[player_rot] << 4) >> 7;
		player_x += (cos128table[player_rot] << 4) >> 7;

		if(get_tile(player_x, player_y, map) < 0 || is_position_wall(map, player_x, player_y)) {
			player_y += (sin128table[player_rot] << 4) >> 7;
			player_x -= (cos128table[player_rot] << 4) >> 7;
		}
	}

	if(key_pressed(SDL_SCANCODE_S)) {
		player_y += (sin128table[player_rot] << 4) >> 7;
		player_x -= (cos128table[player_rot] << 4) >> 7;

		if(get_tile(player_x, player_y, map) < 0 || is_position_wall(map, player_x, player_y)) {
			player_y -= (sin128table[player_rot] << 4) >> 7;
			player_x += (cos128table[player_rot] << 4) >> 7;
		}
	}

	if(key_pressed(SDL_SCANCODE_C)) {
		printf("Player position = [%d, %d]. Player rotation = %d\n", player_x, player_y, player_rot);
	}

	int i;
	for(i = 0; i < map->num_things; ++i) {
		// Update according to type.
		if(map->things[i].type == 0)
			update_thing_type_0(map, &map->things[i]);
		else if(map->things[i].type == 1)
			update_thing_type_1(map, &map->things[i]);

		// Update animation according to class
		if(map->things[i].anim_class == 0)
			update_anim_class_0(&map->things[i]);
		else if(map->things[i].anim_class == 1)
			update_anim_class_1(&map->things[i]);
		else if(map->things[i].anim_class == 2)
			update_anim_class_2(&map->things[i]);
	}

	*keep_going = result;
}

void update_thing_type_0(struct mapdef* map, struct thingdef* thing) {
	// Nothing to do since this is just a player spawn.
}

void update_thing_type_1(struct mapdef* map, struct thingdef* thing) {
	// Nothing to do since this is just a static prop.
}

void update_anim_class_0(struct thingdef* thing) {
	// Nothing to do since this is just a single frame.
}

void update_anim_class_1(struct thingdef* thing) {
	int orientation;

	orientation = get_thing_orientation(thing->rotation, player_rot);
	thing->curr_anim = 0;
	thing->anims[0].curr_frame = orientation;
	thing->anims[0].start_x = 0;
}

void update_anim_class_2(struct thingdef* thing) {
	int orientation;
	int anim;

	// Get the orientation of the thing. Add one to it to get the walking animation
	// for that orientation.
	orientation = get_thing_orientation(thing->rotation, player_rot);
	anim = (orientation << 1) + 1;

	// If the animation the thing should have is not the same as its current,
	// stop the current animation, then set the current animation to the correct one.
	if(anim != thing->curr_anim) {
		stop_anim(&thing->anims[thing->curr_anim]);
		thing->curr_anim = anim;
		start_anim(&thing->anims[thing->curr_anim]);
	}

	update_anim(&thing->anims[thing->curr_anim]);
}

/*RENDERING PROCEDURES*/
void render(SDL_Renderer* renderer) {
	(*state_example.draw)(renderer);
	SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255);
	// Fills the screen with the current render draw color, which is
	// cornflower blue.
	SDL_RenderClear(renderer);

	if(map)
		cast_rays(renderer, map, player_x, player_y, player_rot);

	// Forces the screen to be updated.
	SDL_RenderPresent(renderer);
}

void clean_up() {
	// Just like entering, leaving only makes sense in the
	// context of other states (leaving state A and entering state B).
	// However, when we clean up, we want to make sure we are not in
	// any particular state.
	(*state_example.leave)();
	(*state_example.clean_up)();

	printf("Cleaning up raycaster!\n");

	free_map(&map);

}
