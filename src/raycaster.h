// Liam Wynn, 5/13/2018, CS410p: Full Stack Web Development

/*
	Contains every function, parameter, and so on for performing
	the raycast algorithm.
*/

#include "map.h"

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

struct hitinfo {
	// Where we hit.
	int hit_pos[2];
	// The squared distance.
	int dist;
	// The wall texture.
	int wall_type;
	// If true, the intersection was along a horizontal grid.
	// Otherwise, it was vertical.
	int is_horiz;
	// Used when correcting for 'fisheye' lens.
	int quadrant;
};

// Stores the sin value of every degree from 0 to 360 multiplied by 128.
// This will enable us to preserve enough precision for each number as
// a byte. When we want a value, we can access sin128table[i] >> 7, which
// undoes the multiplication.
int sin128table[361];
// Stores the cos value of every degree from 0 to 360 multiplied by 128.
int cos128table[361];
// Stores every tan value of every degree from 0 to 360 multiplied by 128.
// The values for 0, 90, 180, 270, and 360 will be -1.
int tan128table[361];

// Stores the value of 1/tan(t) * 128 for every degree between 0 and 360.
// The values for 0, 90, 180, 270, and 360 will be -1.
int tan1table[361];
// Stores the value of 1/sin(t) * 128 for every degree between 0 and 360.
int sin1table[361];

// Stores the delta values for horizontal and vertical increments for each
// angle.
int delta_h_x[361];
int delta_h_y[361];
int delta_v_x[361];
int delta_v_y[361];

// Stores the distance each ray is from the player when it hits something.
int z_buffer[PROJ_W];

// What we render for the floor/ceiling
SDL_Texture* floor_ceiling_tex;
// Where we store the floor and ceiling pixels before we render.
unsigned int floor_ceiling_pixels[64000];

// Where we render for skybox and wall pixels.
SDL_Texture* raycast_texture;
// Where we store sky and wall textures before we render.
unsigned int raycast_pixels[64000];

// What we render for the things.
SDL_Texture* thing_texture;
// Where we store thing pixels before we render.
unsigned int thing_pixels[64000];

// Stores the things in a sorted order.
struct thingdef* things_sorted[1000];

void initialize_lookup_tables();
void initialize_render_textures(SDL_Renderer* renderer);
int get_tile(int x, int y, struct mapdef* map);
void cast_rays(SDL_Renderer* renderer, struct mapdef* map, int player_x, int player_y, int player_rot);
#endif
