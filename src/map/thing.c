#include "./thing.h"

void update_thing_anim(struct thingdef* thing, const int player_rot) {
	// Update animation according to class
	if(thing->anim_class == 0)
		update_anim_class_0(thing, player_rot);
	else if(thing->anim_class == 1)
		update_anim_class_1(thing, player_rot);
	else if(thing->anim_class == 2)
		update_anim_class_2(thing, player_rot);
}

void update_anim_class_0(struct thingdef* thing, const int player_rot) {
	// Nothing to do since this is just a single frame.
}

void update_anim_class_1(struct thingdef* thing, const int player_rot) {
	int orientation;

	orientation = get_thing_orientation(thing->rotation, player_rot);
	thing->curr_anim = 0;
	thing->anims[0].curr_frame = orientation;
	thing->anims[0].start_x = 0;
}

void update_anim_class_2(struct thingdef* thing, const int player_rot) {
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
