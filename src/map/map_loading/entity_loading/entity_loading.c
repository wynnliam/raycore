// Liam Wynn, 8/1/2019, Raycore

#include "./entity_loading.h"
#include "../parser/recipe.h"

#include "../../entity/entity.h"
#include "../../entity/example_entity.h"
#include "../../entity/level_transition_entity.h"

#include <string.h>
#include <stdlib.h>

static struct entity* construct_level_transition_entity_from_recipe(struct recipe* recipe) {
	struct entity* result = construct_entity_level_transition();
	struct level_transition_entity* data = (struct level_transition_entity*)result->data;

	char* goto_level_attr = get_attribute_value(recipe->attributes, "goto_level");

	if(goto_level_attr) {
		data->goto_level_id = atoi(goto_level_attr);
		free(goto_level_attr);
	}

	return result;
}

struct entity* construct_entity_from_recipe(struct recipe* recipe) {
	if(!recipe)
		return NULL;

	struct entity* result = NULL;
	char* type_attr = get_attribute_value(recipe->attributes, "type");
	char* x_attr = get_attribute_value(recipe->attributes, "x");
	char* y_attr = get_attribute_value(recipe->attributes, "y");
	char* id_attr = get_attribute_value(recipe->attributes, "id");

	if(!type_attr)
		return NULL;

	if(strcmp(type_attr, "entity_example") == 0) {
		result = construct_entity_example();
	} else if(strcmp(type_attr, "entity_level_transition") == 0) {
		result = construct_level_transition_entity_from_recipe(recipe);
	}

	if(x_attr) {
		result->x = atoi(x_attr);
		free(x_attr);
	}

	if(y_attr) {
		result->y = atoi(y_attr);
		free(y_attr);
	}

	if(id_attr) {
		result->id = atoi(id_attr);
		free(id_attr);
	}

	free(type_attr);

	return result;
}

