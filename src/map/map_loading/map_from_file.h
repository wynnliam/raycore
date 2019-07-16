// Liam Wynn, 7/15/2019, Raycore

/*
	This entire file is a disaster.

	I assure you, this will be cleaned out when level loading is retooled.

	I wrote it hastly at a time when raycore was to be a single game and then
	lost for eternity.

	This used to be in map.h and map.c.
*/

#ifndef MAP_FROM_FILE
#define MAP_FROM_FILE

struct mapdef;
struct map_data;

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

#endif
