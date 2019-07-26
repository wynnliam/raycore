// Liam Wynn, 6/7/2019, Spaceman Engine Level Loading

#include "./recipe.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct recipe* construct_recipe(const int id, const char* type) {
	if(!type)
		return NULL;

	struct recipe* result = (struct recipe*)malloc(sizeof(struct recipe));

	result->id = id;

	result->type = (char*)malloc(strlen(type) + 1);
	strcpy(result->type, type);

	result->attributes = construct_attribute_list();
	result->subrecipes = construct_recipe_list();

	return result;
}

int insert_attribute_into_recipe(struct attribute* attr, struct recipe* recipe) {
	if(!attr || !recipe)
		return 0;

	return insert_attribute_into_list(attr, recipe->attributes);
}

int insert_subrecipe(struct recipe* sub, struct recipe* recipe) {
	if(!sub || !recipe)
		return 0;

	insert_recipe_into_recipe_list(sub, recipe->subrecipes);

	return 1;
}

int clean_recipe(struct recipe* recipe) {
	if(!recipe)
		return 0;

	if(recipe->type)
		free(recipe->type);

	clean_attribute_list(recipe->attributes);
	clean_recipe_list(recipe->subrecipes);

	free(recipe);

	return 1;
}

int print_recipe(const struct recipe* recipe, unsigned int num_spaces) {
	if(!recipe || !recipe->type)
		return 0;

	printf("%*s", num_spaces, "");
	printf("Recipe: %d\n", recipe->id);

	printf("%*s", num_spaces, "");
	printf("type: %s\n", recipe->type);

	print_attribute_list(recipe->attributes, num_spaces);

	putchar('\n');
	// + 1 means that the recipes we print are children of this recipe.
	print_recipe_list(recipe->subrecipes, num_spaces + 1);

	return 1;
}

/* RECIPE LIST FUNCTION IMPLEMENTATIONS */
struct recipe_list* construct_recipe_list() {
	struct recipe_list* result = (struct recipe_list*)malloc(sizeof(struct recipe_list));

	result->head = NULL;

	return result;
}

int insert_recipe_into_recipe_list(struct recipe* recipe, struct recipe_list* list) {
	if(!recipe || !list)
		return 0;

	struct recipe_list_node* temp = (struct recipe_list_node*)malloc(sizeof(struct recipe_list_node));
	temp->recipe = recipe;
	temp->next = list->head;

	list->head = temp;

	return 1;
}

int clean_recipe_list(struct recipe_list* list) {
	if(!list)
		return 0;

	struct recipe_list_node* curr = list->head;
	struct recipe_list_node* temp = NULL;

	while(curr) {
		temp = curr->next;

		clean_recipe(curr->recipe);
		free(curr);
		curr = temp;
	}

	free(list);

	return 1;
}

int print_recipe_list(struct recipe_list* list, unsigned int num_spaces) {
	if(!list)
		return 0;

	struct recipe_list_node* curr = list->head;
	while(curr) {
		print_recipe(curr->recipe, num_spaces);
		curr = curr->next;
	}

	return 1;
}
