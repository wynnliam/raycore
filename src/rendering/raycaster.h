// Liam Wynn, 5/13/2018, Raycore

/*
	Contains every function, parameter, and so on for performing
	the raycast algorithm.
*/

#include "../map/map.h"

#ifndef RAYCASTER
#define RAYCASTER

// Field of view, in radians. This specifies the angle of what we can
// view. It is also important for computing the rays themselves.
#define FOV					60
// This value occurs in the raycaster enough that we ought to pre-compute
// it.
#define FOV_HALF			30
// The world is essentially a series of cubes. This value is the size of
// each cube or unit as I call it in pixels.
#define UNIT_SIZE			64
// Essentially, UNIT_SIZE = 2 ^ UNIT_POWER. We use this to do effecient
// multiplication and division.
#define UNIT_POWER			6
// Since we use it frequently, we will define half of the unit size and its
// power here
#define HALF_UNIT_SIZE		32
#define HALF_UNIT_POWER		5
// The size of the projection area, which is our screen. I would call it
// "SCREEN_W and SCREEN_H", but the underlying math refers to them as
// the PROJECTION PLANE, so I will use that nomenclature instead.
#define PROJ_W				320
#define PROJ_H				200
// We use half of the projection area frequently, so we will just compute
// those here.
#define HALF_PROJ_W			160
#define HALF_PROJ_H			100
// Distance from the player to the projection plane. This is calculated
// with some basic trigonometry, but we will have it pre-computed here
#define DIST_TO_PROJ		277
// Essentially the FOV / PROJ_W, but we will pre-compute that here.
#define ANGLE_BETWEEN_RAYS	0.1875
// Multiply a radian value to get its approximated degree.
#define RAD_TO_DEG 			57.296
// Experimentation with this value showed twice the screen size
// worked very well.
#define SKYBOX_TEX_WIDTH		640

#define MAX_DIST_SQRD			861184

void initialize_lookup_tables();
void initialize_render_textures(SDL_Renderer* renderer);
int get_tile(int x, int y, struct mapdef* map);
void cast_rays(SDL_Renderer* renderer, struct mapdef* map, int player_x, int player_y, int player_rot);
#endif
