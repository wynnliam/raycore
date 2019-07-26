// Liam Wynn, 6/7/2019, Spaceman Engine Level Loading

/*
	Recipes have a list of attributes. However, they also
	have a list of recipes as well. For example, a trigger entity
	(a portion of that map that triggers an event when the player walks
	in it) would have a list of components under it that describe what
	parts of the world activate that trigger.
*/

#ifndef MAP_LOAD_RECIPE
#define MAP_LOAD_RECIPE

#include "./attribute.h"

struct recipe;

struct recipe_list_node {
	struct recipe* recipe;
	struct recipe_list_node* next;
};

struct recipe_list {
	struct recipe_list_node* head;
};

struct recipe {
	char* type;
	struct attribute_list* attributes;
	struct recipe_list* subrecipes;

	int id;
};

// RECIPE FUNCTIONS
struct recipe* construct_recipe(const int id, const char* type);
int insert_attribute_into_recipe(struct attribute* attr, struct recipe* recipe);
int insert_subrecipe(struct recipe* sub, struct recipe* recipe);
int clean_recipe(struct recipe* recipe);
int print_recipe(const struct recipe* recipe, unsigned int num_spaces);

// RECIPE LIST FUNCTIONS
struct recipe_list* construct_recipe_list();
int insert_recipe_into_recipe_list(struct recipe* recipe, struct recipe_list* list);
int clean_recipe_list(struct recipe_list* list);
int print_recipe_list(struct recipe_list* list, unsigned int num_spaces);

#endif
