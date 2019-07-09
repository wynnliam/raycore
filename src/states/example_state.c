// Liam Wynn, 7/8/2019, Raycore

#include "./example_state.h"
#include <stdio.h>

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

int quit = 0;

void update_thing_type_0(struct mapdef* map, struct thingdef* thing);
void update_thing_type_1(struct mapdef* map, struct thingdef* thing);

void update_anim_class_0(struct thingdef* thing);
void update_anim_class_1(struct thingdef* thing);
void update_anim_class_2(struct thingdef* thing);

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
	map = NULL;
}

void state_example_enter() {
	printf("Example Enter!\n");
}

void state_example_leave() {
	printf("Example Leave!\n");
}

void state_example_process_input() {
	if(key_pressed(SDL_SCANCODE_P)) {
		quit = 1;
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
	return -1;
}
