// Liam Wynn, 5/13/2018, CS410p: Full Stack Web Development

#include "raycaster.h"
#include "./util.h"

#include <stdio.h>
#include <math.h>

// TODO: Move this elsewhere.
//------------------------------------------------------------------
int get_dist_sqrd(int x1, int y1, int x2, int y2) {
	int d_x, d_y;

	d_x = x1 - x2;
	d_y = y1 - y2;

	d_x *= d_x;
	d_y *= d_y;

	return d_x + d_y;
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

// Precomputes all sqrts between 0 and STLEN
static int sqrt_table[STLEN];

// Stores the delta values for horizontal and vertical increments for each
// angle.
static int delta_h_x[361];
static int delta_h_y[361];
static int delta_v_x[361];
static int delta_v_y[361];

// Stores the distance each ray is from the player when it hits something.
int z_buffer[PROJ_W];
int z_buffer_2d[PROJ_W][PROJ_H];

// Lookup table for computing a floor/ceil pixel's distance from
// the player
int fc_proj_dist[200][361];
int fc_proj_dist_sqrt[200][361];
int fc_fe[200][361];

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
unsigned int thing_pixels[64000];
// Stores the things in a sorted order.
static struct thingdef* things_sorted[1000];
static int thing_sorted_index[THING_COUNT + MAX_CLIENTS];

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

static void compute_distance_to_player_for_each_thing();

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
unsigned int correct_hit_dist_for_fisheye_effect(const int);

static void draw_sky_slice(const int);

static void compute_wall_slice_render_data_from_hit_and_screen_col(struct hitinfo*, const int, struct wall_slice*);
static void draw_wall_slice(struct wall_slice*, struct hitinfo* hit);

static void draw_column_of_floor_and_ceiling_from_wall(struct wall_slice*);

static void draw_things();
static void project_thing_pos_onto_screen(const int[2], int[2]);
static void compute_thing_dimensions_on_screen(const int, const int[2], SDL_Rect*);
static void compute_frame_offset(const int, int[2]);
static void draw_columns_of_thing(const int, const SDL_Rect*, const int[2]);
static int column_in_bounds_of_screen(const int);
static void compute_column_of_thing_texture(const int, const SDL_Rect*, SDL_Rect*);
static void draw_column_of_thing_texture(struct thing_column_render_data*);
static int thing_pixel_is_not_transparent(const unsigned int);

static void render_pixel_arrays_to_screen(SDL_Renderer*);

void initialize_lookup_tables() {
	int deg;

	for(deg = 0; deg <= 360; ++deg)
		compute_lookup_vals_for_angle(deg);

  init_td_table();

  // Computes the table needed to project the floor/ceil screen pixel into
  // the world.
  int i, j;
  for(i = 100; i < 200; i++) {
    for(j = 0; j < 361; j++) {
      int c = cos128table[j];
      c = c == 0 ? 1 : c;
      int d = i == HALF_PROJ_H ? 1 : i - HALF_PROJ_H;
	    int straight_dist = (int)(DIST_TO_PROJ * HALF_UNIT_SIZE / d);
	    int dist_to_point = (straight_dist << 7) / c;
      fc_proj_dist[i][j] = dist_to_point;
      fc_proj_dist[-i + 199][j] = dist_to_point;
      fc_proj_dist_sqrt[i][j] = (int)sqrt(dist_to_point);
      fc_proj_dist_sqrt[-i + 199][j] = (int)sqrt(dist_to_point);
      ray_angle_relative_to_player_rot = j;
      fc_fe[i][j] = correct_hit_dist_for_fisheye_effect(dist_to_point);
      fc_fe[-i + 199][j] = fc_fe[i][j];
    }
  }

  for(i = 0; i < STLEN; i++)
    sqrt_table[i] = (int)sqrt(i);
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
  compute_distance_to_player_for_each_thing();
  memset(map->vis, 0, map->map_w * map->map_h);
	clean_pixel_arrays();

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
    thing_sorted_index[i] = i;
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
    if(wall_slice.screen_height > 15)
      draw_wall_slice(&wall_slice, &hit);

		// FLOOR AND CEILING RENDERING
		if(render_floors) {
			draw_column_of_floor_and_ceiling_from_wall(&wall_slice);
			render_floors = 0;
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
    set_vis(curr_pos[0], curr_pos[1], map, 1);
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
  int dist = get_dist_sqrd(player_x, player_y, hit_pos[0], hit_pos[1]);
  to_set->dist = dist < STLEN ? sqrt_table[dist] : sqrt(dist);
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

unsigned int correct_hit_dist_for_fisheye_effect(const int hit_dist) {
	unsigned int correct_dist = (hit_dist * cos128table[ray_angle_relative_to_player_rot]) >> 7;

	// Make sure we don't get any issues computing the slice height.
	if(correct_dist == 0)
		correct_dist = 1;

	return correct_dist;
}

static void draw_sky_slice(const int screen_col) {
	if(!map || !map->sky_data)
		return;

	// A skybox has 640 columns of pixels.
	// There are 360 degrees in a circle.
	// So to convert an angle to the corresponding column,
	// we do 640 / 360, which is roughly 1.77. We can just round this
	// to two.
	int adj_angle_to_pixel_col = (adj_ray_angle << 1) % SKYBOX_TEX_WIDTH;

  raycast_pixels[0 + screen_col] = map->sky_data[0 + adj_angle_to_pixel_col];
  raycast_pixels[320 + screen_col] = map->sky_data[640 + adj_angle_to_pixel_col];
  raycast_pixels[640 + screen_col] = map->sky_data[1280 + adj_angle_to_pixel_col];
  raycast_pixels[960 + screen_col] = map->sky_data[1920 + adj_angle_to_pixel_col];
  raycast_pixels[1280 + screen_col] = map->sky_data[2560 + adj_angle_to_pixel_col];
  raycast_pixels[1600 + screen_col] = map->sky_data[3200 + adj_angle_to_pixel_col];
  raycast_pixels[1920 + screen_col] = map->sky_data[3840 + adj_angle_to_pixel_col];
  raycast_pixels[2240 + screen_col] = map->sky_data[4480 + adj_angle_to_pixel_col];
  raycast_pixels[2560 + screen_col] = map->sky_data[5120 + adj_angle_to_pixel_col];
  raycast_pixels[2880 + screen_col] = map->sky_data[5760 + adj_angle_to_pixel_col];
  raycast_pixels[3200 + screen_col] = map->sky_data[6400 + adj_angle_to_pixel_col];
  raycast_pixels[3520 + screen_col] = map->sky_data[7040 + adj_angle_to_pixel_col];
  raycast_pixels[3840 + screen_col] = map->sky_data[7680 + adj_angle_to_pixel_col];
  raycast_pixels[4160 + screen_col] = map->sky_data[8320 + adj_angle_to_pixel_col];
  raycast_pixels[4480 + screen_col] = map->sky_data[8960 + adj_angle_to_pixel_col];
  raycast_pixels[4800 + screen_col] = map->sky_data[9600 + adj_angle_to_pixel_col];
  raycast_pixels[5120 + screen_col] = map->sky_data[10240 + adj_angle_to_pixel_col];
  raycast_pixels[5440 + screen_col] = map->sky_data[10880 + adj_angle_to_pixel_col];
  raycast_pixels[5760 + screen_col] = map->sky_data[11520 + adj_angle_to_pixel_col];
  raycast_pixels[6080 + screen_col] = map->sky_data[12160 + adj_angle_to_pixel_col];
  raycast_pixels[6400 + screen_col] = map->sky_data[12800 + adj_angle_to_pixel_col];
  raycast_pixels[6720 + screen_col] = map->sky_data[13440 + adj_angle_to_pixel_col];
  raycast_pixels[7040 + screen_col] = map->sky_data[14080 + adj_angle_to_pixel_col];
  raycast_pixels[7360 + screen_col] = map->sky_data[14720 + adj_angle_to_pixel_col];
  raycast_pixels[7680 + screen_col] = map->sky_data[15360 + adj_angle_to_pixel_col];
  raycast_pixels[8000 + screen_col] = map->sky_data[16000 + adj_angle_to_pixel_col];
  raycast_pixels[8320 + screen_col] = map->sky_data[16640 + adj_angle_to_pixel_col];
  raycast_pixels[8640 + screen_col] = map->sky_data[17280 + adj_angle_to_pixel_col];
  raycast_pixels[8960 + screen_col] = map->sky_data[17920 + adj_angle_to_pixel_col];
  raycast_pixels[9280 + screen_col] = map->sky_data[18560 + adj_angle_to_pixel_col];
  raycast_pixels[9600 + screen_col] = map->sky_data[19200 + adj_angle_to_pixel_col];
  raycast_pixels[9920 + screen_col] = map->sky_data[19840 + adj_angle_to_pixel_col];
  raycast_pixels[10240 + screen_col] = map->sky_data[20480 + adj_angle_to_pixel_col];
  raycast_pixels[10560 + screen_col] = map->sky_data[21120 + adj_angle_to_pixel_col];
  raycast_pixels[10880 + screen_col] = map->sky_data[21760 + adj_angle_to_pixel_col];
  raycast_pixels[11200 + screen_col] = map->sky_data[22400 + adj_angle_to_pixel_col];
  raycast_pixels[11520 + screen_col] = map->sky_data[23040 + adj_angle_to_pixel_col];
  raycast_pixels[11840 + screen_col] = map->sky_data[23680 + adj_angle_to_pixel_col];
  raycast_pixels[12160 + screen_col] = map->sky_data[24320 + adj_angle_to_pixel_col];
  raycast_pixels[12480 + screen_col] = map->sky_data[24960 + adj_angle_to_pixel_col];
  raycast_pixels[12800 + screen_col] = map->sky_data[25600 + adj_angle_to_pixel_col];
  raycast_pixels[13120 + screen_col] = map->sky_data[26240 + adj_angle_to_pixel_col];
  raycast_pixels[13440 + screen_col] = map->sky_data[26880 + adj_angle_to_pixel_col];
  raycast_pixels[13760 + screen_col] = map->sky_data[27520 + adj_angle_to_pixel_col];
  raycast_pixels[14080 + screen_col] = map->sky_data[28160 + adj_angle_to_pixel_col];
  raycast_pixels[14400 + screen_col] = map->sky_data[28800 + adj_angle_to_pixel_col];
  raycast_pixels[14720 + screen_col] = map->sky_data[29440 + adj_angle_to_pixel_col];
  raycast_pixels[15040 + screen_col] = map->sky_data[30080 + adj_angle_to_pixel_col];
  raycast_pixels[15360 + screen_col] = map->sky_data[30720 + adj_angle_to_pixel_col];
  raycast_pixels[15680 + screen_col] = map->sky_data[31360 + adj_angle_to_pixel_col];
  raycast_pixels[16000 + screen_col] = map->sky_data[32000 + adj_angle_to_pixel_col];
  raycast_pixels[16320 + screen_col] = map->sky_data[32640 + adj_angle_to_pixel_col];
  raycast_pixels[16640 + screen_col] = map->sky_data[33280 + adj_angle_to_pixel_col];
  raycast_pixels[16960 + screen_col] = map->sky_data[33920 + adj_angle_to_pixel_col];
  raycast_pixels[17280 + screen_col] = map->sky_data[34560 + adj_angle_to_pixel_col];
  raycast_pixels[17600 + screen_col] = map->sky_data[35200 + adj_angle_to_pixel_col];
  raycast_pixels[17920 + screen_col] = map->sky_data[35840 + adj_angle_to_pixel_col];
  raycast_pixels[18240 + screen_col] = map->sky_data[36480 + adj_angle_to_pixel_col];
  raycast_pixels[18560 + screen_col] = map->sky_data[37120 + adj_angle_to_pixel_col];
  raycast_pixels[18880 + screen_col] = map->sky_data[37760 + adj_angle_to_pixel_col];
  raycast_pixels[19200 + screen_col] = map->sky_data[38400 + adj_angle_to_pixel_col];
  raycast_pixels[19520 + screen_col] = map->sky_data[39040 + adj_angle_to_pixel_col];
  raycast_pixels[19840 + screen_col] = map->sky_data[39680 + adj_angle_to_pixel_col];
  raycast_pixels[20160 + screen_col] = map->sky_data[40320 + adj_angle_to_pixel_col];
  raycast_pixels[20480 + screen_col] = map->sky_data[40960 + adj_angle_to_pixel_col];
  raycast_pixels[20800 + screen_col] = map->sky_data[41600 + adj_angle_to_pixel_col];
  raycast_pixels[21120 + screen_col] = map->sky_data[42240 + adj_angle_to_pixel_col];
  raycast_pixels[21440 + screen_col] = map->sky_data[42880 + adj_angle_to_pixel_col];
  raycast_pixels[21760 + screen_col] = map->sky_data[43520 + adj_angle_to_pixel_col];
  raycast_pixels[22080 + screen_col] = map->sky_data[44160 + adj_angle_to_pixel_col];
  raycast_pixels[22400 + screen_col] = map->sky_data[44800 + adj_angle_to_pixel_col];
  raycast_pixels[22720 + screen_col] = map->sky_data[45440 + adj_angle_to_pixel_col];
  raycast_pixels[23040 + screen_col] = map->sky_data[46080 + adj_angle_to_pixel_col];
  raycast_pixels[23360 + screen_col] = map->sky_data[46720 + adj_angle_to_pixel_col];
  raycast_pixels[23680 + screen_col] = map->sky_data[47360 + adj_angle_to_pixel_col];
  raycast_pixels[24000 + screen_col] = map->sky_data[48000 + adj_angle_to_pixel_col];
  raycast_pixels[24320 + screen_col] = map->sky_data[48640 + adj_angle_to_pixel_col];
  raycast_pixels[24640 + screen_col] = map->sky_data[49280 + adj_angle_to_pixel_col];
  raycast_pixels[24960 + screen_col] = map->sky_data[49920 + adj_angle_to_pixel_col];
  raycast_pixels[25280 + screen_col] = map->sky_data[50560 + adj_angle_to_pixel_col];
  raycast_pixels[25600 + screen_col] = map->sky_data[51200 + adj_angle_to_pixel_col];
  raycast_pixels[25920 + screen_col] = map->sky_data[51840 + adj_angle_to_pixel_col];
  raycast_pixels[26240 + screen_col] = map->sky_data[52480 + adj_angle_to_pixel_col];
  raycast_pixels[26560 + screen_col] = map->sky_data[53120 + adj_angle_to_pixel_col];
  raycast_pixels[26880 + screen_col] = map->sky_data[53760 + adj_angle_to_pixel_col];
  raycast_pixels[27200 + screen_col] = map->sky_data[54400 + adj_angle_to_pixel_col];
  raycast_pixels[27520 + screen_col] = map->sky_data[55040 + adj_angle_to_pixel_col];
  raycast_pixels[27840 + screen_col] = map->sky_data[55680 + adj_angle_to_pixel_col];
  raycast_pixels[28160 + screen_col] = map->sky_data[56320 + adj_angle_to_pixel_col];
  raycast_pixels[28480 + screen_col] = map->sky_data[56960 + adj_angle_to_pixel_col];
  raycast_pixels[28800 + screen_col] = map->sky_data[57600 + adj_angle_to_pixel_col];
  raycast_pixels[29120 + screen_col] = map->sky_data[58240 + adj_angle_to_pixel_col];
  raycast_pixels[29440 + screen_col] = map->sky_data[58880 + adj_angle_to_pixel_col];
  raycast_pixels[29760 + screen_col] = map->sky_data[59520 + adj_angle_to_pixel_col];
  raycast_pixels[30080 + screen_col] = map->sky_data[60160 + adj_angle_to_pixel_col];
  raycast_pixels[30400 + screen_col] = map->sky_data[60800 + adj_angle_to_pixel_col];
  raycast_pixels[30720 + screen_col] = map->sky_data[61440 + adj_angle_to_pixel_col];
  raycast_pixels[31040 + screen_col] = map->sky_data[62080 + adj_angle_to_pixel_col];
  raycast_pixels[31360 + screen_col] = map->sky_data[62720 + adj_angle_to_pixel_col];
  raycast_pixels[31680 + screen_col] = map->sky_data[63360 + adj_angle_to_pixel_col];
  raycast_pixels[32000 + screen_col] = map->sky_data[64000 + adj_angle_to_pixel_col];
  raycast_pixels[32320 + screen_col] = map->sky_data[64640 + adj_angle_to_pixel_col];
  raycast_pixels[32640 + screen_col] = map->sky_data[65280 + adj_angle_to_pixel_col];
  raycast_pixels[32960 + screen_col] = map->sky_data[65920 + adj_angle_to_pixel_col];
  raycast_pixels[33280 + screen_col] = map->sky_data[66560 + adj_angle_to_pixel_col];
  raycast_pixels[33600 + screen_col] = map->sky_data[67200 + adj_angle_to_pixel_col];
  raycast_pixels[33920 + screen_col] = map->sky_data[67840 + adj_angle_to_pixel_col];
  raycast_pixels[34240 + screen_col] = map->sky_data[68480 + adj_angle_to_pixel_col];
  raycast_pixels[34560 + screen_col] = map->sky_data[69120 + adj_angle_to_pixel_col];
  raycast_pixels[34880 + screen_col] = map->sky_data[69760 + adj_angle_to_pixel_col];
  raycast_pixels[35200 + screen_col] = map->sky_data[70400 + adj_angle_to_pixel_col];
  raycast_pixels[35520 + screen_col] = map->sky_data[71040 + adj_angle_to_pixel_col];
  raycast_pixels[35840 + screen_col] = map->sky_data[71680 + adj_angle_to_pixel_col];
  raycast_pixels[36160 + screen_col] = map->sky_data[72320 + adj_angle_to_pixel_col];
  raycast_pixels[36480 + screen_col] = map->sky_data[72960 + adj_angle_to_pixel_col];
  raycast_pixels[36800 + screen_col] = map->sky_data[73600 + adj_angle_to_pixel_col];
  raycast_pixels[37120 + screen_col] = map->sky_data[74240 + adj_angle_to_pixel_col];
  raycast_pixels[37440 + screen_col] = map->sky_data[74880 + adj_angle_to_pixel_col];
  raycast_pixels[37760 + screen_col] = map->sky_data[75520 + adj_angle_to_pixel_col];
  raycast_pixels[38080 + screen_col] = map->sky_data[76160 + adj_angle_to_pixel_col];
  raycast_pixels[38400 + screen_col] = map->sky_data[76800 + adj_angle_to_pixel_col];
  raycast_pixels[38720 + screen_col] = map->sky_data[77440 + adj_angle_to_pixel_col];
  raycast_pixels[39040 + screen_col] = map->sky_data[78080 + adj_angle_to_pixel_col];
  raycast_pixels[39360 + screen_col] = map->sky_data[78720 + adj_angle_to_pixel_col];
  raycast_pixels[39680 + screen_col] = map->sky_data[79360 + adj_angle_to_pixel_col];
  raycast_pixels[40000 + screen_col] = map->sky_data[80000 + adj_angle_to_pixel_col];
  raycast_pixels[40320 + screen_col] = map->sky_data[80640 + adj_angle_to_pixel_col];
  raycast_pixels[40640 + screen_col] = map->sky_data[81280 + adj_angle_to_pixel_col];
  raycast_pixels[40960 + screen_col] = map->sky_data[81920 + adj_angle_to_pixel_col];
  raycast_pixels[41280 + screen_col] = map->sky_data[82560 + adj_angle_to_pixel_col];
  raycast_pixels[41600 + screen_col] = map->sky_data[83200 + adj_angle_to_pixel_col];
  raycast_pixels[41920 + screen_col] = map->sky_data[83840 + adj_angle_to_pixel_col];
  raycast_pixels[42240 + screen_col] = map->sky_data[84480 + adj_angle_to_pixel_col];
  raycast_pixels[42560 + screen_col] = map->sky_data[85120 + adj_angle_to_pixel_col];
  raycast_pixels[42880 + screen_col] = map->sky_data[85760 + adj_angle_to_pixel_col];
  raycast_pixels[43200 + screen_col] = map->sky_data[86400 + adj_angle_to_pixel_col];
  raycast_pixels[43520 + screen_col] = map->sky_data[87040 + adj_angle_to_pixel_col];
  raycast_pixels[43840 + screen_col] = map->sky_data[87680 + adj_angle_to_pixel_col];
  raycast_pixels[44160 + screen_col] = map->sky_data[88320 + adj_angle_to_pixel_col];
  raycast_pixels[44480 + screen_col] = map->sky_data[88960 + adj_angle_to_pixel_col];
  raycast_pixels[44800 + screen_col] = map->sky_data[89600 + adj_angle_to_pixel_col];
  raycast_pixels[45120 + screen_col] = map->sky_data[90240 + adj_angle_to_pixel_col];
  raycast_pixels[45440 + screen_col] = map->sky_data[90880 + adj_angle_to_pixel_col];
  raycast_pixels[45760 + screen_col] = map->sky_data[91520 + adj_angle_to_pixel_col];
  raycast_pixels[46080 + screen_col] = map->sky_data[92160 + adj_angle_to_pixel_col];
  raycast_pixels[46400 + screen_col] = map->sky_data[92800 + adj_angle_to_pixel_col];
  raycast_pixels[46720 + screen_col] = map->sky_data[93440 + adj_angle_to_pixel_col];
  raycast_pixels[47040 + screen_col] = map->sky_data[94080 + adj_angle_to_pixel_col];
  raycast_pixels[47360 + screen_col] = map->sky_data[94720 + adj_angle_to_pixel_col];
  raycast_pixels[47680 + screen_col] = map->sky_data[95360 + adj_angle_to_pixel_col];
  raycast_pixels[48000 + screen_col] = map->sky_data[96000 + adj_angle_to_pixel_col];
  raycast_pixels[48320 + screen_col] = map->sky_data[96640 + adj_angle_to_pixel_col];
  raycast_pixels[48640 + screen_col] = map->sky_data[97280 + adj_angle_to_pixel_col];
  raycast_pixels[48960 + screen_col] = map->sky_data[97920 + adj_angle_to_pixel_col];
  raycast_pixels[49280 + screen_col] = map->sky_data[98560 + adj_angle_to_pixel_col];
  raycast_pixels[49600 + screen_col] = map->sky_data[99200 + adj_angle_to_pixel_col];
  raycast_pixels[49920 + screen_col] = map->sky_data[99840 + adj_angle_to_pixel_col];
  raycast_pixels[50240 + screen_col] = map->sky_data[100480 + adj_angle_to_pixel_col];
  raycast_pixels[50560 + screen_col] = map->sky_data[101120 + adj_angle_to_pixel_col];
  raycast_pixels[50880 + screen_col] = map->sky_data[101760 + adj_angle_to_pixel_col];
  raycast_pixels[51200 + screen_col] = map->sky_data[102400 + adj_angle_to_pixel_col];
  raycast_pixels[51520 + screen_col] = map->sky_data[103040 + adj_angle_to_pixel_col];
  raycast_pixels[51840 + screen_col] = map->sky_data[103680 + adj_angle_to_pixel_col];
  raycast_pixels[52160 + screen_col] = map->sky_data[104320 + adj_angle_to_pixel_col];
  raycast_pixels[52480 + screen_col] = map->sky_data[104960 + adj_angle_to_pixel_col];
  raycast_pixels[52800 + screen_col] = map->sky_data[105600 + adj_angle_to_pixel_col];
  raycast_pixels[53120 + screen_col] = map->sky_data[106240 + adj_angle_to_pixel_col];
  raycast_pixels[53440 + screen_col] = map->sky_data[106880 + adj_angle_to_pixel_col];
  raycast_pixels[53760 + screen_col] = map->sky_data[107520 + adj_angle_to_pixel_col];
  raycast_pixels[54080 + screen_col] = map->sky_data[108160 + adj_angle_to_pixel_col];
  raycast_pixels[54400 + screen_col] = map->sky_data[108800 + adj_angle_to_pixel_col];
  raycast_pixels[54720 + screen_col] = map->sky_data[109440 + adj_angle_to_pixel_col];
  raycast_pixels[55040 + screen_col] = map->sky_data[110080 + adj_angle_to_pixel_col];
  raycast_pixels[55360 + screen_col] = map->sky_data[110720 + adj_angle_to_pixel_col];
  raycast_pixels[55680 + screen_col] = map->sky_data[111360 + adj_angle_to_pixel_col];
  raycast_pixels[56000 + screen_col] = map->sky_data[112000 + adj_angle_to_pixel_col];
  raycast_pixels[56320 + screen_col] = map->sky_data[112640 + adj_angle_to_pixel_col];
  raycast_pixels[56640 + screen_col] = map->sky_data[113280 + adj_angle_to_pixel_col];
  raycast_pixels[56960 + screen_col] = map->sky_data[113920 + adj_angle_to_pixel_col];
  raycast_pixels[57280 + screen_col] = map->sky_data[114560 + adj_angle_to_pixel_col];
  raycast_pixels[57600 + screen_col] = map->sky_data[115200 + adj_angle_to_pixel_col];
  raycast_pixels[57920 + screen_col] = map->sky_data[115840 + adj_angle_to_pixel_col];
  raycast_pixels[58240 + screen_col] = map->sky_data[116480 + adj_angle_to_pixel_col];
  raycast_pixels[58560 + screen_col] = map->sky_data[117120 + adj_angle_to_pixel_col];
  raycast_pixels[58880 + screen_col] = map->sky_data[117760 + adj_angle_to_pixel_col];
  raycast_pixels[59200 + screen_col] = map->sky_data[118400 + adj_angle_to_pixel_col];
  raycast_pixels[59520 + screen_col] = map->sky_data[119040 + adj_angle_to_pixel_col];
  raycast_pixels[59840 + screen_col] = map->sky_data[119680 + adj_angle_to_pixel_col];
  raycast_pixels[60160 + screen_col] = map->sky_data[120320 + adj_angle_to_pixel_col];
  raycast_pixels[60480 + screen_col] = map->sky_data[120960 + adj_angle_to_pixel_col];
  raycast_pixels[60800 + screen_col] = map->sky_data[121600 + adj_angle_to_pixel_col];
  raycast_pixels[61120 + screen_col] = map->sky_data[122240 + adj_angle_to_pixel_col];
  raycast_pixels[61440 + screen_col] = map->sky_data[122880 + adj_angle_to_pixel_col];
  raycast_pixels[61760 + screen_col] = map->sky_data[123520 + adj_angle_to_pixel_col];
  raycast_pixels[62080 + screen_col] = map->sky_data[124160 + adj_angle_to_pixel_col];
  raycast_pixels[62400 + screen_col] = map->sky_data[124800 + adj_angle_to_pixel_col];
  raycast_pixels[62720 + screen_col] = map->sky_data[125440 + adj_angle_to_pixel_col];
  raycast_pixels[63040 + screen_col] = map->sky_data[126080 + adj_angle_to_pixel_col];
  raycast_pixels[63360 + screen_col] = map->sky_data[126720 + adj_angle_to_pixel_col];
  raycast_pixels[63680 + screen_col] = map->sky_data[127360 + adj_angle_to_pixel_col];
}

static void compute_wall_slice_render_data_from_hit_and_screen_col(struct hitinfo* hit, const int screen_col, struct wall_slice* slice) {
	// Height of the slice in the world
	unsigned int slice_height;
	// The amount of projected wall slice above the first 64 pixels.
	unsigned int slice_remain;
	unsigned int tex_h;

	slice->wall_tex = hit->wall_type - map->num_floor_ceils;

	if(!map->walls[slice->wall_tex].data)
		return;

	tex_h = map->walls[slice->wall_tex].th;

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
  unsigned int* wall_tex_data = map->walls[slice->wall_tex].data;

	if(!wall_tex_data)
		return;

	int pixel_index;
	int p_x, p_y;
  int tw, th;
  sdata d;

  tw = map->walls[slice->wall_tex].tw;
  th = map->walls[slice->wall_tex].th;

	p_x = slice->tex_col;

  d.dst = raycast_pixels;
  d.src = wall_tex_data;
  d.dc = slice->screen_col;
  d.tc = slice->tex_col;
  d.hdist = hit->dist;

	// Manually copies texture from source to portion of screen.
  if(slice->screen_row >= 0 && slice->screen_row + slice->screen_height < PROJ_H) {
    // Case 1: Entire slice can be seen on camera
    d.dr = slice->screen_row;
    d.td = get_texture_delta(th, slice->screen_height);
    d.ofst = 0;
    scale_to_i(&d, slice->screen_height);
	} else if(slice->screen_row < 0 && slice->screen_row + slice->screen_height >= PROJ_H) {
    // Case 2: You are very close to wall, and can only see a portion of it
    // from y = mx + b. Setting up relationship where x is slice rows
    // and y is texture rows
    float m = (float)th / slice->screen_height;
    float b = -m * slice->screen_row;
    int start = (int)b;
    int end = (int)(PROJ_H * m + b);
    int sz = end - start;
    d.dr = 0;
    d.ofst = start;
    d.td = get_texture_delta(sz, PROJ_H);
    scale_to_i(&d, PROJ_H);
  } else if(slice->screen_row < 0 && slice->screen_row + slice->screen_height >= 0 && slice->screen_row + slice->screen_height < PROJ_H) {
    // Case 3: You cannot see the top of the wall, but can see the bottom. Typically
    // what happens when dealing with variable height walls.
    float m = (float)th / slice->screen_height;
    float b = -m * slice->screen_row;
    int start = (int)b;
    int end = th;
    int tsz = end - start;
    int ssz = slice->screen_height + slice->screen_row;
    d.dr = 0;
    d.ofst = start;
    d.td = get_texture_delta(tsz, ssz);
    scale_to_i(&d, ssz);
  } else if(slice->screen_row >= 0 && slice->screen_row + slice->screen_height >= PROJ_H) {
    // Case 4: You can see the top of the slice, but not the bottom. This a result of some approximation issues.
    int start = slice->screen_row;
    float m = (float)th / slice->screen_height;
    float b = -m * start;
    int te = (int)(m * PROJ_H + b);
    int sz = PROJ_H - start;
    d.dr = start;
    d.ofst = 0;
    d.td = get_texture_delta(te, sz);
    scale_to_i(&d, sz);
   }
}

static void draw_column_of_floor_and_ceiling_from_wall(struct wall_slice* wall_slice) {
  fcd args;
	const int bottom = wall_slice->screen_row + wall_slice->screen_height;

  args.sc = wall_slice->screen_col;
  args.sv = sin128table[adj_ray_angle];
  args.cv = cos128table[adj_ray_angle];
  args.ray_angle_relative_to_player_rot = ray_angle_relative_to_player_rot;
  args.map = map;
  args.fcp = floor_ceiling_pixels;
  args.player_x = player_x;
  args.player_y = player_y;

  draw_fc(bottom, &args);
}

static void render_pixel_arrays_to_screen(SDL_Renderer* renderer) {
	SDL_UpdateTexture(raycast_texture, NULL, raycast_pixels, PROJ_W << 2);
	SDL_RenderCopy(renderer, raycast_texture, NULL, NULL);

	SDL_UpdateTexture(floor_ceiling_tex, NULL, floor_ceiling_pixels, PROJ_W << 2);
	SDL_RenderCopy(renderer, floor_ceiling_tex, NULL, NULL);

	SDL_UpdateTexture(thing_texture, NULL, thing_pixels, PROJ_W << 2);
	SDL_RenderCopy(renderer, thing_texture, NULL, NULL);
}

static void sort_thing_indexes() {
  int n = map->num_things;
  int i, j, c;
  for(i = 0; i < n; i++) {
    for(j = i + 1; j < n; j++) {
      if(map->things[thing_sorted_index[i]].dist <
         map->things[thing_sorted_index[j]].dist) {
        c = thing_sorted_index[i];
        thing_sorted_index[i] = thing_sorted_index[j];
        thing_sorted_index[j] = c;
      }
    }
  }
}

static void draw_things() {
	// The position of the sprite on the screen.
	int screen_pos[2];

	// Defines the sprite's screen dimensions and position.
	SDL_Rect thing_rect;
	// How much we add to t_x, t_y to get the correct animation frame.
	int frame_offset[2];

  sort_thing_indexes();

	int i;
  int ind;
	for(i = 0; i < map->num_things; ++i) {
    ind = thing_sorted_index[i];
    if(get_vis(map->things[ind].position[0], map->things[ind].position[1], map) == 0)
      continue;

		if(map->things[ind].dist == 0 || map->things[ind].dist >= MAX_DIST_SQRD)
      continue;
		if(!map->things[ind].data || map->things[ind].type == 0 || map->things[ind].active == 0)
			continue;

		project_thing_pos_onto_screen(map->things[ind].position, screen_pos);
		compute_thing_dimensions_on_screen(ind, screen_pos, &thing_rect);
		compute_frame_offset(ind, frame_offset);
		draw_columns_of_thing(ind, &thing_rect, frame_offset);
	}
}

static void project_thing_pos_onto_screen(const int thing_pos[2], int screen_pos[2]) {
  /*int x_diff = thing_pos[0] - player_x;
  int y_diff = thing_pos[1] - player_y;

  // Flip the y axes to get the correct orientation
  y_diff = -y_diff;

  // player_rot as a vector
  int uc = cos128table[player_rot];
  int us = sin128table[player_rot];

  // player_rot cross product. Vector for the player projection plane
  int vc = us;
  int vs = -uc;

  // Magnitude of projection of diff vector onto v
  screen_pos[0] = (vc * x_diff + vs * y_diff) >> 7;
  screen_pos[0] += HALF_PROJ_W;
  screen_pos[1] = (uc * x_diff + us * y_diff) >> 7;*/

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

    //We make three assumptions about sprites and the environment
    //1. Sprites are vertically centered.
    //2. Sprites are all 64 x 64 pixels -- Actually this no longer holds
    //3. The player height is 32
    //Thus, the center of every sprite is at half of the projection screen height.
  screen_pos[1] = PROJ_H >> 1;
}

static void compute_thing_dimensions_on_screen(const int thing_sorted_index, const int screen_pos[2], SDL_Rect* thing_screen_rect) {
	int tex_h;

	if(map->things[thing_sorted_index].data && map->things[thing_sorted_index].anim_class == 0)
		tex_h = map->things[thing_sorted_index].th;
	else
		tex_h = 64;

  int distsqrd = map->things[thing_sorted_index].dist;
  double dist = distsqrd < STLEN ? sqrt_table[distsqrd] : sqrt(distsqrd);
	// How much we subtract from the thing height to render at the
	// correct row.
	int height_remain;

	// Inspired by the computation to find the height of a wall slice
	// on the screen.

	/*
	slice_remain = slice_height - ((DIST_TO_PROJ << 6) / hit->dist);
	*/

	/*thing_screen_rect->w = 64;
  thing_screen_rect->h = tex_h;
  thing_screen_rect->x = screen_pos[0];
  thing_screen_rect->y = HALF_PROJ_H - (tex_h >> 1);*/

	thing_screen_rect->w = (int)(UNIT_SIZE / dist * DIST_TO_PROJ);
	// Since sprites are squares, the screen width == screen height
	//thing_screen_rect->h = thing_screen_rect->w;
	thing_screen_rect->h = (int)(tex_h / dist * DIST_TO_PROJ);
	height_remain = thing_screen_rect->h - ((DIST_TO_PROJ << 6) / dist);
	// x and y are is the top-left corner of the screen.
	thing_screen_rect->y = HALF_PROJ_H - (thing_screen_rect->h >> 1) - (height_remain >> 1);
	thing_screen_rect->x = screen_pos[0] - (thing_screen_rect-> w >> 1);
}

static void compute_frame_offset(const int thing_sorted_index, int frame_offset[2]) {
	unsigned int curr_anim = map->things[thing_sorted_index].curr_anim;
	// Take starting position and multiply by 64 to go from unit coordinates to pixel coordinates.
	// This puts us in the correct position for the animation as a whole.
	frame_offset[0] = (int)(map->things[thing_sorted_index].anims[curr_anim].start_x) << 6;
	frame_offset[1] = (int)(map->things[thing_sorted_index].anims[curr_anim].start_y) << 6;
	// Add the current frame to the offset so that we have the correct frame.
	// Note that since animations progress only horizontally, we don't need to
	// do anything to the y part of the offset.
	frame_offset[0] += map->things[thing_sorted_index].anims[curr_anim].curr_frame << 6;
}

static void draw_columns_of_thing(const int thing_sorted_index, const SDL_Rect* dest, const int frame_offset[2]) {
	struct thing_column_render_data thing_column;
	// Defines the column of pixels of the sprite we want.
	SDL_Rect src_tex_col;
	
	// The column for the scaled texture.
	int m = 0;

	thing_column.thing_sorted_index = thing_sorted_index;
	thing_column.dest = dest;
	thing_column.frame_offset = frame_offset;

    int thing_col_y = dest->y;
    int thing_col_h = dest->h;
    int q = thing_col_y + thing_col_h;
    int start_row, end_row;

    start_row = thing_col_y >= 0 ? thing_col_y : 0;
    end_row = q < PROJ_H ? q : PROJ_H;

	thing_column.start_row = start_row;
	thing_column.end_row = end_row;

  thing_column.tex_height = UNIT_SIZE;

	int thing_dist = map->things[thing_sorted_index].dist;
	thing_column.thing_dist_sqrt = thing_dist < STLEN ? sqrt_table[thing_dist] : (int)sqrt(thing_dist);

  thing_column.map = map;

	int j;
	for(j = dest->x; j < dest->x + dest->w; ++j) {
		if(column_in_bounds_of_screen(j)) {
			compute_column_of_thing_texture(m, dest, &src_tex_col);

			thing_column.src = &src_tex_col;
			thing_column.screen_column = j;

			draw_column_of_thing_texture(&thing_column);
		}
		++m;
	}
}

static int column_in_bounds_of_screen(const int col) {
	return col >= 0 && col < PROJ_W;
}

static void compute_column_of_thing_texture(const int scaled_column, const SDL_Rect* rect_to_render, SDL_Rect* thing_src_rect) {
	thing_src_rect->x = (scaled_column << 6) / rect_to_render->w;
	thing_src_rect->y = 0;
	thing_src_rect->w = 1;
	thing_src_rect->h = UNIT_SIZE;
}

static void draw_column_of_thing_texture(struct thing_column_render_data* thing_column_data) {
  int len = thing_column_data->end_row - thing_column_data->start_row;
  thing_col(thing_column_data->dest->h, thing_column_data);
}

