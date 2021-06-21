// Liam Wynn, 7/10/2019, Raycore

#include "./thing.h"
#include "./map.h"

#include <stdio.h>

// Single-frame prop
static void initialize_anim_class_0(struct thingdef* empty_thingdef);
// 8-frame prop. Each frame for a different relative orientation to the player.
static void initialize_anim_class_1(struct thingdef* empty_thingdef);
// Have 16 animations, one for each orientation to the player.
// There is also a corresponding single frame animation for each orientation.
// So 2 animations per direction, hence 16 animations. Also, the animated
// orientations have 4 frames.
static void initialize_anim_class_2(struct thingdef* empty_thingdef);

static void update_anim_class_0(struct thingdef* thing, const int player_rot);
static void update_anim_class_1(struct thingdef* thing, const int player_rot);
static void update_anim_class_2(struct thingdef* thing, const int player_rot);

static unsigned int get_pixel(SDL_Surface* surface, int x, int y) {
	if(!surface)
		return 0;
	if(x < 0 || x >= surface->w)
		return 0;
	if(y < 0 || y >= surface->h)
		return 0;

  SDL_LockSurface(surface);

	unsigned int result = 0;
	int bytes_per_pixel = surface->format->BytesPerPixel;

	if(bytes_per_pixel == 3) {
		unsigned char* channels = (unsigned char*)surface->pixels + y * surface->pitch + x * bytes_per_pixel;
		if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
			result = 0xFF000000 | channels[0] << 16 | channels[1] << 8 | channels[2];
		else
			result = 0xFF000000 | channels[2] << 16 | channels[1] << 8 | channels[0];
  } else if(bytes_per_pixel == 4) {
		unsigned char r, g, b, a;
		unsigned int pixel = *((unsigned int*)surface->pixels + y * surface->w + x);
		SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);
		if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
			result = ((unsigned int)b) << 24 | ((unsigned int)g)  << 16 | ((unsigned int)r)<< 8 | ((unsigned int)a);
		else
			result = ((unsigned int)a) << 24 | ((unsigned int)r)  << 16 | ((unsigned int)g)<< 8 | ((unsigned int)b);
  }

  SDL_UnlockSurface(surface);

	return result;
}

int create_thingdef(struct thingdef* empty_thingdef, char* sprite_sheet, int anim_class, int x, int y, int rot) {
	if(!empty_thingdef || !sprite_sheet)
		return 0;

	// TODO: Remove type since we have entities (originally, 
	// type was a way to distinguish different things when things and entities were the same).
	empty_thingdef->type = 1;

	empty_thingdef->id = 0;
	SDL_Surface* surf = SDL_LoadBMP(sprite_sheet);

  if(surf) {
    unsigned int tw, th, i, j;
    tw = surf->w;
    th = surf->h;
    empty_thingdef->tw = tw;
    empty_thingdef->th = th;
    empty_thingdef->data = (unsigned int*)malloc(sizeof(unsigned int) * tw * th);
    for(i = 0; i < tw; i++) {
      for(j = 0; j < th; j++)
        empty_thingdef->data[j * tw + i] = get_pixel(surf, i, j);
    }
  } else {
    printf("Could not load path to sprite sheet: %s\n", sprite_sheet);
    empty_thingdef->data = NULL;
    empty_thingdef->tw = 0;
    empty_thingdef->th = 0;
  }

	empty_thingdef->position[0] = x;
	empty_thingdef->position[1] = y;
	empty_thingdef->rotation = rot;

	// Will get set by the raycaster itself, but we'll make it 0 until then.
	empty_thingdef->dist = 0;

	empty_thingdef->anim_class = anim_class;
	empty_thingdef->curr_anim = 0;

	if(anim_class == 0)
		initialize_anim_class_0(empty_thingdef);
	else if(anim_class == 1)
		initialize_anim_class_1(empty_thingdef);
	else if(anim_class == 2)
		initialize_anim_class_2(empty_thingdef);
	else {
		printf("WARNING: thing has bad anim_class. Make sure it is set to 0, 1, or 2\n");
		initialize_anim_class_0(empty_thingdef);
	}

	clear_all_signals(empty_thingdef);

    empty_thingdef->active = 1;

	return 1;
}

static void initialize_anim_class_0(struct thingdef* empty_thingdef) {
	if(!empty_thingdef)
		return;

	empty_thingdef->num_anims = 1;
	empty_thingdef->anims[0].curr_frame = 0;
	empty_thingdef->anims[0].frame_time = 0;
	empty_thingdef->anims[0].bRepeats = 1;
	empty_thingdef->anims[0].start_x = 0;
	empty_thingdef->anims[0].start_y = 0;
}

static void initialize_anim_class_1(struct thingdef* empty_thingdef) {
	if(!empty_thingdef)
		return;

	empty_thingdef->num_anims = 1;
	empty_thingdef->anims[0].num_frames = 8;
	empty_thingdef->anims[0].frame_time = 0;
	empty_thingdef->anims[0].bRepeats = 1;
	empty_thingdef->anims[0].start_x = 0;
	empty_thingdef->anims[0].start_y = 0;
}

static void initialize_anim_class_2(struct thingdef* empty_thingdef) {
	if(!empty_thingdef)
		return;

	empty_thingdef->num_anims = 16;

	empty_thingdef->anims[0].num_frames = 1;
	empty_thingdef->anims[0].frame_time = 0;
	empty_thingdef->anims[0].bRepeats = 1;
	empty_thingdef->anims[0].start_x = 0;
	empty_thingdef->anims[0].start_y = 0;

	empty_thingdef->anims[1].num_frames = 4;
	empty_thingdef->anims[1].frame_time = 250;
	empty_thingdef->anims[1].bRepeats = 1;
	empty_thingdef->anims[1].start_x = 0;
	empty_thingdef->anims[1].start_y = 1;

	empty_thingdef->anims[2].num_frames = 1;
	empty_thingdef->anims[2].frame_time = 0;
	empty_thingdef->anims[2].bRepeats = 1;
	empty_thingdef->anims[2].start_x = 0;
	empty_thingdef->anims[2].start_y = 2;

	empty_thingdef->anims[3].num_frames = 4;
	empty_thingdef->anims[3].frame_time = 250;
	empty_thingdef->anims[3].bRepeats = 1;
	empty_thingdef->anims[3].start_x = 0;
	empty_thingdef->anims[3].start_y = 3;

	empty_thingdef->anims[4].num_frames = 1;
	empty_thingdef->anims[4].frame_time = 0;
	empty_thingdef->anims[4].bRepeats = 1;
	empty_thingdef->anims[4].start_x = 0;
	empty_thingdef->anims[4].start_y = 4;

	empty_thingdef->anims[5].num_frames = 4;
	empty_thingdef->anims[5].frame_time = 250;
	empty_thingdef->anims[5].bRepeats = 1;
	empty_thingdef->anims[5].start_x = 0;
	empty_thingdef->anims[5].start_y = 5;

	empty_thingdef->anims[6].num_frames = 1;
	empty_thingdef->anims[6].frame_time = 0;
	empty_thingdef->anims[6].bRepeats = 1;
	empty_thingdef->anims[6].start_x = 0;
	empty_thingdef->anims[6].start_y = 6;

	empty_thingdef->anims[7].num_frames = 4;
	empty_thingdef->anims[7].frame_time = 250;
	empty_thingdef->anims[7].bRepeats = 1;
	empty_thingdef->anims[7].start_x = 0;
	empty_thingdef->anims[7].start_y = 7;

	empty_thingdef->anims[8].num_frames = 1;
	empty_thingdef->anims[8].frame_time = 0;
	empty_thingdef->anims[8].bRepeats = 1;
	empty_thingdef->anims[8].start_x = 0;
	empty_thingdef->anims[8].start_y = 8;

	empty_thingdef->anims[9].num_frames = 4;
	empty_thingdef->anims[9].frame_time = 250;
	empty_thingdef->anims[9].bRepeats = 1;
	empty_thingdef->anims[9].start_x = 0;
	empty_thingdef->anims[9].start_y = 9;

	empty_thingdef->anims[10].num_frames = 1;
	empty_thingdef->anims[10].frame_time = 0;
	empty_thingdef->anims[10].bRepeats = 1;
	empty_thingdef->anims[10].start_x = 0;
	empty_thingdef->anims[10].start_y = 10;

	empty_thingdef->anims[11].num_frames = 4;
	empty_thingdef->anims[11].frame_time = 250;
	empty_thingdef->anims[11].bRepeats = 1;
	empty_thingdef->anims[11].start_x = 0;
	empty_thingdef->anims[11].start_y = 11;

	empty_thingdef->anims[12].num_frames = 1;
	empty_thingdef->anims[12].frame_time = 0;
	empty_thingdef->anims[12].bRepeats = 1;
	empty_thingdef->anims[12].start_x = 0;
	empty_thingdef->anims[12].start_y = 12;

	empty_thingdef->anims[13].num_frames = 4;
	empty_thingdef->anims[13].frame_time = 250;
	empty_thingdef->anims[13].bRepeats = 1;
	empty_thingdef->anims[13].start_x = 0;
	empty_thingdef->anims[13].start_y = 13;

	empty_thingdef->anims[14].num_frames = 1;
	empty_thingdef->anims[14].frame_time = 0;
	empty_thingdef->anims[14].bRepeats = 1;
	empty_thingdef->anims[14].start_x = 0;
	empty_thingdef->anims[14].start_y = 14;

	empty_thingdef->anims[15].num_frames = 4;
	empty_thingdef->anims[15].frame_time = 250;
	empty_thingdef->anims[15].bRepeats = 1;
	empty_thingdef->anims[15].start_x = 0;
	empty_thingdef->anims[15].start_y = 15;
}

void update_thing_anims(struct mapdef* map, const int player_rot) {
	if(!map)
		return;

	int i;
	for(i = 0; i < map->num_things; ++i) {
		if(map->things[i].anim_class == 0)
			update_anim_class_0(&map->things[i], player_rot);
		else if(map->things[i].anim_class == 1)
			update_anim_class_1(&map->things[i], player_rot);
		else if(map->things[i].anim_class == 2)
			update_anim_class_2(&map->things[i], player_rot);
	}
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

int clear_thingdef(struct thingdef* to_clean) {
	if(!to_clean)
		return 0;

	if(to_clean->data) {
		free(to_clean->data);
		to_clean->data = NULL;
	}

	return 1;
}

void set_signal_user_interact_on(struct thingdef* thing) {
	if(!thing)
		return;

	thing->signal_user_interact = 1;
}

void set_signal_user_interact_off(struct thingdef* thing) {
	if(!thing)
		return;

	thing->signal_user_interact = 0;
}

int check_signal_user_interact(struct thingdef* thing) {
	if(!thing)
		return 0;

	return thing->signal_user_interact;
}

void clear_all_signals(struct thingdef* thing) {
	if(!thing)
		return;

	set_signal_user_interact_off(thing);
}
