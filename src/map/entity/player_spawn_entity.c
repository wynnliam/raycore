// Liam Wynn, 8/6/2019, Raycore

#include "./player_spawn_entity.h"

#include "./entity.h"
#include "../map.h"

#include <stdlib.h>

struct entity* construct_entity_player_spawn() {
	struct player_spawn_entity* data = (struct player_spawn_entity*)
												malloc(sizeof(struct player_spawn_entity));
	data->player_x = 0;
	data->player_y = 0;
	data->player_rot = 0;

	struct entity* result = construct_empty_entity();

	result->type = ENTITY_TYPE_PLAYER_SPAWN;
	result->id = 0;
	result->data = (void*)data;

	result->x = 0;
	result->y = 0;

	result->initialize = &entity_player_spawn_initialize;
	result->update = &entity_player_spawn_update;
	result->clean = &entity_player_spawn_clean;

	return result;
}

void entity_player_spawn_initialize(struct entity* entity, struct mapdef* curr_map) {
}

void entity_player_spawn_update(struct entity* entity, struct mapdef* curr_map) {
}

void entity_player_spawn_clean(struct entity* entity, struct mapdef* curr_map) {
}
