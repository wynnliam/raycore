// Liam Wynn, 7/28/2018, Raycore

#include "map.h"

#include "./entity/player_spawn_entity.h"

static void clean_walldef(struct walldef* to_clean);
static void clean_floorcieldef(struct floorcielingdef* to_clean);

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
		map->walls[i].data = NULL;
    map->walls[i].th = 0;
    map->walls[i].tw = 0;
	}

	for(i = 0; i < FLOOR_CEIL_COUNT; i++) {
		map->floor_ceils[i].floor_path = NULL;
    map->floor_ceils[i].dataf = NULL;
    map->floor_ceils[i].ftw = 0;
    map->floor_ceils[i].fth = 0;

		map->floor_ceils[i].ceil_path = NULL;
    map->floor_ceils[i].datac = NULL;
    map->floor_ceils[i].ctw = 0;
    map->floor_ceils[i].cth = 0;

		map->floor_ceils[i].invisible_wall = 0;
	}

	map->sky_data = NULL;

	for(i = 0; i < THING_COUNT; i++) {
		map->things[i].data = NULL;
    map->things[i].th = 0;
    map->things[i].tw = 0;

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

		clear_all_signals(&map->things[i]);
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

	map->signal_level_transition = -1;

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

int remove_entity_from_map(struct mapdef* map, const int id) {
	if(!map)
		return 0;

	int result = 0;

	unsigned int i;
	for(i = 0; i < ENTITY_COUNT; i++) {
		if(map->entities[i] && map->entities[i]->id == id) {
			map->entities[i]->clean(map->entities[i], map);
			free(map->entities[i]);
			map->entities[i] = NULL;
			map->num_entities -= 1;
			result = 1;
			break;
		}
	}

	return result;
}

void clear_all_thing_signals(struct mapdef* map) {
	if(!map)
		return;

	unsigned int i;
	for(i = 0; i < map->num_things; i++)
		clear_all_signals(&map->things[i]);
}

void update_entities(struct mapdef* map) {
	if(!map)
		return;

	unsigned int i;
	for(i = 0; i < ENTITY_COUNT; i++) {
		if(map->entities[i])
			map->entities[i]->update(map->entities[i], map);

		// TODO: Have signal system.
	}
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

	if(to_clean->sky_data) {
		free(to_clean->sky_data);
		to_clean->sky_data = NULL;
	}

	for(i = 0; i < to_clean->num_wall_tex; ++i)
		clean_walldef(&(to_clean->walls[i]));

	for(i = 0; i < to_clean->num_floor_ceils; ++i)
		clean_floorcieldef(&(to_clean->floor_ceils[i]));

	for(i = 0; i < to_clean->num_things; ++i)
		clear_thingdef(&(to_clean->things[i]));

	for(i = 0; i < ENTITY_COUNT; i++) {
		if(to_clean->entities[i])
			(*(to_clean->entities[i]->clean))(to_clean->entities[i], to_clean);
	}

	return 1;
}

void clean_walldef(struct walldef* to_clean) {
	if(!to_clean)
		return;

	if(to_clean->path) {
		free(to_clean->path);
		to_clean->path = NULL;
	}

  if(to_clean->data) {
    free(to_clean->data);
    to_clean->data = NULL;
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

	if(to_clean->dataf) {
		free(to_clean->dataf);
		to_clean->dataf = NULL;
	}

	if(to_clean->datac) {
		free(to_clean->datac);
		to_clean->datac = NULL;
	}
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

void spawn_player(struct mapdef* map, int* player_x, int* player_y, int* player_rot, int spawn_id) {
	if(!map || !player_x || !player_y || !player_rot || !spawn_id)
		return;

	struct entity* spawn = NULL;
	struct player_spawn_entity* data = NULL;
	int i;

	for(i = 0; i < ENTITY_COUNT; i++) {
		if(map->entities[i] && map->entities[i]->type == ENTITY_TYPE_PLAYER_SPAWN) {
			if(!spawn)
				spawn = map->entities[i];

			if(map->entities[i]->id == spawn_id) {
				spawn = map->entities[i];
				break;
			}
		}
	}

	if(spawn) {
		data = (struct player_spawn_entity*)spawn->data;

		*player_x = data->player_x;
		*player_y = data->player_y;
		*player_rot = data->player_rot;
	}
}

void add_client_thing(struct mapdef* map, const int id) {
  
}
