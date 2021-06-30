#include "util.h"

#define UNIT_POWER 6

int get_tile(int x, int y, struct mapdef* map) {
	int grid_x = x >> UNIT_POWER;
	int grid_y = y >> UNIT_POWER;

	if(grid_x < 0 || grid_x > map->map_w - 1)
		return -1;
	if(grid_y < 0 || grid_y > map->map_h - 1)
		return -1;

	return map->layout[grid_y * map->map_w + grid_x];
}

void set_vis(int x, int y, struct mapdef* map, char vf) {
	int grid_x = x >> UNIT_POWER;
	int grid_y = y >> UNIT_POWER;

	if(grid_x < 0 || grid_x > map->map_w - 1)
		return;
	if(grid_y < 0 || grid_y > map->map_h - 1)
		return;

  map->vis[grid_y * map->map_w + grid_x] = vf;
}
