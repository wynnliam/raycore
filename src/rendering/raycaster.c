// Liam Wynn, 5/13/2018, CS410p: Full Stack Web Development

#include "raycaster.h"

#include <stdio.h>
#include <math.h>

// TODO: Move this elsewhere.
//------------------------------------------------------------------
int get_tile(int x, int y, struct mapdef* map) {
	int grid_x = x >> UNIT_POWER;
	int grid_y = y >> UNIT_POWER;

	if(grid_x < 0 || grid_x > map->map_w - 1)
		return -1;
	if(grid_y < 0 || grid_y > map->map_h - 1)
		return -1;

	return map->layout[grid_y * map->map_w + grid_x];
}

int get_dist_sqrd(int x1, int y1, int x2, int y2) {
	int d_x, d_y;

	d_x = x1 - x2;
	d_y = y1 - y2;

	d_x *= d_x;
	d_y *= d_y;

	return d_x + d_y;
}

static unsigned int get_pixel(SDL_Surface* surface, int x, int y) {
	if(!surface)
		return 0;
	if(x < 0 || x >= surface->w)
		return 0;
	if(y < 0 || y >= surface->h)
		return 0;

	// Stores the channels of the pixel color.
	unsigned char* channels;
	// Used to compute channels and decides how we construct result.
	int bytes_per_pixel = surface->format->BytesPerPixel;
	// What we will return.
	unsigned int result;

	channels = (unsigned char*)surface->pixels + y * surface->pitch + x * bytes_per_pixel;

	switch(bytes_per_pixel) {
		case 1:
			result = 0xFF000000 | channels[0] << 16 | channels[1] << 8 | channels[2];
			break;
		case 2:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
				result = *(unsigned short*)(channels);
			break;
		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
				result = 0xFF000000 | channels[0] << 16 | channels[1] << 8 | channels[2];
			else
				result = 0xFF000000 | channels[2] << 16 | channels[1] << 8 | channels[0];
			break;
		case 4:
			result = *(unsigned int*)channels;
			break;
		default:
			result = 0;
			break;
	}

	return result;
}
//------------------------------------------------------------------

// RAYCASTER STATE VARIABLES
static int player_x, player_y;
static int player_rot;
static struct mapdef* map;
// Fog color as a single int.
static unsigned int fog_color;

// Stores the sin value of every degree from 0 to 360 multiplied by 128.
// This will enable us to preserve enough precision for each number as
// a byte. When we want a value, we can access sin128table[i] >> 7, which
// undoes the multiplication.
int sin128table[361];
// Stores the cos value of every degree from 0 to 360 multiplied by 128.
int cos128table[361];
// Stores every tan value of every degree from 0 to 360 multiplied by 128.
// The values for 0, 90, 180, 270, and 360 will be -1.
static int tan128table[361];

// Stores the value of 1/tan(t) * 128 for every degree between 0 and 360.
// The values for 0, 90, 180, 270, and 360 will be -1.
static int tan1table[361];
// Stores the value of 1/sin(t) * 128 for every degree between 0 and 360.
static int sin1table[361];

// Stores the delta values for horizontal and vertical increments for each
// angle.
static int delta_h_x[361];
static int delta_h_y[361];
static int delta_v_x[361];
static int delta_v_y[361];

// Stores the distance each ray is from the player when it hits something.
static int z_buffer[PROJ_W];
static int z_buffer_2d[PROJ_W][PROJ_H];

// What we render for the floor/ceiling
static SDL_Texture* floor_ceiling_tex;
// Where we store the floor and ceiling pixels before we render.
static unsigned int floor_ceiling_pixels[64000];

// Where we render for skybox and wall pixels.
static SDL_Texture* raycast_texture;
// Where we store sky and wall textures before we render.
static unsigned int raycast_pixels[64000];

// What we render for the things.
static SDL_Texture* thing_texture;
// Where we store thing pixels before we render.
static unsigned int thing_pixels[64000];
// Stores the things in a sorted order.
static struct thingdef* things_sorted[1000];

// Stores the precise angle of our current ray.
static float curr_ray_angle;
// The curr_ray_angle adjusted to be within 0 and 360.
static int adj_ray_angle;
static int ray_angle_relative_to_player_rot;

struct ray_data {
	int curr_h[2];
	int curr_v[2];
	int delta_h[2];
	int delta_v[2];
	int hit_h[2];
	int hit_v[2];
};

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

struct wall_slice {
	// The row of pixels on the screen we want to render from (top-most row going down).
	int screen_row;
	// The column of pixels on the screen we want to render from.
	int screen_col;
	// The height in pixels of the slice. No width since a slice is just a single line of pixels.
	int screen_height;
	// For drawing slices of walls behind this one. Note that this is backwards: as the slice
	// has greater height when rendering, this value gets smaller. This is because the y axis is
	// flipped.
	int highest_slice_row;
	// The texture we want to render.
	int wall_tex;
	// The column of pixels we want to render.
	int tex_col;
};

struct floor_ceiling_pixel {
	unsigned int texture;
	int screen_row;
	int screen_col;
	int world_space_coordinates[2];
};

struct thing_column_render_data {
	int thing_sorted_index;
	int screen_column;
	SDL_Rect* src;
	const SDL_Rect* dest;
	const int* frame_offset;
};

/*
	TODO: Nix this. We will keep track of distance as we fire rays.
	As rays travel we will determine if we need a fog factor or not.
*/

static unsigned int apply_fog(unsigned int pixel_color, const unsigned int dist) {
	unsigned char* color = (unsigned char*)&pixel_color;
	unsigned char fog_r, fog_g, fog_b;

	fog_r = (unsigned char)map->fog_r;
	fog_g = (unsigned char)map->fog_g;
	fog_b = (unsigned char)map->fog_b;

	int param_index = (dist << 2) >> 10;
	param_index = param_index > 4 ? 4 : param_index;
	unsigned char f = (unsigned char)param_index;
	unsigned char b = 4 - f;

	color[2] = ((fog_r * f) + (color[2] * b)) >> 2;
	color[1] = ((fog_g * f) + (color[1] * b)) >> 2;
	color[0] = ((fog_b * f) + (color[0] * b)) >> 2;

	return *(unsigned int*)color;
}

static void compute_lookup_vals_for_angle(const int);
static void compute_tan_lookup_val_for_angle(const int);
static void compute_inverse_tan_lookup_val_for_angle(const int);
static int is_tan_undefined_for_angle(const int);
static void compute_delta_lookup_vals_for_angle(const int);
static void compute_delta_lookup_vals_quadrant_1(const int);
static void compute_delta_lookup_vals_quadrant_2(const int);
static void compute_delta_lookup_vals_quadrant_3(const int);
static void compute_delta_lookup_vals_quadrant_4(const int);
static int is_angle_in_quadrant_1(const int);
static int is_angle_in_quadrant_2(const int);
static int is_angle_in_quadrant_3(const int);
static int is_angle_in_quadrant_4(const int);

static void update_state_variables(struct mapdef*, const int, const int, const int);

static void clean_pixel_arrays();

static void preprocess_things();
static void compute_distance_to_player_for_each_thing();
static void sort_things(int, int);
static int partition(int, int);

static void cast_single_ray(const int);
static void update_adjusted_angle();

static void get_ray_hit(struct ray_data*, struct hitinfo*);
static void choose_ray_horizontal_or_vertical_hit_pos(int[2], int[2], struct hitinfo*);
static int both_ray_horizontal_and_vertical_hit_pos_invalid(int[2], int[2]);
static int ray_hit_pos_is_invalid(int[2]);
static void set_hit(struct hitinfo*, int[2], const int);
static void choose_ray_pos_according_to_shortest_dist(struct hitinfo*, int[2], int[2]);

static int compute_initial_ray_pos(const int, int[2], int[2]);
static void compute_initial_ray_pos_when_angle_in_quad_1(const int, int[2], int[2]);
static void compute_initial_ray_pos_when_angle_in_quad_2(const int, int[2], int[2]);
static void compute_initial_ray_pos_when_angle_in_quad_3(const int, int[2], int[2]);
static void compute_initial_ray_pos_when_angle_in_quad_4(const int, int[2], int[2]);
static void compute_ray_delta_vectors(const int, int[2], int [2]);
static void compute_ray_hit_position(int[2], int[2], int[2]);
static int tile_is_floor_ceil(const int);
static void move_ray_pos(int[2], int[2]);

static int ray_hit_wall(struct hitinfo*);
static unsigned int correct_hit_dist_for_fisheye_effect(const int);

static void draw_sky_slice(const int);

static void compute_wall_slice_render_data_from_hit_and_screen_col(struct hitinfo*, const int, struct wall_slice*);
static void draw_wall_slice(struct wall_slice*, struct hitinfo* hit);

static void draw_column_of_floor_and_ceiling_from_wall(struct wall_slice*);
static int compute_row_for_bottom_of_wall_slice(struct wall_slice*);
static void project_screen_pixel_to_world_space(struct floor_ceiling_pixel*);
static void draw_floor_and_ceiling_pixels(struct floor_ceiling_pixel*, int);

static void draw_things();
static void project_thing_pos_onto_screen(const int[2], int[2]);
static void compute_thing_dimensions_on_screen(const int, const int[2], SDL_Rect*);
static void compute_frame_offset(const int, int[2]);
static void draw_columns_of_thing(const int, const SDL_Rect*, const int[2]);
static int column_in_bounds_of_screen(const int);
static int thing_not_obscured_by_wall_slice(int, int);
static void compute_column_of_thing_texture(const int, const SDL_Rect*, SDL_Rect*);
static void draw_column_of_thing_texture(struct thing_column_render_data*);
static int thing_pixel_row_out_of_screen_bounds(const int);
static int thing_pixel_is_not_transparent(const unsigned int);

static void render_pixel_arrays_to_screen(SDL_Renderer*);

void initialize_lookup_tables() {
	int deg;

	for(deg = 0; deg <= 360; ++deg)
		compute_lookup_vals_for_angle(deg);
}

static void compute_lookup_vals_for_angle(const int deg) {
	compute_tan_lookup_val_for_angle(deg);
	compute_inverse_tan_lookup_val_for_angle(deg);
	sin128table[deg] = (int)round(sin(deg * M_PI / 180.0f) * 128);
	cos128table[deg] = (int)round(cos(deg * M_PI / 180.0f) * 128);
	sin1table[deg] = (int)(round(128.0 / sin(deg * M_PI / 180.0f)));

	compute_delta_lookup_vals_for_angle(deg);
}

static void compute_tan_lookup_val_for_angle(const int deg) {
	if(is_tan_undefined_for_angle(deg))
		tan128table[deg] = -1;
	else
		tan128table[deg] = (int)round(tan(deg * M_PI / 180.0f) * 128);
}

static void compute_inverse_tan_lookup_val_for_angle(const int deg) {
	if(is_tan_undefined_for_angle(deg))
		tan1table[deg] = -1;
	else
		tan1table[deg] = (int)round(128.0 / tan(deg * M_PI / 180.0f));
}

static int is_tan_undefined_for_angle(const int deg) {
	return deg == 0 || deg == 90 || deg == 180 || deg == 270 || deg == 360;
}

static void compute_delta_lookup_vals_for_angle(const int deg) {
	if(is_tan_undefined_for_angle(deg)) {
		delta_h_x[deg] = 0;
		delta_h_y[deg] = 0;
		delta_v_x[deg] = 0;
		delta_v_y[deg] = 0;
	}

	if(is_angle_in_quadrant_1(deg)) {
		compute_delta_lookup_vals_quadrant_1(deg);
	} else if(is_angle_in_quadrant_2(deg)) {
		compute_delta_lookup_vals_quadrant_2(deg);
	} else if(is_angle_in_quadrant_3(deg)) {
		compute_delta_lookup_vals_quadrant_3(deg);
	} else if(is_angle_in_quadrant_4(deg)) {
		compute_delta_lookup_vals_quadrant_4(deg);
	}
}

static int is_angle_in_quadrant_1(const int deg) {
	return 1 <= deg && deg <= 89;
}

static int is_angle_in_quadrant_2(const int deg) {
	return 91 <= deg && deg <= 179;
}

static int is_angle_in_quadrant_3(const int deg) {
	return 181 <= deg && deg <= 269;
}

static int is_angle_in_quadrant_4(const int deg) {
	return 271 <= deg && deg <= 359;
}

static void compute_delta_lookup_vals_quadrant_1(const int deg) {
	// 64 / tan(ray_angle). We must account for the 128.
	delta_h_x[deg] = (1 << 13) / tan128table[deg];
	delta_h_y[deg]= -UNIT_SIZE;

	delta_v_x[deg] = UNIT_SIZE;
	// Compute -tan(angle) * 64
	delta_v_y[deg] = -((tan128table[deg] << UNIT_POWER) >> 7);
}

static void compute_delta_lookup_vals_quadrant_2(const int deg) {
	// -64, since we are travelling in the negative y direction.
	delta_h_y[deg] = -UNIT_SIZE;
	// Computes -64 / tan(deg). Negative since we're
	// travelling in the negative y direction.
	delta_h_x[deg] = (tan128table[deg - 90] * -UNIT_SIZE) >> 7;

	delta_v_x[deg] = -UNIT_SIZE;
	delta_v_y[deg] = -((1 << 13) / tan128table[deg - 90]);
}

static void compute_delta_lookup_vals_quadrant_3(const int deg) {
	delta_h_y[deg] = UNIT_SIZE;
	delta_h_x[deg]  = -(1 << 13) / tan128table[deg - 180];

	delta_v_x[deg] = -UNIT_SIZE;
	// Computes 64 * tan(ray_angle).
	delta_v_y[deg]  = (UNIT_SIZE * tan128table[deg - 180]) >> 7;
}

static void compute_delta_lookup_vals_quadrant_4(const int deg) {
	delta_h_y[deg] = UNIT_SIZE;
	// Computes 64 * tan(ray_angle)
	delta_h_x[deg] = (UNIT_SIZE * tan128table[deg - 270]) >> 7;

	delta_v_x[deg] = UNIT_SIZE;
	// Computes 64 / tan(ray_angle)
	delta_v_y[deg] = (1 << 13) / tan128table[deg - 270];
}

void initialize_render_textures(SDL_Renderer* renderer) {
	floor_ceiling_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, PROJ_W, PROJ_H);
	raycast_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, PROJ_W, PROJ_H);
	thing_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, PROJ_W, PROJ_H);

	SDL_SetTextureBlendMode(floor_ceiling_tex, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(raycast_texture, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(thing_texture, SDL_BLENDMODE_BLEND);
}

void cast_rays(SDL_Renderer* renderer, struct mapdef* curr_map, int curr_player_x, int curr_player_y, int curr_player_rot) {
	update_state_variables(curr_map, curr_player_x, curr_player_y, curr_player_rot);

	clean_pixel_arrays();
	compute_distance_to_player_for_each_thing();

	int screen_col;
	for(screen_col = 0; screen_col < PROJ_W; ++screen_col) {
		cast_single_ray(screen_col);

		curr_ray_angle -= ANGLE_BETWEEN_RAYS;
	}

	// THING CASTING
	draw_things();

	render_pixel_arrays_to_screen(renderer);
}

static void update_state_variables(struct mapdef* curr_map, const int curr_player_x, const int curr_player_y, const int curr_player_rot) {
	player_x = curr_player_x;
	player_y = curr_player_y;
	player_rot = curr_player_rot;
	map = curr_map;

	curr_ray_angle = (float)(player_rot + FOV_HALF);

	unsigned char* fog_color_arr = (unsigned char*)&fog_color;
	fog_color_arr[3] = 255;
	fog_color_arr[2] = (unsigned char)map->fog_r;
	fog_color_arr[1] = (unsigned char)map->fog_g;
	fog_color_arr[0] = (unsigned char)map->fog_b;
}

static void clean_pixel_arrays() {
	int i;
	for(i = 0; i < PROJ_W * PROJ_H; ++i) {
		floor_ceiling_pixels[i] = 0;
		raycast_pixels[i] = 0;
		thing_pixels[i] = 0;
	}
}

static void compute_distance_to_player_for_each_thing() {
	unsigned int i;

	// Compute the distance between each thing and the player.
	for(i = 0; i < map->num_things; ++i) {
		map->things[i].dist = get_dist_sqrd(map->things[i].position[0], map->things[i].position[1],
											player_x, player_y);

		if(map->things[i].dist == 0)
			map->things[i].dist = 1;

		// Add the thing to the sorted list.
		things_sorted[i] = &(map->things[i]);
	}
}

static void cast_single_ray(const int screen_col) {
	// Use this to keep persistent ray traverse information.
	struct ray_data ray_data;
	struct hitinfo hit;
	// Data needed to render a wall slice.
	struct wall_slice wall_slice;
	// We only want to render floors on the first ray hit.
	int render_floors = 1;

	update_adjusted_angle();

	if(compute_initial_ray_pos(adj_ray_angle, ray_data.curr_h, ray_data.curr_v) == 0)
		return;

	compute_ray_delta_vectors(adj_ray_angle, ray_data.delta_h, ray_data.delta_v);

	// SKY CASTING
	draw_sky_slice(screen_col);

	int i;
	for(i = 0; i < PROJ_H; i++)
		z_buffer_2d[screen_col][i] = -1;

	z_buffer[screen_col] = -1;
	wall_slice.highest_slice_row = PROJ_H;

	do {
		get_ray_hit(&ray_data, &hit);

		if(!ray_hit_wall(&hit))
			break;

		if(z_buffer[screen_col] == -1)
			z_buffer[screen_col] = hit.dist;

		// Computes the angle relative to the player rotation.
		ray_angle_relative_to_player_rot = abs(adj_ray_angle - player_rot);
		hit.dist = correct_hit_dist_for_fisheye_effect(hit.dist);

		// WALL CASTING
		compute_wall_slice_render_data_from_hit_and_screen_col(&hit, screen_col, &wall_slice);

		// Only render if we actually can see it.
		if(wall_slice.screen_height > 0)
			draw_wall_slice(&wall_slice, &hit);

		// FLOOR AND CEILING RENDERING
		if(render_floors) {
			draw_column_of_floor_and_ceiling_from_wall(&wall_slice);
			render_floors = 0;

			// TODO: Make seperate call for ceiling rendering when we do variable
			// height ceilings.
		}

		// After rendering, we need to move the ray curr_h and curr_v's again.
		move_ray_pos(ray_data.curr_h, ray_data.delta_h);
		move_ray_pos(ray_data.curr_v, ray_data.delta_v);
		
	} while(ray_hit_wall(&hit));

	// SKY CASTING
	//draw_sky_slice(screen_col);

	/*z_buffer[screen_col] = 0;
	get_ray_hit(adj_ray_angle, &hit);
	if(ray_hit_wall(&hit)) {
		z_buffer[screen_col] = hit.dist;

		// Computes the angle relative to the player rotation.
		ray_angle_relative_to_player_rot = abs(adj_ray_angle - player_rot);
		hit.dist = correct_hit_dist_for_fisheye_effect(hit.dist);

		// WALL CASTING
		compute_wall_slice_render_data_from_hit_and_screen_col(&hit, screen_col, &wall_slice);
		draw_wall_slice(&wall_slice, &hit);

		// FLOOR AND CEILING CASTING
		//draw_column_of_floor_and_ceiling_from_wall(&wall_slice);
	}*/
}

static void update_adjusted_angle() {
	adj_ray_angle = curr_ray_angle;

	// Make the angle between 0 and 360.
	if(adj_ray_angle < 0)
		adj_ray_angle += 360;
	if(adj_ray_angle > 360)
		adj_ray_angle -= 360;

	// First, we must deal with bad angles. These angles will break the raycaster, since
	// it will produce NaN's.
	if(adj_ray_angle == 360)
		adj_ray_angle = 0;
	if(adj_ray_angle == 0 || adj_ray_angle == 90 || adj_ray_angle == 180 || adj_ray_angle == 270)
		adj_ray_angle += 1;
}

static void get_ray_hit(struct ray_data* ray, struct hitinfo* hit) {
	// Now find the point that is a wall by travelling along horizontal gridlines.
	compute_ray_hit_position(ray->curr_h, ray->delta_h, ray->hit_h);
	// Now find the point that is a wall by travelling along vertical gridlines.
	compute_ray_hit_position(ray->curr_v, ray->delta_v, ray->hit_v);

	// Now choose either the horizontal or vertical intersection
	// point. Or choose -1, -1 to denote an error.
	choose_ray_horizontal_or_vertical_hit_pos(ray->hit_h, ray->hit_v, hit);

	hit->wall_type = get_tile(hit->hit_pos[0], hit->hit_pos[1], map);
}

static int compute_initial_ray_pos(const int ray_angle, int curr_h[2], int curr_v[2]) {
	if(is_tan_undefined_for_angle(ray_angle))
		return 0;

	if(is_angle_in_quadrant_1(ray_angle)) {
		compute_initial_ray_pos_when_angle_in_quad_1(ray_angle, curr_h, curr_v);
	} else if(is_angle_in_quadrant_2(ray_angle)) {
		compute_initial_ray_pos_when_angle_in_quad_2(ray_angle, curr_h, curr_v);
	} else if(is_angle_in_quadrant_3(ray_angle)) {
		compute_initial_ray_pos_when_angle_in_quad_3(ray_angle, curr_h, curr_v);
	} else if(is_angle_in_quadrant_4(ray_angle)) {
		compute_initial_ray_pos_when_angle_in_quad_4(ray_angle, curr_h, curr_v);
	}

	return 1;
}

static void compute_initial_ray_pos_when_angle_in_quad_1(const int ray_angle, int curr_h[2], int curr_v[2]) {
	int alpha = ray_angle;

	// Divide player_y by 64, floor that, multiply by 64, and then subtract 1.
	curr_h[1] = ((player_y >> UNIT_POWER) << UNIT_POWER) - 1;
	// Multiply player_y and curr_h[1] by 128, then divide by the tan * 128. This will
	// undo the the 128 multiplication without having a divide by 0 (For example, tan128table[1]).
	curr_h[0] = (((player_y - curr_h[1]) * tan1table[alpha]) >> 7) + player_x;

	// Divide player_x by 64, floor the result, multiply by 64 and add 64.
	curr_v[0]= ((player_x >> UNIT_POWER) << UNIT_POWER) + UNIT_SIZE;
	// Get the tan(curr_v[0] - player_x) and subtract that from player_y.
	curr_v[1]= player_y - (((curr_v[0]- player_x) * tan128table[alpha]) >> 7);
}

static void compute_initial_ray_pos_when_angle_in_quad_2(const int ray_angle, int curr_h[2], int curr_v[2]) {
	// Adjusts the angle so its between 1 and 89.
	int alpha = ray_angle - 90;
	// Compute floor(player_y / 64) * 64 - 1.
	curr_h[1] = ((player_y >> UNIT_POWER) << UNIT_POWER) - 1;
	// Computes player_x - (player_y * curr_h[1]) * tan(ray_angle).
	curr_h[0] = player_x - ((tan128table[alpha] * (player_y - curr_h[1])) >> 7);

	// Compute floor(player_x / 64) * 64 - 1.
	curr_v[0] = ((player_x >> UNIT_POWER) << UNIT_POWER) - 1;
	// Compute player_y - (player_x - curr_v[0]) / tan(ray_angle).
	curr_v[1] = player_y - (((player_x - curr_v[0]) * tan1table[alpha]) >> 7);
}

static void compute_initial_ray_pos_when_angle_in_quad_3(const int ray_angle, int curr_h[2], int curr_v[2]) {
	// Adjusts the angle so its between 1 and 89.
	int alpha = ray_angle - 180;

	// Computes floor(player_y / 64) * 64 + 64.
	curr_h[1] = ((player_y >> UNIT_POWER) << UNIT_POWER) + UNIT_SIZE;
	// Computes player_x - (curr_h[1] - player_y / tan(ray_angle).
	curr_h[0] = player_x - (((curr_h[1]- player_y) * tan1table[alpha]) >> 7);

	// Computes floor(player x / 64) * 64 - 1.
	curr_v[0] = ((player_x >> UNIT_POWER) << UNIT_POWER) - 1;
	// Computes tan(ray_angle) * (player_x - curr_v[0]) + player_y.
	curr_v[1] = ((tan128table[alpha] * (player_x  - curr_v[0])) >> 7) + player_y;
}

static void compute_initial_ray_pos_when_angle_in_quad_4(const int ray_angle, int curr_h[2], int curr_v[2]) {
	// Adjusts the angle so its between 1 and 89.
	int alpha = ray_angle - 270;

	// Computes floor(player_y / 64) * 64 + 64
	curr_h[1] = ((player_y >> UNIT_POWER) << UNIT_POWER) + UNIT_SIZE;
	// Computes (curr_h[1] - player_y) * tan(ray_angle) + player_x
	curr_h[0] = (((curr_h[1] - player_y) * tan128table[alpha]) >> 7) + player_x;

	// Computes floor(player_x / 64) * 64 + 64
	curr_v[0] = ((player_x >> UNIT_POWER) << UNIT_POWER) + UNIT_SIZE;
	// Computes (curr_v[0] - player_x) / tan(ray_angle) + player_y.
	curr_v[1] = (((curr_v[0] - player_x) * tan1table[alpha]) >> 7) + player_y;
}

static void compute_ray_delta_vectors(const int ray_angle, int delta_h[2], int delta_v[2]) {
	delta_h[0] = delta_h_x[ray_angle];
	delta_h[1] = delta_h_y[ray_angle];
	delta_v[0] = delta_v_x[ray_angle];
	delta_v[1] = delta_v_y[ray_angle];
}

static void compute_ray_hit_position(int curr_pos[2], int delta[2], int hit[2]) {
	int tile;

	tile = get_tile(curr_pos[0], curr_pos[1], map);
	while(tile_is_floor_ceil(tile)) {
		move_ray_pos(curr_pos, delta);
		tile = get_tile(curr_pos[0], curr_pos[1], map);
	}

	// We went outside the bounds of the map.
	if(tile == -1) {
		hit[0] = -1;
		hit[1] = -1;
	} else {
		hit[0] = curr_pos[0];
		hit[1] = curr_pos[1];
	}
}

static int tile_is_floor_ceil(const int tile) {
	return -1 < tile && tile < map->num_floor_ceils;
}

static void move_ray_pos(int ray_pos[2], int ray_delta[2]) {
	ray_pos[0] += ray_delta[0];
	ray_pos[1] += ray_delta[1];
}

static void choose_ray_horizontal_or_vertical_hit_pos(int hit_h[2], int hit_v[2], struct hitinfo* hit) {
	if(both_ray_horizontal_and_vertical_hit_pos_invalid(hit_h, hit_v)) {
		hit->hit_pos[0] = -1;
		hit->hit_pos[1] = -1;
	} else if(ray_hit_pos_is_invalid(hit_h)) {
		set_hit(hit, hit_v, 0);
	} else if(ray_hit_pos_is_invalid(hit_v)) {
		set_hit(hit, hit_h, 1);
	} else {
		choose_ray_pos_according_to_shortest_dist(hit, hit_h, hit_v);
	}
}

static int both_ray_horizontal_and_vertical_hit_pos_invalid(int hit_h[2], int hit_v[2]) {
	return hit_h[0] == -1 && hit_h[1] == -1 && hit_v[0] == -1 && hit_v[1] == -1;
}

static int ray_hit_pos_is_invalid(int hit_pos[2]) {
	return hit_pos[0] == -1 && hit_pos[1] == -1;
}

static void set_hit(struct hitinfo* to_set, int hit_pos[2], const int is_horiz) {
	to_set->hit_pos[0] = hit_pos[0];
	to_set->hit_pos[1] = hit_pos[1];
	to_set->is_horiz = is_horiz;
	to_set->dist = sqrt(get_dist_sqrd(player_x, player_y, hit_pos[0], hit_pos[1]));
}

static void choose_ray_pos_according_to_shortest_dist(struct hitinfo* hit, int hit_h[2], int hit_v[2]) {
	int h_dist;
	int v_dist;
	
	h_dist = get_dist_sqrd(player_x, player_y, hit_h[0], hit_h[1]);
	v_dist = get_dist_sqrd(player_x, player_y, hit_v[0], hit_v[1]);

	if(h_dist < v_dist) {
		set_hit(hit, hit_h, 1);
	} else {
		set_hit(hit, hit_v, 0);
	}
}

static int ray_hit_wall(struct hitinfo* hit) {
	return hit->hit_pos[0] != -1 && hit->hit_pos[1] != -1;
}

static unsigned int correct_hit_dist_for_fisheye_effect(const int hit_dist) {
	unsigned int correct_dist = (hit_dist * cos128table[ray_angle_relative_to_player_rot]) >> 7;

	// Make sure we don't get any issues computing the slice height.
	if(correct_dist == 0)
		correct_dist = 1;

	return correct_dist;
}

static void draw_sky_slice(const int screen_col) {
	if(!map || !map->sky_surf)
		return;

	// A skybox has 640 columns of pixels.
	// There are 360 degrees in a circle.
	// So to convert an angle to the corresponding column,
	// we do 640 / 360, which is roughly 1.77. We can just round this
	// to two.
	int adj_angle_to_pixel_col = (adj_ray_angle << 1) % SKYBOX_TEX_WIDTH;

	int j;
	for(j = 0; j < PROJ_H; ++j)
		raycast_pixels[j * PROJ_W + screen_col] = get_pixel(map->sky_surf, adj_angle_to_pixel_col, j);
}

static void compute_wall_slice_render_data_from_hit_and_screen_col(struct hitinfo* hit, const int screen_col, struct wall_slice* slice) {
	// Height of the slice in the world
	unsigned int slice_height;
	// The amount of projected wall slice above the first 64 pixels.
	unsigned int slice_remain;
	int tex_h;

	slice->wall_tex = hit->wall_type - map->num_floor_ceils;

	if(!map->walls[slice->wall_tex].surf)
		return;

	tex_h = map->walls[slice->wall_tex].surf->h;

	// Dist to projection * 64 / slice dist.
	slice_height = (DIST_TO_PROJ * tex_h) / hit->dist;
	slice_remain = slice_height - ((DIST_TO_PROJ << 6) / hit->dist);

	// Define the part of the screen we render to such that it is a single column with the
	// slice's middle pixel at the center of the screen.
	slice->screen_row  = HALF_PROJ_H - (slice_height >> 1) - (slice_remain >> 1);
	slice->screen_col = screen_col;

	// Case 1: We basically have not rendered a slice for this column yet.
	if(slice->highest_slice_row >= PROJ_H) {
		slice->screen_height = slice_height;
		slice->highest_slice_row = slice->screen_row;
	// Case 2: The slice we want to render is above the last highest one.
	// Note that the y axis is flipped in this renderer, so a larger slice->highest_slice_row
	// is rendered from a row below the current slice row.
	} else if(slice->highest_slice_row > slice->screen_row) {
		slice->screen_height = slice->highest_slice_row - slice->screen_row;
		slice->highest_slice_row = slice->screen_row;
	// Case 3: The slice we want to render is obscured by a slice we already rendered.
	} else {
		slice->screen_height = 0;
	}

	// Use a single column of pixels based on where the ray hit.
	slice->tex_col = hit->is_horiz ? (hit->hit_pos[0] % UNIT_SIZE) : (hit->hit_pos[1] % UNIT_SIZE);
}

static void draw_wall_slice(struct wall_slice* slice, struct hitinfo* hit) {
	SDL_Surface* tex = map->walls[slice->wall_tex].surf;

	if(!tex)
		return;

	int fog_dist  = hit->dist;
	int pixel_index;
	int p_x, p_y;
	int tex_h = tex->h;

	p_x = slice->tex_col;

	// Manually copies texture from source to portion of screen.
	int j;
	for(j = 0; j < slice->screen_height; ++j) {
		// j + screen_slice_y gives us the position to render the current pixel on the screen.
		if(j + slice->screen_row < 0 || j + slice->screen_row  >= PROJ_H)
			continue;

		z_buffer_2d[slice->screen_col][j + slice->screen_row] = hit->dist;

		pixel_index = (j + slice->screen_row) * PROJ_W + slice->screen_col;

		if(hit->dist <= 1024) {
			p_y = (j * tex_h) / slice->screen_height;
			raycast_pixels[pixel_index] = apply_fog(get_pixel(tex, p_x, p_y), fog_dist);
	 	} else
			raycast_pixels[pixel_index] = fog_color;
	}
}

static void draw_column_of_floor_and_ceiling_from_wall(struct wall_slice* wall_slice) {
	// Data needed to render a floor (and corresponding ceiling pixel).
	struct floor_ceiling_pixel floor_ceil_pixel;

	int pixel_dist;

	int j;
	for(j = compute_row_for_bottom_of_wall_slice(wall_slice); j < PROJ_H; ++j) {
		floor_ceil_pixel.screen_row = j;
		floor_ceil_pixel.screen_col = wall_slice->screen_col;
		project_screen_pixel_to_world_space(&floor_ceil_pixel);

		pixel_dist = get_dist_sqrd(floor_ceil_pixel.world_space_coordinates[0],
								   floor_ceil_pixel.world_space_coordinates[1],
								   player_x, player_y);
		pixel_dist = correct_hit_dist_for_fisheye_effect((int)sqrt(pixel_dist));

		floor_ceil_pixel.texture  = get_tile(floor_ceil_pixel.world_space_coordinates[0],
								   			 floor_ceil_pixel.world_space_coordinates[1],
								   			 map);

		if(floor_ceil_pixel.texture >= map->num_floor_ceils)
			continue;

		draw_floor_and_ceiling_pixels(&floor_ceil_pixel, pixel_dist);
	}
}

static int compute_row_for_bottom_of_wall_slice(struct wall_slice* wall_slice) {
	return wall_slice->screen_row + wall_slice->screen_height;
}

static void project_screen_pixel_to_world_space(struct floor_ceiling_pixel* floor_ceil_pixel) {
	// Compute the distance from the player to the point.
	int straight_dist = (int)(DIST_TO_PROJ * HALF_UNIT_SIZE / (floor_ceil_pixel->screen_row - HALF_PROJ_H));
	int dist_to_point = (straight_dist << 7) / (cos128table[ray_angle_relative_to_player_rot]);

	floor_ceil_pixel->world_space_coordinates[0] = player_x + ((dist_to_point * cos128table[adj_ray_angle]) >> 7);
	floor_ceil_pixel->world_space_coordinates[1] = player_y - ((dist_to_point * sin128table[adj_ray_angle]) >> 7);
}

static void draw_floor_and_ceiling_pixels(struct floor_ceiling_pixel* floor_ceil_pixel, int pixel_dist) {
	int texture_x = floor_ceil_pixel->world_space_coordinates[0] % UNIT_SIZE;
	int texture_y = floor_ceil_pixel->world_space_coordinates[1] % UNIT_SIZE;
	int floor_screen_pixel = floor_ceil_pixel->screen_row * PROJ_W + floor_ceil_pixel->screen_col;
	int ceiling_screen_pixel = ((-floor_ceil_pixel->screen_row) + PROJ_H) * PROJ_W + floor_ceil_pixel->screen_col;

	int use_fog = pixel_dist;

	// Put floor pixel.
	if(map->floor_ceils[floor_ceil_pixel->texture].floor_surf) {
		if(pixel_dist <= 1024)
			floor_ceiling_pixels[floor_screen_pixel] = apply_fog(get_pixel(map->floor_ceils[floor_ceil_pixel->texture].floor_surf, texture_x, texture_y),
																 use_fog);
		else
			floor_ceiling_pixels[floor_screen_pixel] = fog_color;
	}

	// Put ceiling pixel.
	if(map->floor_ceils[floor_ceil_pixel->texture].ceil_surf) {
		if(pixel_dist <= 1024)
			floor_ceiling_pixels[ceiling_screen_pixel] = apply_fog(get_pixel(map->floor_ceils[floor_ceil_pixel->texture].ceil_surf, texture_x, texture_y),
																   use_fog);
		else
			floor_ceiling_pixels[ceiling_screen_pixel] = fog_color;

		if(z_buffer_2d[floor_ceil_pixel->screen_col][-floor_ceil_pixel->screen_row + PROJ_H] == -1 || 
		   z_buffer_2d[floor_ceil_pixel->screen_col][-floor_ceil_pixel->screen_row + PROJ_H] > pixel_dist) {
			z_buffer_2d[floor_ceil_pixel->screen_col][-floor_ceil_pixel->screen_row + PROJ_H] = pixel_dist;
		}
	}
}

static void render_pixel_arrays_to_screen(SDL_Renderer* renderer) {
	SDL_UpdateTexture(raycast_texture, NULL, raycast_pixels, PROJ_W << 2);
	SDL_RenderCopy(renderer, raycast_texture, NULL, NULL);

	SDL_UpdateTexture(floor_ceiling_tex, NULL, floor_ceiling_pixels, PROJ_W << 2);
	SDL_RenderCopy(renderer, floor_ceiling_tex, NULL, NULL);

	SDL_UpdateTexture(thing_texture, NULL, thing_pixels, PROJ_W << 2);
	SDL_RenderCopy(renderer, thing_texture, NULL, NULL);
}

static void draw_things() {
	// The position of the sprite on the screen.
	int screen_pos[2];

	// Defines the sprite's screen dimensions and position.
	SDL_Rect thing_rect;
	// How much we add to t_x, t_y to get the correct animation frame.
	int frame_offset[2];

	int i;
	for(i = 0; i < map->num_things; ++i) {
		if(things_sorted[i]->type == 0)
			continue;

		project_thing_pos_onto_screen(things_sorted[i]->position, screen_pos);
		compute_thing_dimensions_on_screen(i, screen_pos, &thing_rect);
		compute_frame_offset(i, frame_offset);
		draw_columns_of_thing(i, &thing_rect, frame_offset);
	}
}

static void project_thing_pos_onto_screen(const int thing_pos[2], int screen_pos[2]) {
	int x_diff, y_diff;
	// The angle between the player and the thing to render.
	int theta_temp;
	// The ray angle that corresponds to the x position of the center
	// of the sprite/thing.
	int thing_ray_angle;

	x_diff = thing_pos[0] - player_x;
	y_diff = thing_pos[1] - player_y;
	theta_temp = (int)(atan2(-y_diff, x_diff) * RAD_TO_DEG);

	// Make sure the angle is between 0 and 360.
	if(theta_temp < 0)
		theta_temp += 360;

	thing_ray_angle = player_rot + FOV_HALF - theta_temp;

	// Corrects for case where player rotation is in quadrant 1,
	// but angle between player and thing is in quadrant 4 (or vice-versa).
	if(theta_temp > 270 && player_rot < 90)
		thing_ray_angle += 360;
	if(player_rot > 270 && theta_temp < 90)
		thing_ray_angle -= 360;

	screen_pos[0] = thing_ray_angle * PROJ_W / FOV;

	/*
		We make three assumptions about sprites and the environment
		1. Sprites are vertically centered.
		2. Sprites are all 64 x 64 pixels -- Actually this no longer holds
		3. The player height is 32

		Thus, the center of every sprite is at half of the projection screen height.
	*/
	screen_pos[1] = PROJ_H >> 1;
}

static void compute_thing_dimensions_on_screen(const int thing_sorted_index, const int screen_pos[2], SDL_Rect* thing_screen_rect) {
	int tex_h;

	if(things_sorted[thing_sorted_index]->surf)
		tex_h = things_sorted[thing_sorted_index]->surf->h;
	else
		tex_h = 64;

	int dist_squared = things_sorted[thing_sorted_index]->dist;
	double dist = sqrt(dist_squared);
	// How much we subtract from the thing height to render at the
	// correct row.
	int height_remain;

	// Inspired by the computation to find the height of a wall slice
	// on the screen.

	/*
	slice_remain = slice_height - ((DIST_TO_PROJ << 6) / hit->dist);
	*/

	thing_screen_rect->w = (int)(UNIT_SIZE / dist * DIST_TO_PROJ);
	// Since sprites are squares, the screen width == screen height
	//thing_screen_rect->h = thing_screen_rect->w;
	thing_screen_rect->h = (int)(tex_h / dist * DIST_TO_PROJ);
	height_remain = thing_screen_rect->h - ((DIST_TO_PROJ << 6) / dist);
	// x and y are is the top-left corner of the screen.
	thing_screen_rect->y = HALF_PROJ_H - (thing_screen_rect->h >> 1) - (height_remain >> 1);
	thing_screen_rect->x = screen_pos[0] - (thing_screen_rect->w >> 1);
}

static void compute_frame_offset(const int thing_sorted_index, int frame_offset[2]) {
	unsigned int curr_anim = things_sorted[thing_sorted_index]->curr_anim;
	// Take starting position and multiply by 64 to go from unit coordinates to pixel coordinates.
	// This puts us in the correct position for the animation as a whole.
	frame_offset[0] = (int)(things_sorted[thing_sorted_index]->anims[curr_anim].start_x) << 6;
	frame_offset[1] = (int)(things_sorted[thing_sorted_index]->anims[curr_anim].start_y) << 6;
	// Add the current frame to the offset so that we have the correct frame.
	// Note that since animations progress only horizontally, we don't need to
	// do anything to the y part of the offset.
	frame_offset[0] += things_sorted[thing_sorted_index]->anims[curr_anim].curr_frame << 6;
}

static void draw_columns_of_thing(const int thing_sorted_index, const SDL_Rect* dest, const int frame_offset[2]) {
	struct thing_column_render_data thing_column;
	// Defines the column of pixels of the sprite we want.
	SDL_Rect src_tex_col;
	
	// The column for the scaled texture.
	int m = 0;

	int j;
	for(j = dest->x; j < dest->x + dest->w; ++j) {
		if(column_in_bounds_of_screen(j)) {//&& thing_not_obscured_by_wall_slice(thing_sorted_index, j)) {
			compute_column_of_thing_texture(m, dest, &src_tex_col);

			thing_column.thing_sorted_index = thing_sorted_index;
			thing_column.screen_column = j;
			thing_column.src = &src_tex_col;
			thing_column.dest = dest;
			thing_column.frame_offset = frame_offset;

			//draw_column_of_thing_texture(thing_sorted_index, dest, &src_tex_col, frame_offset, j);
			draw_column_of_thing_texture(&thing_column);
		}
		++m;
	}
}

static int column_in_bounds_of_screen(const int col) {
	return col >= 0 && col < PROJ_W;
}

static int thing_not_obscured_by_wall_slice(int thing_sorted_index, int slice_column) {
	return sqrt(things_sorted[thing_sorted_index]->dist) - 1 < z_buffer[slice_column];
}

static void compute_column_of_thing_texture(const int scaled_column, const SDL_Rect* rect_to_render, SDL_Rect* thing_src_rect) {
	thing_src_rect->x = (scaled_column << 6) / rect_to_render->w;
	thing_src_rect->y = 0;
	thing_src_rect->w = 1;
	thing_src_rect->h = UNIT_SIZE;
}

static void draw_column_of_thing_texture(struct thing_column_render_data* thing_column_data) {
	// The texture point.
	int t_x, t_y;
	// RGB value of the sprite texture.
	unsigned int t_color;

	int screen_row;
	int tex_height;
	int thing_dist_sqrt = 0;

	if(things_sorted[thing_column_data->thing_sorted_index]->surf)
		tex_height = things_sorted[thing_column_data->thing_sorted_index]->surf->h;
	else
		tex_height = 64;

	thing_dist_sqrt = (int)sqrt(things_sorted[thing_column_data->thing_sorted_index]->dist);

	int k;
	for(k = 0; k < thing_column_data->dest->h; ++k) {
		screen_row = k + thing_column_data->dest->y;

		if(thing_pixel_row_out_of_screen_bounds(screen_row))
			continue;
		if(z_buffer_2d[thing_column_data->screen_column][screen_row] != -1 &&
		   thing_dist_sqrt > z_buffer_2d[thing_column_data->screen_column][screen_row])
			continue;

		t_x = (thing_column_data->src->x) + thing_column_data->frame_offset[0];
		t_y = ((k * tex_height) / thing_column_data->dest->h) + thing_column_data->frame_offset[1];
		t_color = get_pixel(things_sorted[thing_column_data->thing_sorted_index]->surf, t_x, t_y);
		// Only put a pixel if it is not transparent.
		if(thing_pixel_is_not_transparent(t_color)) {
			if(thing_dist_sqrt <= 1024)
				thing_pixels[(screen_row) * PROJ_W + thing_column_data->screen_column] = apply_fog(t_color, thing_dist_sqrt);
			else
				thing_pixels[(screen_row) * PROJ_W + thing_column_data->screen_column] = fog_color;

			z_buffer_2d[thing_column_data->screen_column][screen_row] = thing_dist_sqrt;
		}
	}
}

static int thing_pixel_row_out_of_screen_bounds(const int pixel) {
	return pixel < 0 || pixel >= PROJ_H;
}

static int thing_pixel_is_not_transparent(const unsigned int t_color) {
	return ((unsigned char*)&t_color)[3] > 0;
}

