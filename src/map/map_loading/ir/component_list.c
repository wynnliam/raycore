// Liam Wynn, 7/24/2019, Spaceman Engine Level Loading

#include "./component_list.h"
#include "../parser/recipe.h"
#include "./texture_list.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int build_component_list_from_recipe_list_nodes_and_texture_list(struct recipe_list_node* head, struct texture_list* texlist, struct component_list* result);

/* COMPONENT IMPLEMENTATIONS */
struct component* construct_component(unsigned int tex_id, unsigned int x, unsigned int y,
									  unsigned int w, unsigned int h) {
	struct component* result = (struct component*)malloc(sizeof(struct component));

	result->tex_id = tex_id;
	result->x = x;
	result->y = y;
	result->w = w;
	result->h = h;

	return result;
}

struct component* construct_component_from_recipe_and_texture_list(struct recipe* recipe, struct texture_list* texture_list) {
	if(!recipe || !texture_list)
		return NULL;
	
	unsigned int tex_id;
	unsigned int x, y, w, h;

	char* x_attr = get_attribute_value(recipe->attributes, "x");
	char* y_attr = get_attribute_value(recipe->attributes, "y");
	char* w_attr = get_attribute_value(recipe->attributes, "w");
	char* h_attr = get_attribute_value(recipe->attributes, "h");

	struct texlist_data* component_texture_data = build_texlist_data_from_recipe(recipe);
	tex_id = find_mapdef_id(component_texture_data, texture_list);
	if(component_texture_data)
		clean_textlist_data(component_texture_data);

	if(x_attr) {
		x = (unsigned int)atoi(x_attr);
		free(x_attr);
	} else
		x = 0;

	if(y_attr) {
		y = (unsigned int)atoi(y_attr);
		free(y_attr);
	} else
		y = 0; 

	if(w_attr) {
		w = (unsigned int)atoi(w_attr);
		free(w_attr);
	} else
		w = 1;

	if(h_attr) {
		h = (unsigned int)atoi(h_attr);
		free(h_attr);
	} else
		h = 1;

	return construct_component(tex_id, x, y, w, h);
}

void clean_component(struct component* to_clean) {
	if(!to_clean)
		return;

	free(to_clean);
}

void print_component(struct component* to_print) {
	if(!to_print)
		return;

	printf("Texture ID: %u\n", to_print->tex_id);
	printf("Dimensions: { x: %u, y: %u, w: %u, h: %u }\n\n",
			to_print->x, to_print->y, to_print->w, to_print->h);
}

/* COMPONENT LIST IMPLEMENTATIONS */
struct component_list* construct_component_list() {
	struct component_list* result = (struct component_list*)malloc(sizeof(struct component_list));

	result->head = NULL;

	return result;
}

struct component_list* build_component_list_from_recipe_list_and_texture_list(
						struct recipe_list* map_tree, struct texture_list* texture_list) {
	if(!map_tree || !texture_list)
		return NULL;

	struct component_list* result = construct_component_list();

	build_component_list_from_recipe_list_nodes_and_texture_list(map_tree->head, texture_list, result);

	return result;
}

static int build_component_list_from_recipe_list_nodes_and_texture_list(struct recipe_list_node* head,
																		struct texture_list* texlist,
																		struct component_list* result) {
	if(!head || !texlist || !result)
		return 0;

	if(head->recipe && strcmp(head->recipe->type, "component") == 0) {
		struct component* component = construct_component_from_recipe_and_texture_list(head->recipe, texlist);
		insert_component_into_list(result, component);
	}

	return 1 + build_component_list_from_recipe_list_nodes_and_texture_list(head->recipe->subrecipes->head, texlist, result) +
			   build_component_list_from_recipe_list_nodes_and_texture_list(head->next, texlist, result);
}

void insert_component_into_list(struct component_list* list, struct component* component) {
	if(!list || !component)
		return;

	struct component_list_node* temp = (struct component_list_node*)malloc(sizeof(struct component_list_node));
	temp->data = component;
	temp->next = list->head;
	list->head = temp;
}

void clean_component_list(struct component_list* list) {
	if(!list)
		return;

	struct component_list_node* curr;
	struct component_list_node* temp;

	curr = list->head;
	temp = NULL;

	while(curr) {
		temp = curr->next;

		clean_component(curr->data);
		free(curr);

		curr = temp;
	}

	free(list);
}

void print_component_list(struct component_list* list) {
	if(!list)
		return;

	struct component_list_node* curr = list->head;

	while(curr) {
		print_component(curr->data);
		curr = curr->next;
	}
}
