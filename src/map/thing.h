// Liam Wynn, 7/10/2019, Raycore

#ifndef THING
#define THING

#include "../animation/animation.h"
#include <SDL2/SDL.h>

#define ANIM_COUNT	100

struct mapdef;

// Defines a "thing" in the world. This is for
// static objects in the world, but can easily
// be extended for dynamic objects and/or NPCs.
struct thingdef {
  // The texture to render
  unsigned int* data;
  // The dimensions of the texture
  unsigned int tw, th;
	struct animdef anims[ANIM_COUNT];
	// The global position.
	int position[2];
	// The global rotation in degrees.
	int rotation;
	// The distance from the player.
	int dist;
    int active;

	// 0 is no animations. 1 is frame per orientation. 2 is
	// animations for each orientation. This is a value that
	// is set/interpreted on a per game basis.
	int anim_class;
	unsigned int num_anims;
	unsigned int curr_anim;

	int type;
	// A way to identify things. Note that id's are NOT required for
	// things. They only make sense for things that are children of
	// entities. Most things only exist as decorations for levels.
	// By default, the id is set to 0, so for unique things, choose
	// ids with a number not 0.
	int id;

	// TODO Union of signal bits!
	int signal_user_interact;
};

int create_thingdef(struct thingdef* empty_thingdef, char* sprite_sheet, int anim_class, int x, int y, int rot);

/*
	Updates the animations of each thing. Depending on the animation
	class it has, we use a different update function.
*/
void update_thing_anims(struct mapdef* map, const int player_rot);

int clear_thingdef(struct thingdef* to_clean);

// Thing signal api

void set_signal_user_interact_on(struct thingdef* thing);
void set_signal_user_interact_off(struct thingdef* thing);
int check_signal_user_interact(struct thingdef* thing);
void clear_all_signals(struct thingdef* thing);
#endif
