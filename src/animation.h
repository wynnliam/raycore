// Liam Wynn, 6/26/2018, CS410p: Full Stack Web Development

/*
	Defines data structures and procedures relating to
	animations.

	Let's discuss the nature of animations a little bit here.
	We'll begin by defining a couple of things. We say that
	an animation is a series of frames, and each frame is a 64 x 64
	pixel square component of an overall sprite sheet. So in
	concrete terms, when we render an animation for a sprite, we
	render a square component of a larget image.

	There are a couple of requirements for animations for our system here.
	First, as I stated previously, all animation frames must be 64 by 64 pixels.
	Second, an animation goes from left to right. This means that the first frame
	of an animation sequence is the left most 64 by 64 pixel square. The rightmost
	one is its last.
*/

#ifndef ANIMATION
#define ANIMATION

struct animdef {
	// TODO: Finish me!
	// The number of 64 x 64 pixel square.
	unsigned int num_frames;
	// The time in milliseconds between frames.
	unsigned int frame_time;
	unsigned int curr_frame;
	// If we repeat the animation or not.
	int bRepeats;
	// If the animation is currently running or not.
	int bRunning;

	// When (SDL_GetTicks() - start_tick) >= frame_time, we update
	// our current frame.
	unsigned int start_tick;

	// The position of where this animation sequence starts on the
	// sprite sheet in unit values. That is, 1 is 64 pixels, 2 is 128, etc.
	unsigned char start_x, start_y;
};

/*
	Begins to play an animation. It does so by setting bRunning to true,
	and setting the value of start_tick to the current value SDL has.

	ARGUMENTS:
		anim - the animation to start/unpause.

	PRECONDITIONS:
		SDL Initialized, and animation set, and animation not already running.

	POSTCONDITIONS:
		Animation is now running (can use it in update_anim);
*/
void start_anim(struct animdef* anim);

/*
	Handles the logic of updating an animation. This is where the curr_frame
	gets updated and whatnot.

	ARGUMENTS:
		anim - the animation to update.

	PRECONDITIONS:
		anim is running.

	POSTCONDITIONS:
		anim is updated. Curr_frame may be different, as well as start_tick.
*/
void update_anim(struct animdef* anim);

/*
	Stops an animation that is currently playing. Resets animation by setting
	bRunning to false.

	ARGUMENTS:
		anim - the animation to stop.

	PRECONDITIONS:
		bRunning must be true.

	POSTCONDITIONS:
		bRunning set to false, thus stopping the animation.
*/
void stop_anim(struct animdef* anim);

/*
	A raycaster emulates 3D environments. One aspect of 3D environments is
	that you can look at an object in the world from multiple perspectives.
	If an object is looking at angle 90, but I'm staring at it from behind, I will
	see the "back" of it. Likewise, if the object is looking at angle 90, but I'm
	standing to the left of it looking it, I will see the object's profile. I call
	this the "orientation" of the object relative to the player. I could use the
	word "rotation" relative to the player, but I saved rotation for it's "global"
	rotation. If an object has rotation of 90 degrees, it is 90 in the global sense.

	This procedure handles that essentially, but using the rotation of the player and
	the thing. I put it under animations because the orientation directly relates to
	what animation is used. However, this procedure does not set a thing's animation.
	How the user wants to handle animations given this information is totally up to
	them.

	Finally, there are eight "orientations" this engine recognizes. They are listed as
	follows:

	0 - Facing the player.
	1 - Facing slightly left of the player (South west).
	2 - Facing westward relative to the player.
	3 - Facing northwest relative to the player.
	4 - Facing north relative to the player.
	5 - Facing northeast relative to the player.
	6 - Facing eastward relative to the player.
	7 - Facing southeast relative to the player.

	If you want other orientations, implement your own system. Otherwise, this
	is perfect for Wolfenstein 3D or DOOM styled things.

	ARGUMENTS:
		thing_rot - the global rotation of some thing.
		player_rot - the global rotation of the player.
		Note that we assume both of these values are between 0 and 360.

	RETURNS:
		the Orientation according to the key above.
*/
int get_thing_orientation(int thing_rot, int player_rot);

#endif
