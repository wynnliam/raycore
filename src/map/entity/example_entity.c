// Liam Wynn, 7/15/2019, Raycore

#include "./example_entity.h"

#include "./entity.h"
#include "../map.h"

#include <stdlib.h>
#include <stdio.h>

struct entity* construct_entity_example() {
	struct entity_example* data = (struct entity_example*)malloc(sizeof(struct entity_example));
	data->counter = 0;

	struct entity* result = (struct entity*)malloc(sizeof(struct entity));

	result->type = ENTITY_TYPE_EXAMPLE;
	result->id = 0;
	result->data = (void*)data;

	result->x = 0;
	result->y = 0;

	result->initialize = &entity_example_initialize;
	result->update = &entity_example_update;
	result->clean = &entity_example_clean;

	return result;
}

void entity_example_initialize(struct entity* entity, struct mapdef* curr_map) {
	printf("Entity Example: I am initialized!\n");
}

void entity_example_update(struct entity* entity, struct mapdef* curr_map) {
	if(!entity || !entity->data)
		return;

	struct entity_example* data = (struct entity_example*)entity->data;
	data->counter += 1;
}

void entity_example_clean(struct entity* entity, struct mapdef* curr_map) {
	if(!entity || !entity->data)
		return;

	struct entity_example* data = (struct entity_example*)entity->data;
	printf("Counter at: %d\n", data->counter);
}

