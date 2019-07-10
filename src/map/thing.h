// Liam Wynn, 7/10/2019, Raycore

#ifndef THING
#define THING

#include "../animation/animation.h"
#include <SDL2/SDL.h>

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

struct mapdef;

void update_things(struct mapdef* map, const int player_rot);
/*
	For different classes animations, we update them differently.
	These are example animation classes. If you want your own,
	you'd have to implement them here.
*/
void update_thing_anim(struct thingdef* thing, const int player_rot);
#endif
