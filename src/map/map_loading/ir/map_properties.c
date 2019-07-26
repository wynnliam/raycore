// Liam Wynn, 7/26/2019, Raycore

#include "./map_properties.h"
#include "../parser/recipe.h"

#include <stdlib.h>
#include <string.h>

struct ir_map_properties* construct_properties() {
	struct ir_map_properties* result = (struct ir_map_properties*)malloc(sizeof(struct ir_map_properties));

	result->mapdef_name = NULL;

	result->sky_tex = NULL;

	result->use_fog = 0;
	result->fog_r = 0;
	result->fog_g = 0;
	result->fog_b = 0;

	return result;
}

struct ir_map_properties* build_ir_map_properties_from_recipe(struct recipe* recipe) {
	struct ir_map_properties* result = construct_properties();

	if(!recipe || !recipe->attributes)
		return result;

	char* map_name_attr = get_attribute_value(recipe->attributes, "name");

	char* sky_tex_attr = get_attribute_value(recipe->attributes, "sky_tex");

	char* use_fog_attr = get_attribute_value(recipe->attributes, "use_fog");
	char* fog_r_attr = get_attribute_value(recipe->attributes, "fog_r");
	char* fog_g_attr = get_attribute_value(recipe->attributes, "fog_g");
	char* fog_b_attr = get_attribute_value(recipe->attributes, "fog_b");

	if(map_name_attr) {
		result->mapdef_name = (char*)malloc(strlen(map_name_attr) + 1);
		strcpy(result->mapdef_name, map_name_attr);
		free(map_name_attr);
	}

	if(sky_tex_attr) {
		result->sky_tex = (char*)malloc(strlen(sky_tex_attr) + 1);
		strcpy(result->sky_tex, sky_tex_attr);
		free(sky_tex_attr);
	}

	if(use_fog_attr) {
		result->use_fog = atoi(use_fog_attr);
		free(use_fog_attr);
	}

	if(fog_r_attr) {
		result->fog_r = atoi(fog_r_attr);
		free(fog_r_attr);
	}

	if(fog_g_attr) {
		result->fog_g = atoi(fog_g_attr);
		free(fog_g_attr);
	}

	if(fog_b_attr) {
		result->fog_b = atoi(fog_b_attr);
		free(fog_b_attr);
	}

	return result;
}

static struct ir_map_properties* find_properties_recipe(struct recipe_list_node* head) {
	if(!head)
		return construct_properties();

	if(head->recipe && strcmp(head->recipe->type, "properties") == 0)
		return build_ir_map_properties_from_recipe(head->recipe);

	return find_properties_recipe(head->next);
}

struct ir_map_properties* build_ir_map_properties_from_map_tree(struct recipe_list* map_tree) {
	if(!map_tree)
		return construct_properties();

	return find_properties_recipe(map_tree->head);
}

void clean_ir_map_properties(struct ir_map_properties* to_clean) {
	if(!to_clean)
		return;

	if(to_clean->mapdef_name)
		free(to_clean->mapdef_name);
	if(to_clean->sky_tex)
		free(to_clean->sky_tex);

	free(to_clean);
}

