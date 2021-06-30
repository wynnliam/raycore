// Liam Wynn, 6/28/2021, Raycore

#include "../map/map.h"

#ifndef RAYCASTER_UTIL
#define RAYCASTER_UTIL

int get_tile(int x, int y, struct mapdef* map);
void set_vis(int x, int y, struct mapdef* map, char vf);
char get_vis(int x, int y, struct mapdef* map);

#endif
