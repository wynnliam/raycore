// Liam Wynn, 7/28/2018, Raycore

#include "map.h"

struct map_bounds {
	int x_min, y_min, x_max, y_max;
};

/*
	From the list of components that define a map, return rectangular bounds
	such that all components would be enclosed in them.
*/
int calculate_map_bounds(struct component* components, struct map_bounds* bounds);

/*
	Builds the wall and floor/ceiling texture lists for the map, given the components.
*/
int build_texture_lists(struct component* components, struct mapdef* map);

int find_fc_tex(char* tex_0, char* tex_1, struct floorcielingdef floor_ceils[100]);
int find_wall_tex(char* tex_0, struct walldef walls[100]);

int tex_match(char* mapdef_tex, char* map_data_tex);

int add_floor_ceil_tex(char* tex_0, char* tex_1, struct floorcielingdef floor_ceils[100], int index);
int add_wall_tex(char* tex_0, struct walldef walls[100], int index);

int add_things_to_map(struct mapdef* map, struct thing_data* things);

// Different thing factories
int add_thing_type_0(struct thingdef* thing, struct thing_data* data);
int add_thing_type_1(struct thingdef* thing, struct thing_data* data);

// Different animation factories TODO: put in anim.c
int add_anim_class_0(struct thingdef* thing);
int add_anim_class_1(struct thingdef* thing);
int add_anim_class_2(struct thingdef* thing);

/*
	Adds each component into the map level.
*/
int place_components_into_mapdef(struct mapdef* map, struct component* components);

/*
	Initializes all properties for map.
*/
int initialize_map_properties(struct mapdef* map, struct map_data* map_data);

void clean_walldef(struct walldef* to_clean);
void clean_floorcieldef(struct floorcielingdef* to_clean);
void clean_thing(struct thingdef* to_clean);

int build_mapdef_from_map_data(struct mapdef* mapdef, struct map_data* map_data, int* player_x, int* player_y, int* player_rot) {
	struct map_bounds bounds;

	if(!calculate_map_bounds(map_data->component_head, &bounds))
		return 0;

	mapdef->map_w = bounds.x_max - bounds.x_min;
	mapdef->map_h = bounds.y_max - bounds.y_min;
	mapdef->layout = (unsigned int*)malloc(mapdef->map_w * mapdef->map_h * sizeof(unsigned int));
	mapdef->invisible_walls = (int*)malloc(mapdef->map_w * mapdef->map_h * sizeof(int));

	int i;
	// Fills in the entire map with the first walldef.
	for(i = 0; i < mapdef->map_w * mapdef->map_h; ++i) {
		mapdef->layout[i] = 100;
		mapdef->invisible_walls[i] = 0;
	}

	for(i = 0; i < 100; ++i) {
		mapdef->walls[i].path = NULL;
		mapdef->walls[i].surf = NULL;

		mapdef->floor_ceils[i].floor_path = NULL;
		mapdef->floor_ceils[i].floor_surf = NULL;
		mapdef->floor_ceils[i].ceil_path = NULL;
		mapdef->floor_ceils[i].ceil_surf = NULL;
		mapdef->floor_ceils[i].invisible_wall = 0;
	}

	mapdef->num_wall_tex = 100;
	mapdef->num_floor_ceils = 100;
	mapdef->num_tiles = mapdef->num_wall_tex + mapdef->num_floor_ceils;

	build_texture_lists(map_data->component_head, mapdef);
	place_components_into_mapdef(mapdef, map_data->component_head);

	add_things_to_map(mapdef, map_data->thing_head);
	for(i = 0; i < mapdef->num_things; ++i) {
		if(mapdef->things[i].type == 0) {
			*player_x = mapdef->things[i].position[0];
			*player_y = mapdef->things[i].position[1];
			*player_rot = mapdef->things[i].rotation;
		}
	}

	initialize_map_properties(mapdef, map_data);

	return 1;
}

int calculate_map_bounds(struct component* components, struct map_bounds* bounds) {
	if(!components || !bounds)
		return 0;

	struct component* curr = components;

	bounds->x_min = curr->x;
	bounds->y_min = curr->y;
	// Adding width and height accounts for additional
	// area that furthest components occupy. The rule is that
	// components grow rightwards and downwards as we add to their
	// perimeter.
	bounds->x_max = curr->x + curr->w;
	bounds->y_max = curr->y + curr->h;

	while(curr) {
		if(curr->x < bounds->x_min)
			bounds->x_min = curr->x;
		if(curr->y < bounds->y_min)
			bounds->y_min = curr->y;

		if((curr->x + curr->w) > bounds->x_max)
			bounds->x_max = curr->x + curr->w;
		if((curr->y + curr->h) > bounds->y_max)
			bounds->y_max = curr->y + curr->h;

		curr = curr->next;
	}

	return 1;
}

int build_texture_lists(struct component* components, struct mapdef* map) {
	if(!components || !map)
		return 0;

	struct component* curr = components;
	int num_wall_tex = 0;
	int num_floor_ceil_tex = 0;

	while(curr) {
		if(curr->is_floor_ceil) {
			if(find_fc_tex(curr->tex_0, curr->tex_1, map->floor_ceils) == -1) {
				add_floor_ceil_tex(curr->tex_0, curr->tex_1, map->floor_ceils, num_floor_ceil_tex);
				++num_floor_ceil_tex;
			}
		}

		else {
			if(find_wall_tex(curr->tex_0, map->walls) == -1) {
				add_wall_tex(curr->tex_0, map->walls, num_wall_tex);
				++num_wall_tex;
			}
		}

		curr = curr->next;
	}

	return 1;
}

int find_fc_tex(char* tex_0, char* tex_1, struct floorcielingdef floor_ceils[100]) {
	int i;

	for(i = 0; i < 100; ++i) {
		if(tex_match(floor_ceils[i].floor_path, tex_0) && tex_match(floor_ceils[i].ceil_path, tex_1))
			return i;
	}

	return -1;
}

int find_wall_tex(char* tex_0, struct walldef walls[100]) {
	int i;

	for(i = 0; i < 100; ++i) {
		if(tex_match(walls[i].path, tex_0))
			return i;
	}

	return -1;
}

int tex_match(char* mapdef_tex, char* map_data_tex) {
	if(map_data_tex == NULL)
		return 0;
	else if(mapdef_tex == NULL && strcmp(map_data_tex, "0") != 0)
		return 0;
	else if(mapdef_tex == NULL && strcmp(map_data_tex, "0") == 0)
		return 1;
	else if(strcmp(mapdef_tex, map_data_tex) == 0)
		return 1;
	else
		return 0;
}

int add_floor_ceil_tex(char* tex_0, char* tex_1, struct floorcielingdef floor_ceils[100], int index) {
	if(strcmp(tex_0, "0") == 0) {
		floor_ceils[index].floor_path = NULL;
		floor_ceils[index].floor_surf = NULL;
	}

	else {
		floor_ceils[index].floor_path = (char*)malloc(strlen(tex_0) + 1);
		strcpy(floor_ceils[index].floor_path, tex_0);
		floor_ceils[index].floor_surf = SDL_LoadBMP(tex_0);
	}

	if(strcmp(tex_1, "0") == 0) {
		floor_ceils[index].ceil_path = NULL;
		floor_ceils[index].ceil_surf = NULL;
	}

	else {
		floor_ceils[index].ceil_path = (char*)malloc(strlen(tex_1) + 1);
		strcpy(floor_ceils[index].ceil_path, tex_1);
		floor_ceils[index].ceil_surf = SDL_LoadBMP(tex_1);
	}

	return 1;
}

int add_wall_tex(char* tex_0, struct walldef walls[100], int index) {
	if(strcmp(tex_0, "0") == 0) {
		walls[index].path = NULL;
		walls[index].path = NULL;
	}

	else {
		walls[index].path = (char*)malloc(strlen(tex_0) + 1);
		strcpy(walls[index].path, tex_0);
		walls[index].surf = SDL_LoadBMP(tex_0);
	}

	return 1;
}

int place_components_into_mapdef(struct mapdef* map, struct component* components) {
	if(!components || !map)
		return 0;

	struct component* curr = components;
	int x, y;
	unsigned int val;

	while(curr) {
		val = curr->is_floor_ceil ? find_fc_tex(curr->tex_0, curr->tex_1, map->floor_ceils) :
									find_wall_tex(curr->tex_0, map->walls) + 100;

		for(x = curr->x; x < curr->x + curr->w; ++x) {
			for(y = curr->y; y < curr->y + curr->h; ++y) {
				if(x < 0 || x >= map->map_w || y < 0 || y >= map->map_h)
					continue;

				map->layout[y * map->map_w + x] = val;
				map->invisible_walls[y * map->map_w + x] = curr->invisible_wall;
			}
		}

		curr = curr->next;
	}

	return 1;
}

int add_things_to_map(struct mapdef* map, struct thing_data* things) {
	if(!map || !things)
		return 0;

	struct thing_data* curr = things;
	int index = 0;

	while(curr) {
		// Sets all properties that are independent of the type.
		map->things[index].position[0] = curr->x;
		map->things[index].position[1] = curr->y;
		map->things[index].rotation = curr->rot;

		map->things[index].type = curr->type;
		if(curr->type == 0)
			add_thing_type_0(&map->things[index], curr);
		else if(curr->type == 1)
			add_thing_type_1(&map->things[index], curr);

		map->things[index].anim_class = curr->anim_class;
		if(curr->anim_class == 0)
			add_anim_class_0(&map->things[index]);
		else if(curr->anim_class == 1)
			add_anim_class_1(&map->things[index]);
		else if(curr->anim_class == 2)
			add_anim_class_2(&map->things[index]);

		curr = curr->next;
		++index;
	}

	map->num_things = index;

	return 1;
}

int add_thing_type_0(struct thingdef* thing, struct thing_data* data) {
	if(!thing)
		return 0;

	return 1;
}

int add_thing_type_1(struct thingdef* thing, struct thing_data* data) {
	if(!thing || !data)
		return 0;

	// TODO: Set according to 
	thing->surf = SDL_LoadBMP(data->sprite_sheet);

	return 1;
}

int add_anim_class_0(struct thingdef* thing) {
	thing->curr_anim = 0;
	thing->num_anims = 1;
	thing->anims[0].curr_frame = 0;
	thing->anims[0].frame_time = 0;
	thing->anims[0].bRepeats = 1;
	thing->anims[0].start_x = 0;
	thing->anims[0].start_y = 0;

	return 1;
}

int add_anim_class_1(struct thingdef* thing) {
	if(!thing)
		return 0;

	// A single animation where each frame is for a orientation.
	thing->curr_anim = 0;
	thing->anims[0].num_frames = 8;
	thing->anims[0].frame_time = 0;
	thing->anims[0].bRepeats = 1;
	thing->anims[0].start_x = 0;
	thing->anims[0].start_y = 0;

	return 1;
}

int add_anim_class_2(struct thingdef* thing) {
	if(!thing)
		return 0;

	thing->curr_anim = 0;

	thing->anims[0].num_frames = 1;
	thing->anims[0].frame_time = 0;
	thing->anims[0].bRepeats = 1;
	thing->anims[0].start_x = 0;
	thing->anims[0].start_y = 0;

	thing->anims[1].num_frames = 4;
	thing->anims[1].frame_time = 250;
	thing->anims[1].bRepeats = 1;
	thing->anims[1].start_x = 0;
	thing->anims[1].start_y = 1;

	thing->anims[2].num_frames = 1;
	thing->anims[2].frame_time = 0;
	thing->anims[2].bRepeats = 1;
	thing->anims[2].start_x = 0;
	thing->anims[2].start_y = 2;

	thing->anims[3].num_frames = 4;
	thing->anims[3].frame_time = 250;
	thing->anims[3].bRepeats = 1;
	thing->anims[3].start_x = 0;
	thing->anims[3].start_y = 3;

	thing->anims[4].num_frames = 1;
	thing->anims[4].frame_time = 0;
	thing->anims[4].bRepeats = 1;
	thing->anims[4].start_x = 0;
	thing->anims[4].start_y = 4;

	thing->anims[5].num_frames = 4;
	thing->anims[5].frame_time = 250;
	thing->anims[5].bRepeats = 1;
	thing->anims[5].start_x = 0;
	thing->anims[5].start_y = 5;

	thing->anims[6].num_frames = 1;
	thing->anims[6].frame_time = 0;
	thing->anims[6].bRepeats = 1;
	thing->anims[6].start_x = 0;
	thing->anims[6].start_y = 6;

	thing->anims[7].num_frames = 4;
	thing->anims[7].frame_time = 250;
	thing->anims[7].bRepeats = 1;
	thing->anims[7].start_x = 0;
	thing->anims[7].start_y = 7;

	thing->anims[8].num_frames = 1;
	thing->anims[8].frame_time = 0;
	thing->anims[8].bRepeats = 1;
	thing->anims[8].start_x = 0;
	thing->anims[8].start_y = 8;

	thing->anims[9].num_frames = 4;
	thing->anims[9].frame_time = 250;
	thing->anims[9].bRepeats = 1;
	thing->anims[9].start_x = 0;
	thing->anims[9].start_y = 9;

	thing->anims[10].num_frames = 1;
	thing->anims[10].frame_time = 0;
	thing->anims[10].bRepeats = 1;
	thing->anims[10].start_x = 0;
	thing->anims[10].start_y = 10;

	thing->anims[11].num_frames = 4;
	thing->anims[11].frame_time = 250;
	thing->anims[11].bRepeats = 1;
	thing->anims[11].start_x = 0;
	thing->anims[11].start_y = 11;

	thing->anims[12].num_frames = 1;
	thing->anims[12].frame_time = 0;
	thing->anims[12].bRepeats = 1;
	thing->anims[12].start_x = 0;
	thing->anims[12].start_y = 12;

	thing->anims[13].num_frames = 4;
	thing->anims[13].frame_time = 250;
	thing->anims[13].bRepeats = 1;
	thing->anims[13].start_x = 0;
	thing->anims[13].start_y = 13;

	thing->anims[14].num_frames = 1;
	thing->anims[14].frame_time = 0;
	thing->anims[14].bRepeats = 1;
	thing->anims[14].start_x = 0;
	thing->anims[14].start_y = 14;

	thing->anims[15].num_frames = 4;
	thing->anims[15].frame_time = 250;
	thing->anims[15].bRepeats = 1;
	thing->anims[15].start_x = 0;
	thing->anims[15].start_y = 15;

	return 1;
}

int initialize_map_properties(struct mapdef* map, struct map_data* map_data) {
	if(!map || !map_data)
		return 0;

	if(strcmp(map_data->sky_tex, "0")) {
		map->sky_surf = SDL_LoadBMP(map_data->sky_tex);
	} else {
		map->sky_surf = NULL;
	}

	map->use_fog = map_data->use_fog;
	map->fog_r = map_data->fog_r;
	map->fog_g = map_data->fog_g;
	map->fog_b = map_data->fog_b;

	return 1;
}

int clean_mapdef(struct mapdef* to_clean) {
	if(!to_clean)
		return 0;

	unsigned int i;

	if(to_clean->layout) {
		free(to_clean->layout);
		to_clean->layout = NULL;
	}

	if(to_clean->invisible_walls) {
		free(to_clean->invisible_walls);
		to_clean->invisible_walls = NULL;
	}

	if(to_clean->sky_surf) {
		SDL_FreeSurface(to_clean->sky_surf);
		to_clean->sky_surf = NULL;
	}

	for(i = 0; i < to_clean->num_wall_tex; ++i)
		clean_walldef(&(to_clean->walls[i]));

	for(i = 0; i < to_clean->num_floor_ceils; ++i)
		clean_floorcieldef(&(to_clean->floor_ceils[i]));

	for(i = 0; i < to_clean->num_things; ++i)
		clean_thing(&(to_clean->things[i]));

	return 1;
}

void clean_walldef(struct walldef* to_clean) {
	if(!to_clean)
		return;

	if(to_clean->path) {
		free(to_clean->path);
		to_clean->path = NULL;
	}

	if(to_clean->surf) {
		SDL_FreeSurface(to_clean->surf);
		to_clean->surf = NULL;
	}
}

void clean_floorcieldef(struct floorcielingdef* to_clean) {
	if(!to_clean)
		return;

	if(to_clean->floor_path) {
		free(to_clean->floor_path);
		to_clean->floor_path = NULL;
	}

	if(to_clean->ceil_path) {
		free(to_clean->ceil_path);
		to_clean->ceil_path = NULL;
	}

	if(to_clean->floor_surf) {
		SDL_FreeSurface(to_clean->floor_surf);
		to_clean->floor_surf = NULL;
	}

	if(to_clean->ceil_surf) {
		SDL_FreeSurface(to_clean->ceil_surf);
		to_clean->ceil_surf = NULL;
	}
}

void clean_thing(struct thingdef* to_clean) {
	if(!to_clean)
		return;

	if(to_clean->surf) {
		SDL_FreeSurface(to_clean->surf);
		to_clean->surf = NULL;
	}
}

struct mapdef* load_map(const char* path, int* player_x, int* player_y, int* player_rot) {
	if(!path)
		return NULL;

	FILE* map_file = fopen(path, "r");
	struct map_data* map_data = parse_to_map_data(map_file);
	struct mapdef* map = (struct mapdef*)malloc(sizeof(struct mapdef));

	build_mapdef_from_map_data(map, map_data, player_x, player_y, player_rot);

	fclose(map_file);
	
	clear_map_data(map_data);
	free(map_data);
	map_data = NULL;

	return map;
}

void free_map(struct mapdef** map) {
	if(!map || !(*map))
		return;

	clean_mapdef(*map);

	free(*map);
	*map = NULL;
}

int is_position_wall(struct mapdef* map, int player_x, int player_y) {
	int map_w = map->map_w;
	int index;

	player_x = player_x >> 6;
	player_y = player_y >> 6;

	index = player_y * map_w + player_x;

	return map->layout[index] >= map->num_floor_ceils || map->invisible_walls[index];
}
