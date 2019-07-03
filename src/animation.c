// Liam Wynn, 6/26/2018, CS410p: Full Stack Web Development

#include "animation.h"
#include <SDL2/SDL.h>

void start_anim(struct animdef* anim) {
	if(!anim || anim->bRunning)
		return;

	anim->curr_frame = 0;
	anim->bRunning = 1;
	anim->start_tick = SDL_GetTicks();
}

void update_anim(struct animdef* anim) {
	if(!anim || !anim->bRunning)
		return;

	unsigned int curr_tick = SDL_GetTicks();

	// It's time to update.
	if((curr_tick - anim->start_tick) >= anim->frame_time) {
		// If we are at the last frame, check if we repeat.
		if(anim->curr_frame == anim->num_frames - 1) {
			// If not, we freeze, as the animation is done.
			if(anim->bRepeats)
				anim->curr_frame = 0;
			else
				stop_anim(anim);
		}

		else
			++anim->curr_frame;

		anim->start_tick = curr_tick;
	}
}

void stop_anim(struct animdef* anim) {
	if(!anim || !anim->bRunning)
		return;

	anim->bRunning = 0;
}

/*
	First we subtract the player's rotation from the thing's rotation. Then we add 90 to that.
	That second step isn't neccessary, but it makes the orientation we want match up with a rotation
	angle that makes sense. For example, if they player is looking at the thing, and the thing is
	facing the player, we can say the rotation of the thing and player is opposite, which means there
	is an angle of 180. I thought of if the player was facing 90 degrees, so the angle we check is
	270. Essentially, we get the angle between the player and thing as if the player is rotated at 90
	degrees. The orientations used are as follows:

	0 - Facing the player.
	1 - Facing slightly left of the player (South west).
	2 - Facing westward relative to the player.
	3 - Facing northwest relative to the player.
	4 - Facing north relative to the player.
	5 - Facing northeast relative to the player.
	6 - Facing eastward relative to the player.
	7 - Facing southeast relative to the player.
*/
int get_thing_orientation(int thing_rot, int player_rot) {
	// Figures out what the thing rotation is relative to 90 degrees.
	// We pick 90 degrees because the sprite orientation will make sense
	// visually.
	int transformed_rotation;
	// A number from 0 to 7 that tells us what orientation to use.
	int orientation;

	// Compute orientation data.
	// Get relative angle of thing assuming player was facing 90 degrees.
	transformed_rotation = (thing_rot - player_rot) + 90;
	// Correct transformed_rotation
	if(transformed_rotation < 0)
		transformed_rotation += 360;
	if(transformed_rotation >= 360)
		transformed_rotation -= 360;

	if(0 <= transformed_rotation && transformed_rotation <= 5)
		orientation = 6;
	else if(6 <= transformed_rotation && transformed_rotation <= 84)
		orientation = 5;
	else if(85 <= transformed_rotation && transformed_rotation <= 95)
		orientation = 4;
	else if(96 <= transformed_rotation && transformed_rotation <= 174)
		orientation = 3;
	else if(175 <= transformed_rotation && transformed_rotation <= 185)
		orientation = 2;
	else if(186 <= transformed_rotation && transformed_rotation <= 264)
		orientation = 1;
	else if(265 <= transformed_rotation && transformed_rotation <= 275)
		orientation = 0;
	else if(276 <= transformed_rotation && transformed_rotation <= 354)
		orientation = 7;
	else
		orientation = 6;

	return orientation;
}
