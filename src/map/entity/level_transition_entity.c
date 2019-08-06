// Liam Wynn, 8/6/2019, Raycore

#include "./level_transition_entity.h"

#include "./entity.h"
#include "../map.h"

#include <stdlib.h>

struct entity* construct_entity_level_transition() {
	struct level_transition_entity* data = (struct level_transition_entity*)
												malloc(sizeof(struct level_transition_entity));
	data->goto_level_id = 0;

	struct entity* result = construct_empty_entity();

	result->type = ENTITY_TYPE_LEVEL_TRANSITION;
	result->id = 0;
	result->data = (void*)data;

	result->x = 0;
	result->y = 0;

	result->initialize = &entity_level_transition_initialize;
	result->update = &entity_level_transition_update;
	result->clean = &entity_level_transition_clean;

	return result;
}

void entity_level_transition_initialize(struct entity* entity, struct mapdef* curr_map) {
}

void entity_level_transition_update(struct entity* entity, struct mapdef* curr_map) {
	if(!entity || !curr_map)
		return;

	int i;
	for(i = 0; i < CHILD_THING_COUNT; i++) {
		if(entity->child_things[i] && check_signal_user_interact(entity->child_things[i])) {
			printf("Activated child %u\n", entity->child_things[i]->id);
		}
	}
}

void entity_level_transition_clean(struct entity* entity, struct mapdef* curr_map) {
	if(!entity)
		return;

	if(entity->data)
		free(entity->data);
	free(entity->data);
}
