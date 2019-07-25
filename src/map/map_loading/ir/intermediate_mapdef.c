// Liam Wynn, 7/24/2019, Spaceman Engine Level Loading

#include "intermediate_mapdef.h"

#include <stdlib.h>
#include <stdio.h>

struct intermediate_mapdef* construct_intermediate_mapdef(struct recipe_list* map_tree) {
	if(!map_tree)
		return NULL;

	struct intermediate_mapdef* result = (struct intermediate_mapdef*)malloc(sizeof(struct intermediate_mapdef));

	result->textures = construct_texture_list();
	build_texture_list_from_map_tree(map_tree, result->textures);

	result->things = build_thing_list_from_map_tree(map_tree);

	result->components = build_component_list_from_recipe_list_and_texture_list(map_tree, result->textures);

	return result;
}

void clean_intermediate_mapdef(struct intermediate_mapdef* to_clean) {
	if(!to_clean)
		return;

	if(to_clean->things)
		clean_thing_list(to_clean->things);

	if(to_clean->components)
		clean_component_list(to_clean->components);

	if(to_clean->textures) {
		clear_texture_list(to_clean->textures);
		free(to_clean->textures);
	}

	free(to_clean);
}

void print_intermediate_mapdef(struct intermediate_mapdef* to_print) {
	if(!to_print)
		return;

	print_texture_list(to_print->textures);
	printf("\n");
	print_thing_list(to_print->things);
	printf("\n");
	print_component_list(to_print->components);
}
