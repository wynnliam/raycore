// Liam Wynn, 5/12/2018, Raycore

/*
	Defines the properties of a basic map. For now, this data
	will be constant, and we will just render walls with a single
	color.
*/

#ifndef MAP
#define MAP

#include <SDL2/SDL.h>

#include "./thing.h"
#include "./entity/entity.h"

#define MAP_W	20
#define MAP_H	10

#define WALL_DEF_COUNT		200
#define FLOOR_CEIL_COUNT	200
#define THING_COUNT			1000
#define ENTITY_COUNT		1000

// Specifies the data for a given wall tile.
struct walldef {
	// To do a quick lookup of the path for a given texture.
	char* path;
	// Defines the texture for this wall.
	SDL_Surface* surf;
};

// Specifies floor and ceiling textures for a given
// point. If a texture is NULL, we do not render it.
struct floorcielingdef {
	// To do a quick lookup of the path for a given texture.
	char* floor_path;
	char* ceil_path;
	SDL_Surface* floor_surf;
	SDL_Surface* ceil_surf;

	// If not 0, then we treat this as having an invisible wall.
	int invisible_wall;
};

struct mapdef {
	// Specifies the layout of the world.
	unsigned int* layout;
	// Specifies invisible walls in level. If a value is not 0,
	// it is an invisible wall.
	int* invisible_walls;

	// Assume an upper bound of 100 wall textures.
	struct walldef walls[WALL_DEF_COUNT];
	// Assume also an upper bound of 100 floor and ceiling textures.
	struct floorcielingdef floor_ceils[FLOOR_CEIL_COUNT];

	// Stores the sky texture used for the map.
	SDL_Surface* sky_surf;

	// The list of sprites in the world. We sort
	// this every frame by distance from the player.
	// We will assume there can be at most 1000 sprites
	// in a level.
	struct thingdef things[THING_COUNT];

	struct entity* entities[ENTITY_COUNT];

	// Specifies the dimensions of the world.
	unsigned int map_w, map_h;

	// Stores the number of tile types (num_wall_tex + num_foor_ceils)
	unsigned int num_tiles;
	// Stores the number of wall textures used by the system.
	unsigned int num_wall_tex;
	// Stores the number of floor ceiling pairs used in the map.
	unsigned int num_floor_ceils;

	// Stores the number of things in the world.
	unsigned int num_things;
	unsigned int num_entities;

	// If 0 -- Do not use fog. If not 0 -- Use fog.
	int use_fog;
	int fog_r, fog_b, fog_g;

	// TODO: Have gamemode struct where this gets handled.
	// if -1: no transition. Otherwise, transition to corresponding level.
	int signal_level_transition;
	// Id of the spawn to go to.
	int signal_next_spawn;
};

// Null initializes everything in the map.
int initialize_map(struct mapdef* map);

int insert_entity_into_map(struct mapdef* map, struct entity* entity);
int remove_entity_from_map(struct mapdef* map, const int id);
void update_entities(struct mapdef* map);

/*
	Cleans up all the allocated attributes of a given mapdef.

	PRECONDITIONS:
		to_clean (see ARGUMENTS) is properly allocated and initialized.

	POSTCONDITIONS:
		to_clean's attributes will be properly cleaned up.

	ARGUMENTS:
		to_clean - the mapdef to clean up.

	RETURNS:
		0 - to_clean is NULL.
		1 - to_clean is properly cleaned up.
*/
int clean_mapdef(struct mapdef* to_clean);

struct mapdef* load_map(const char* path, int* player_x, int* player_y, int* player_rot);
void free_map(struct mapdef** map);

int is_position_wall(struct mapdef* map, int player_x, int player_y);

void clear_all_thing_signals(struct mapdef* map);

void spawn_player(struct mapdef* map, int* player_x, int* player_y, int* player_rot, int spawn_id);

#endif
