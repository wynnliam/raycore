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

// Child Entity CRUD
int insert_child_entity(struct entity* insert_into, struct entity* child) {
	if(!insert_into || !child)
		return 0;

	int result = 0;

	// Find the first empty slot to put the child and place. If no spot is found, return 0.
	unsigned int i;
	for(i = 0; i < CHILD_ENTITY_COUNT; i++) {
		if(!insert_into->child_entities[i]) {
			insert_into->child_entities[i] = child;
			insert_into->num_child_entity += 1;
			result = 1;
			break;
		}
	}

	return result;
}

int remove_child_entity(struct entity* remove_from, int child_id) {
	if(!remove_from)
		return 0;

	int result = 0;

	unsigned int i;
	for(i = 0; i < CHILD_ENTITY_COUNT; i++) {
		if(remove_from->child_entities[i] && remove_from->child_entities[i]->id == child_id) {
			// Don't actually remove them, since the map is responsible for the
			// child entity itself.
			remove_from->child_entities[i] = NULL;
			remove_from->num_child_entity -= 1;

			result = 1;
			break;
		}
	}

	return result;
}

struct entity* find_child_entity(struct entity* find_in, int child_id) {
	if(!find_in)
		return NULL;

	struct entity* result = NULL;

	unsigned int i;
	for(i = 0; i < CHILD_ENTITY_COUNT; i++) {
		if(find_in->child_entities[i] && find_in->child_entities[i]->id == child_id) {
			result = find_in->child_entities[i];
			break;
		}
	}

	return result;
}

void clear_child_entities(struct entity* to_clear) {
	if(!to_clear)
		return;

	unsigned int i;
	for(i = 0; i < CHILD_ENTITY_COUNT; i++)
		to_clear->child_entities[i] = NULL;

	to_clear->num_child_entity = 0;
}

int insert_child_thing(struct entity* insert_into, struct thingdef* child) {
	if(!insert_into || !child)
		return 0;

	int result = 0;

	// Find the first empty slot to put the child and place. If no spot is found, return 0.
	unsigned int i;
	for(i = 0; i < CHILD_THING_COUNT; i++) {
		if(!insert_into->child_entities[i]) {
			insert_into->child_things[i] = child;
			insert_into->num_child_things += 1;
			result = 1;
			break;
		}
	}

	return result;
}

int remove_child_thing(struct entity* remove_from, int child_id) {
	if(!remove_from)
		return 0;

	int result = 0;

	unsigned int i;
	for(i = 0; i < CHILD_THING_COUNT; i++) {
		if(remove_from->child_things[i] && remove_from->child_things[i]->id == child_id) {
			// Don't actually remove them, since the map is responsible for the
			// child thing itself.
			remove_from->child_things[i] = NULL;
			remove_from->num_child_things -= 1;

			result = 1;
			break;
		}
	}

	return result;
}

struct thingdef* find_child_thing(struct entity* find_in, int child_id) {
	if(!find_in)
		return NULL;

	struct thingdef* result = NULL;

	unsigned int i;
	for(i = 0; i < CHILD_THING_COUNT; i++) {
		if(find_in->child_things[i] && find_in->child_things[i]->id == child_id) {
			result = find_in->child_things[i];
			break;
		}
	}

	return result;
}

void clear_child_things(struct entity* to_clear) {
	if(!to_clear)
		return;

	unsigned int i;
	for(i = 0; i < CHILD_THING_COUNT; i++)
		to_clear->child_things[i] = NULL;

	to_clear->num_child_things = 0;
}
