// Liam Wynn, 7/15/2019, Raycore

#include "./entity.h"
#include "../thing.h"

#include <stdlib.h>

static int initialize_entity(struct entity* to_initialize);

struct entity* construct_empty_entity() {
	struct entity* result = (struct entity*)malloc(sizeof(struct entity));
	initialize_entity(result);

	return result;
}

static int initialize_entity(struct entity* to_initialize) {
	if(!to_initialize)
		return 0;

	to_initialize->data = NULL;

	unsigned int i;

	for(i = 0; i < CHILD_ENTITY_COUNT; i++)
		to_initialize->child_entities[i] = NULL;
	for(i = 0; i < CHILD_THING_COUNT; i++)
		to_initialize->child_things[i] = NULL;

	to_initialize->type = ENTITY_TYPE_NONE;
	to_initialize->id = -1;

	to_initialize->x = 0;
	to_initialize->y = 0;

	to_initialize->num_child_entity = 0;
	to_initialize->num_child_tiles = 0;
	to_initialize->num_child_things = 0;

	return 1;
}
