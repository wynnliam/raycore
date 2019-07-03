// Liam Wynn, 5/12/2018, Raycore

/*
	Defines the properties of a basic map. For now, this data
	will be constant, and we will just render walls with a single
	color.
*/

#ifndef MAP
#define MAP

#include <SDL2/SDL.h>

#include "../animation.h"
#include "./map_loading/parse/parser.h"

#define MAP_W	20
#define MAP_H	10

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

// Defines a "thing" in the world. This is for
// static objects in the world, but can easily
// be extended for dynamic objects and/or NPCs.
struct thingdef {
	// The texture to render.
	SDL_Surface* surf;
	struct animdef anims[100];
	// The global position.
	int position[2];
	// The global rotation in degrees.
	int rotation;
	// The distance from the player.
	int dist;

	// 0 is no animations. 1 is frame per orientation. 2 is
	// animations for each orientation. This is a value that
	// is set/interpreted on a per game basis.
	int anim_class;
	unsigned int num_anims;
	unsigned int curr_anim;

	int type;
};

struct mapdef {
	// Specifies the layout of the world.
	unsigned int* layout;
	// Specifies invisible walls in level. If a value is not 0,
	// it is an invisible wall.
	int* invisible_walls;

	// Assume an upper bound of 100 wall textures.
	struct walldef walls[100];
	// Assume also an upper bound of 100 floor and ceiling textures.
	struct floorcielingdef floor_ceils[100];

	// Stores the sky texture used for the map.
	SDL_Surface* sky_surf;

	// The list of sprites in the world. We sort
	// this every frame by distance from the player.
	// We will assume there can be at most 1000 sprites
	// in a level.
	struct thingdef things[1000];

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

	// If 0 -- Do not use fog. If not 0 -- Use fog.
	int use_fog;
	int fog_r, fog_b, fog_g;
};

/*
	Main driver for constructing a mapdef from a map_data.

	PRECONDITIONS:
	mapdef and map_data are not NULL, and the data inside map_data is valid (files
	can be found, no inappropriate values for members, etc).

	POSTCONDITIONS:
	The player position might be changed.

	ARGUMENTS:
	mapdef - the mapdef structure that will have data added to it.
	map_data - who we transform into mapdef.
	player_x and player_y and player_rot- temporary values so that we can set the player's position and rotation

	RETURNS:
	1 - Successfully built mapdef from map_data.
	0 - Failed to build mapdef from map_data.
*/
int build_mapdef_from_map_data(struct mapdef* mapdef, struct map_data* map_data, int* player_x, int* player_y, int* player_rot);

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

#endif
