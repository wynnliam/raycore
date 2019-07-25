// Liam Wynn, 7/24/2019, Spaceman Engine Level Loading

/*
	Stores a texture_list, thing_list, and the tree representation
	of a map. This way, we can have everything we need to construct
	a mapdef in one place.
*/

#ifndef INTERMEDIATE_MAPDEF
#define INTERMEDIATE_MAPDEF

#include "../parser/parser.h"
#include "./texture_list.h"
#include "./thing_list.h"
#include "./component_list.h"

struct intermediate_mapdef {
	// Special lists to hold all textures and things
	struct texture_list* textures;
	struct thing_list* things;
	struct component_list* components;
};

struct intermediate_mapdef* construct_intermediate_mapdef(struct recipe_list* map_tree);
void clean_intermediate_mapdef(struct intermediate_mapdef* to_clean);
void print_intermediate_mapdef(struct intermediate_mapdef* to_print);

#endif
