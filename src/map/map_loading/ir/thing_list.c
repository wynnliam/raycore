// Liam Wynn, 7/18/2019, Spaceman Engine Level Loading

#include "./thing_list.h"
#include "../parser/recipe.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* THING LIST DATA IMPLEMENTATIONS */
struct thinglist_data* construct_thinglist_data(const char* sprite_sheet, int anim_class, int x, int y, int rot) {
	struct thinglist_data* result = (struct thinglist_data*)malloc(sizeof(struct thinglist_data));

	if(!sprite_sheet)
		result->sprite_sheet = NULL;
	else {
		result->sprite_sheet = (char*)malloc(strlen(sprite_sheet) + 1);
		strcpy(result->sprite_sheet, sprite_sheet);
	}

	result->anim_class = anim_class;
	result->x = x;
	result->y = y;
	result->rot = rot;

	return result;
}

int thinglist_data_equals(struct thinglist_data* a, struct thinglist_data* b) {
	int result;
	int sprite_sheet_cmp;

	if(!a || !b) {
		result = a == b;
	} else {
		if(a->sprite_sheet && b->sprite_sheet)
			sprite_sheet_cmp = strcmp(a->sprite_sheet, b->sprite_sheet) == 0;
		else
			sprite_sheet_cmp = a->sprite_sheet == b->sprite_sheet;

		result = sprite_sheet_cmp &&
				 a->x == b->x && a->y == b->y &&
				 a->rot == b->rot &&
				 a->anim_class == b->anim_class;
	}

	return result;
}

void clean_thinglist_data(struct thinglist_data* to_clean) {
	if(!to_clean)
		return;

	if(to_clean->sprite_sheet)
		free(to_clean->sprite_sheet);

	free(to_clean);
}

void print_thinglist_data(struct thinglist_data* to_print) {
	if(!to_print)
		return;

	if(to_print->sprite_sheet)
		printf("sprite sheed: %s\n", to_print->sprite_sheet);

	printf("anim class: %d\n", to_print->anim_class);
	printf("position: %d %d\n", to_print->x, to_print->y);
	printf("rotation: %d\n", to_print->rot);
}

struct thinglist_data* thinglist_data_from_recipe(struct recipe* recipe) {
	if(!recipe || !recipe->attributes)
		return NULL;

	// The raw attribute values
	char* sprite_sheet_attr = get_attribute_value(recipe->attributes, "sprite_sheet");
	char* anim_class_attr = get_attribute_value(recipe->attributes, "anim_class");
	char* x_attr = get_attribute_value(recipe->attributes, "x");
	char* y_attr = get_attribute_value(recipe->attributes, "y");
	char* rot_attr = get_attribute_value(recipe->attributes, "rot");

	// The thinglist data values
	char* result_sprite_sheet = NULL;
	int result_anim_class = 0;
	int result_x = 0;
	int result_y = 0;
	int result_rot = 0;
	struct thinglist_data* result;

	// If the sprite_sheet_attr exists and does not equal "0" (denotes None in level file)
	if(sprite_sheet_attr && strcmp(sprite_sheet_attr, "0") != 0)
		result_sprite_sheet = sprite_sheet_attr;
	if(anim_class_attr)
		result_anim_class = atoi(anim_class_attr);
	if(x_attr)
		result_x = atoi(x_attr);
	if(y_attr)
		result_y = atoi(y_attr);
	if(rot_attr)
		result_rot = atoi(rot_attr);
	
	result = construct_thinglist_data(result_sprite_sheet, result_anim_class, result_x, result_y, result_rot);

	if(sprite_sheet_attr) free(sprite_sheet_attr);
	if(anim_class_attr) free(anim_class_attr);
	if(x_attr) free(x_attr);
	if(y_attr) free(y_attr);
	if(rot_attr) free(rot_attr);

	return result;
}

/* THING LIST IMPLEMENTATIONS */
struct thing_list* construct_thing_list() {
	struct thing_list* result = (struct thing_list*)malloc(sizeof(struct thing_list));

	result->head = NULL;

	return result;
}

int insert_data_into_thing_list(struct thing_list* list, struct thinglist_data* data) {
	if(!list || !data)
		return 0;

	struct thinglist_node* temp = (struct thinglist_node*)malloc(sizeof(struct thinglist_node*));

	temp->data = data;
	temp->next = list->head;
	list->head = temp;

	return 1;
}

int thing_list_contains(struct thing_list* list, struct thinglist_data* to_find) {
	if(!list)
		return 0;

	int result = 0;
	struct thinglist_node* curr = list->head;

	while(curr) {
		if(thinglist_data_equals(curr->data, to_find)) {
			result = 1;
			break;
		} else
			curr = curr->next;
	}

	return result;
}

void clean_thing_list(struct thing_list* to_clean) {
	if(!to_clean)
		return;

	struct thinglist_node* curr;
	struct thinglist_node* next;

	curr = to_clean->head;
	next = NULL;

	while(curr) {
		next = curr->next;

		clean_thinglist_data(curr->data);
		free(curr);

		curr = next;
	}

	free(to_clean);
}

void print_thing_list(struct thing_list* list) {
	if(!list)
		return;

	struct thinglist_node* curr = list->head;

	while(curr) {
		print_thinglist_data(curr->data);
		curr = curr->next;
	}
}

static int build_thing_list_from_recipe_list_node(struct recipe_list_node* node, struct thing_list* result) {
	if(!node || !node->recipe || !result)
		return 0;

	struct recipe* recipe = node->recipe;
	struct recipe_list_node* next_recipe = node->next;
	struct recipe_list_node* subrecipe_node = recipe->subrecipes->head;

	if(strcmp(recipe->type, "thing") == 0) {
		struct thinglist_data* data = thinglist_data_from_recipe(recipe);

		if(!thing_list_contains(result, data))
			insert_data_into_thing_list(result, data);
		else
			clean_thinglist_data(data);
	}

	return 1 + build_thing_list_from_recipe_list_node(subrecipe_node, result) +
			   build_thing_list_from_recipe_list_node(next_recipe, result);
}

struct thing_list* build_thing_list_from_map_tree(struct recipe_list* map_tree) {
	if(!map_tree)
		return NULL;

	struct thing_list* result = construct_thing_list();
	build_thing_list_from_recipe_list_node(map_tree->head, result);

	return result;
}
