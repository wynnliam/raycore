// Liam Wynn, 7/28/2018, Raycore

#include "map.h"
#include "./map_loading/map_from_file.h"

static void clean_walldef(struct walldef* to_clean);
static void clean_floorcieldef(struct floorcielingdef* to_clean);
static void clean_thing(struct thingdef* to_clean);

// TODO: Clean this up!
int initialize_map(struct mapdef* map) {
	if(!map)
		return 0;

	// Integer based iterators.
	unsigned int i, j;

	map->layout = NULL;
	map->invisible_walls = NULL;

	for(i = 0; i < WALL_DEF_COUNT; i++) {
		map->walls[i].path = NULL;
		map->walls[i].surf = NULL;
	}

	for(i = 0; i < FLOOR_CEIL_COUNT; i++) {
		map->floor_ceils[i].floor_path = NULL;
		map->floor_ceils[i].ceil_path = NULL;
		map->floor_ceils[i].floor_surf = NULL;
		map->floor_ceils[i].ceil_surf = NULL;
		map->floor_ceils[i].invisible_wall = 0;
	}

	map->sky_surf = NULL;

	for(i = 0; i < THING_COUNT; i++) {
		map->things[i].surf = NULL;

		for(j = 0; j < ANIM_COUNT; j++) {
			map->things[i].anims[j].num_frames = 0;
			map->things[i].anims[j].frame_time = 0;
			map->things[i].anims[j].curr_frame = 0;
			map->things[i].anims[j].bRepeats = 0;
			map->things[i].anims[j].bRunning = 0;
			map->things[i].anims[j].start_tick = 0;
			map->things[i].anims[j].start_x = 0;
			map->things[i].anims[j].start_y = 0;
		}

		map->things[i].position[0] = 0;
		map->things[i].position[1] = 0;
		map->things[i].rotation = 0;
		map->things[i].dist = 0;
		map->things[i].anim_class = 0;
		map->things[i].num_anims = 0;
		map->things[i].curr_anim = 0;
		map->things[i].type = 0;
	}

	for(i = 0; i < ENTITY_COUNT; i++) {
		map->entities[i] = NULL;
	}

	map->map_w = 0;
	map->map_h = 0;
	map->num_tiles = 0;
	map->num_wall_tex = 0;
	map->num_floor_ceils = 0;

	map->num_things = 0;
	map->num_entities = 0;

	map->use_fog = 0;

	return 1;
}

int insert_entity_into_map(struct mapdef* map, struct entity* entity) {
	if(!map || !entity)
		return 0;

	int result = 0;

	unsigned int i;
	for(i = 0; i < ENTITY_COUNT; i++) {
		if(!map->entities[i]) {
			map->entities[i] = entity;
			map->num_entities += 1;
			result = 1;
			break;
		}
	}

	return result;
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
