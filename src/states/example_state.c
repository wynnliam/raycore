// Liam Wynn, 7/8/2019, Raycore

#include "./example_state.h"
#include <stdio.h>
#include <stdlib.h>

#include "../rendering/raycaster.h"
#include "../input_handler.h"

#include "../map/map_loading/map_loading.h"
#include "../map/map_loading/parse/parser.h"

// Stores the player 
int player_x, player_y;
// The player rotation.
int player_rot;

// Temporary storage for map.
unsigned int curr_level;
struct mapdef* map;

static int quit = 0;
static int next_state;

void state_example_initialize(SDL_Renderer* renderer) {
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
	map = load_map(do_map_lookup(curr_level), &player_x, &player_y, &player_rot);
	curr_level++;
}

void state_example_enter(const int from_state, void* message) {
	// Setting this to NONE requests the loop handler to NOT
	// transition to another state.
	next_state = STATE_ID_NONE;

	if(!message)
		return;

	// The message is a string in this case.
	if(from_state == STATE_ID_MAIN_MENU)
		printf("%s", (char*)message);

	free(message);
}

void state_example_leave() {
	printf("Example Leave!\n");
}

void state_example_process_input() {
	if(key_pressed_once(SDL_SCANCODE_P)) {
		next_state = STATE_ID_MAIN_MENU;
	}
}

void state_example_update() {
	if(key_pressed_once(SDL_SCANCODE_1)) {
		free_map(&map);
		map = load_map(do_map_lookup(curr_level), &player_x, &player_y, &player_rot);

		curr_level++;
		if(curr_level >= get_num_loaded_maps())
			curr_level = 0;
	}

	if(!map) {
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

	update_things(map, player_rot);
}


void state_example_draw(SDL_Renderer* renderer) {
	SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255);
	// Fills the screen with the current render draw color, which is
	// cornflower blue.
	SDL_RenderClear(renderer);

	if(map)
		cast_rays(renderer, map, player_x, player_y, player_rot);

	// Forces the screen to be updated.
	SDL_RenderPresent(renderer);
}

void state_example_clean_up() {
	free_map(&map);
}

int state_example_quit() {
	return quit;
}

int state_example_next_state() {
	return next_state;
}

void* state_example_get_pass_message() {
	return NULL;
}
