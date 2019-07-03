// Liam Wynn, 7/23/2018, Raycaster: Map File Loader

#include "map_data.h"

#include <stdlib.h>

static int clear_components(struct component** head);
static int clear_thing_data(struct thing_data** head);

int add_component(struct map_data* add_to) {
	if(!add_to)
		return 0;

	struct component* to_add = (struct component*)malloc(sizeof(struct component));

	to_add->x = 0;
	to_add->y = 0;
	to_add->w = 0;
	to_add->h = 0;

	to_add->is_floor_ceil = 0;
	to_add->invisible_wall = 0;

	to_add->tex_0 = NULL;
	to_add->tex_1 = NULL;

	to_add->next = add_to->component_head;
	add_to->component_head = to_add;

	return 1;
}

int add_thing_data(struct map_data* add_to) {
	if(!add_to)
		return 0;

	struct thing_data* to_add = (struct thing_data*)malloc(sizeof(struct thing_data));

	to_add->x = 0;
	to_add->y = 0;
	to_add->type = 0;

	to_add->next = add_to->thing_head;
	add_to->thing_head = to_add;

	return 1;
}

int clear_map_data(struct map_data* to_clear) {
	if(!to_clear)
		return 0;

	if(to_clear->name) {
		free(to_clear->name);
		to_clear->name = NULL;
	}

	if(to_clear->sky_tex) {
		free(to_clear->sky_tex);
		to_clear->sky_tex = NULL;
	}

	clear_components(&(to_clear->component_head));
	clear_thing_data(&(to_clear->thing_head));
	return 1;
}

static int clear_components(struct component** head) {
	if(!*head)
		return 0;

	struct component* next = (*head)->next;

	if((*head)->tex_0) {
		free((*head)->tex_0);
		(*head)->tex_0 = NULL;
	}

	if((*head)->tex_1) {
		free((*head)->tex_1);
		(*head)->tex_1 = NULL;
	}

	free(*head);
	*head = next;

	return clear_components(head);
}

static int clear_thing_data(struct thing_data** head) {
	if(!*head)
		return 0;

	struct thing_data* next = (*head)->next;

	if((*head)->sprite_sheet) {
		free((*head)->sprite_sheet);
		(*head)->sprite_sheet = NULL;
	}

	free(*head);
	*head = next;

	return clear_thing_data(head);
}
