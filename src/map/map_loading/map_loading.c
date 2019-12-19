// Liam Wynn, 4/17/2019, Raycore

#include "map_loading.h"
#include "../map.h"

#include "./tokenizer/tokenizer.h"
#include "./parser/recipe.h"
#include "./ir/intermediate_mapdef.h"
#include "./entity_loading/entity_loading.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: All these static functions should probably go into a seperate file.
static void compute_map_properties(struct ir_map_properties* ir_properties, struct mapdef* result);

static void compute_map_dimensions(struct component_list* components, unsigned int* map_w, unsigned int* map_h);
static void compute_map_layout(struct component_list* components, struct mapdef* result);

static void create_wall_textures(struct texture_list* textures, struct mapdef* result);
static void create_floor_ceil_textures(struct texture_list* textures, struct mapdef* result);

static void add_thingdefs_to_map(struct thing_list* things, struct mapdef* result);

static void add_entities_to_map(struct recipe_list_node* head, struct entity* parent, struct mapdef* result);

// And array of every level we want in the game.
char** map_lookup_table;
unsigned int map_lookup_table_size = 0;

int initialize_map_lookup_table() {
	FILE* load_conf = fopen("./assests/maps/load.conf", "r");
	// For reading values into
	char buffer[1000];

	if(!load_conf)
		return 0;
	if(map_lookup_table_size > 0)
		return 0;

	// Read table size:
	fscanf(load_conf, "%d", &map_lookup_table_size);
	printf("Num maps: %d\n", map_lookup_table_size);

	map_lookup_table = (char**)malloc(sizeof(char**) * map_lookup_table_size);

	// Load levels
	unsigned int i;
	for(i = 0; i < map_lookup_table_size; ++i) {
		fscanf(load_conf, "%s", buffer);

		map_lookup_table[i] = (char*)malloc(strlen(buffer) + 1);
		strcpy(map_lookup_table[i], buffer);

		printf("%d: %s\n", i, map_lookup_table[i]);
	}

	fclose(load_conf);

	return 0;
}

char* do_map_lookup(unsigned int index) {
	if(map_lookup_table_size == 0 || index >= map_lookup_table_size)
		return NULL;

	return map_lookup_table[index];
}

unsigned int get_num_loaded_maps() {
	return map_lookup_table_size;
}

struct mapdef* load_map_from_file(const char* path) {
	if(!path)
		return NULL;

	struct token_list* tokens = get_tokens(path);
	struct recipe_list* map_tree = build_map_tree(tokens);
	struct intermediate_mapdef* intermediate_mapdef = construct_intermediate_mapdef(map_tree);

	unsigned int map_w, map_h;

	// TODO: Move this to seperate functions/files
	struct mapdef* result = (struct mapdef*)malloc(sizeof(struct mapdef));
	initialize_map(result);

	compute_map_properties(intermediate_mapdef->properties, result);

	// TODO: Make seperate function for these lines (set_map_dimensions)
	compute_map_dimensions(intermediate_mapdef->components, &map_w, &map_h);
	result->map_w = map_w;
	result->map_h = map_h;

	compute_map_layout(intermediate_mapdef->components, result);

	result->num_tiles = 200;
	create_wall_textures(intermediate_mapdef->textures, result);
	create_floor_ceil_textures(intermediate_mapdef->textures, result);

	add_thingdefs_to_map(intermediate_mapdef->things, result);

	add_entities_to_map(map_tree->head, NULL, result);

	printf("Loaded %s\n", path);

	clean_token_list(tokens);
	clean_recipe_list(map_tree);
	clean_intermediate_mapdef(intermediate_mapdef);

	return result;
}

static void compute_map_properties(struct ir_map_properties* ir_properties, struct mapdef* result) {
	if(!ir_properties || !result)
		return;

	if(ir_properties->sky_tex)
		result->sky_surf = SDL_LoadBMP(ir_properties->sky_tex);
	else
		result->sky_surf = NULL;

	result->use_fog = ir_properties->use_fog;
	result->fog_r = ir_properties->fog_r;
	result->fog_g = ir_properties->fog_g;
	result->fog_b = ir_properties->fog_b;
}

static void compute_map_dimensions(struct component_list* components, unsigned int* map_w, unsigned int* map_h) {
	if(!components)
		return;

	// Used to keep track of the minimal and maximal dimensions.
	// By having all of these, we can 
	struct component* min_x_component;
	struct component* min_y_component;
	struct component* max_x_component;
	struct component* max_y_component;

	struct component_list_node* curr = components->head;

	min_x_component = curr->data;
	min_y_component = curr->data;
	max_x_component = curr->data;
	max_y_component = curr->data;

	while(curr) {
		if(curr->data->x < min_x_component->x)
			min_x_component = curr->data;
		if(curr->data->x > max_x_component->x)
			max_x_component = curr->data;
		if(curr->data->y < min_y_component->y)
			min_y_component = curr->data;
		if(curr->data->y > max_y_component->y)
			max_y_component = curr->data;

		curr = curr->next;
	}

	*map_w = (max_x_component->x + max_x_component->w) - min_x_component->x;
	*map_h = (max_y_component->y + max_y_component->h) - min_y_component->y;
}

static void place_single_component(struct component_list_node* head, struct mapdef* result) {
	if(!head || !result)
		return;

	// Recurse to the end of the list. As we unroll our loop, place components so the draw order
	// is correct.
	place_single_component(head->next, result);

	unsigned int x, y;
	// a 2D point converted into 1D so that we can index into our map.
	unsigned int mapdef_index;

	for(x = head->data->x; x < head->data->x + head->data->w; x++) {
		for(y = head->data->y; y < head->data->y + head->data->h; y++) {
			if(x >= result->map_w || y >= result->map_h)
				continue;

			mapdef_index = y * result->map_w + x;

			result->layout[mapdef_index] = head->data->tex_id;
			// TODO: Invisible walls!
		}
	}
}

static void compute_map_layout(struct component_list* components, struct mapdef* result) {
	if(!components || !result)
		return;

	result->layout = (unsigned int*)malloc(sizeof(unsigned int) * result->map_w * result->map_h);
	result->invisible_walls = (int*)malloc(sizeof(int) * result->map_w * result->map_h);

	unsigned int i;
	for(i = 0; i < result->map_w * result->map_h; i++) {
		result->layout[i] = 0;
		result->invisible_walls[i] = 0;
	}

	place_single_component(components->head, result);

}

static void create_wall_textures(struct texture_list* textures, struct mapdef* result) {
	if(!textures || !result)
		return;

	result->num_wall_tex = 100;

	struct texlist_node* curr = textures->head;
	int walldef_index;

	while(curr) {
		if(curr->data->mapdef_id >= 100) {
			walldef_index = curr->data->mapdef_id - 100;

			if(curr->data->tex_0) {
				if(walldef_index == 5)
					printf("Found the problem\n");
				result->walls[walldef_index].path = (char*)malloc(strlen(curr->data->tex_0) + 1);
				strcpy(result->walls[walldef_index].path, curr->data->tex_0);

				printf("loading wall texture %s\n", result->walls[walldef_index].path);
				result->walls[walldef_index].surf = SDL_LoadBMP(result->walls[walldef_index].path);

			} else {
				result->walls[walldef_index].path = NULL;
				result->walls[walldef_index].surf = NULL;
			}
		}

		curr = curr->next;
	}
}

static void create_floor_ceil_textures(struct texture_list* textures, struct mapdef* result) {
	if(!textures || !result)
		return;

	result->num_floor_ceils = 100;

	struct texlist_node* curr = textures->head;
	int floorceildef_index;

	while(curr) {
		if(curr->data->mapdef_id < 100) {
			floorceildef_index = curr->data->mapdef_id;

			if(curr->data->tex_0) {
				result->floor_ceils[floorceildef_index].floor_path = (char*)malloc(strlen(curr->data->tex_0) + 1);
				strcpy(result->floor_ceils[floorceildef_index].floor_path, curr->data->tex_0);

				result->floor_ceils[floorceildef_index].floor_surf = SDL_LoadBMP(result->floor_ceils[floorceildef_index].floor_path);
			} else {
				result->floor_ceils[floorceildef_index].floor_path = NULL;
				result->floor_ceils[floorceildef_index].floor_surf = NULL;
			}

			if(curr->data->tex_1) {
				result->floor_ceils[floorceildef_index].ceil_path = (char*)malloc(strlen(curr->data->tex_1) + 1);
				strcpy(result->floor_ceils[floorceildef_index].ceil_path, curr->data->tex_1);

				result->floor_ceils[floorceildef_index].ceil_surf = SDL_LoadBMP(result->floor_ceils[floorceildef_index].ceil_path);
			} else {
				result->floor_ceils[floorceildef_index].ceil_path = NULL;
				result->floor_ceils[floorceildef_index].ceil_surf = NULL;
			}
		}

		curr = curr->next;
	}
}

static void add_thingdefs_to_map(struct thing_list* things, struct mapdef* result) {
	if(!things || !result)
		return;

	result->num_things = things->count;

	// TODO: Create CRUD interface for things in map
	struct thinglist_node* curr = things->head;
	unsigned int index = 0;

	while(curr) {
		if(curr->data && curr->data->sprite_sheet) {
			create_thingdef(&result->things[index],
							curr->data->sprite_sheet,
							curr->data->anim_class,
							curr->data->x, curr->data->y,
							curr->data->rot);

			result->things[index].id = curr->data->id;

			index++;
		}

		curr = curr->next;
	}
}

// Add child tiles to parent entity. However, we take a recipe, and find the corresponding
// tiles.
static void add_enitty_child_tiles_from_recipe(struct entity* parent, struct recipe* recipe, struct mapdef* map) {
	if(!parent || !recipe || !map)
		return;

	struct component* from_recipe = construct_component_from_recipe_and_texture_list(recipe, NULL);
	if(!from_recipe)
		return;

	unsigned int x, y, tile_index;
	for(x = from_recipe->x; x < from_recipe->w; x++) {
		for(y = from_recipe->y; y < from_recipe->h; y++) {
			if(x >= map->map_w || y >= map->map_h)
				continue;

			tile_index = y * map->map_w + x;
			insert_child_tile(parent, tile_index);
		}
	}

	printf("Child tiles for entity: %u", parent->num_child_tiles);
	clean_component(from_recipe);
}

void add_entity_child_thing_from_recipe(struct entity* parent, struct recipe* recipe, struct mapdef* map) {
	if(!parent || !recipe || !map)
		return;

	struct thinglist_data* temp_data = thinglist_data_from_recipe(recipe);
	if(!temp_data)
		return;

	int id = temp_data->id;
	clean_thinglist_data(temp_data);

	unsigned int i;
	for(i = 0; i < map->num_things; i++) {
		if(map->things[i].id == id) {
			insert_child_thing(parent, &map->things[i]);
			printf("Added thing child!\n");
		}
	}
}

static void add_entities_to_map(struct recipe_list_node* head, struct entity* parent, struct mapdef* result) {
	if(!head || !head->recipe || !result)
		return;

	struct entity* entity = NULL;
	if(strcmp("entity", head->recipe->type) == 0) {
		entity  = construct_entity_from_recipe(head->recipe);

		if(entity) {
			insert_entity_into_map(result, entity);

			if(parent)
				insert_child_entity(parent, entity);
		} else
			printf("Error reading entity! Check entity recipes to ensure they are correct!\n");
	} else if(parent) {
		if(strcmp("component", head->recipe->type) == 0) {
			add_enitty_child_tiles_from_recipe(parent, head->recipe, result);
		} else if(strcmp("thing", head->recipe->type) == 0) {
			add_entity_child_thing_from_recipe(parent, head->recipe, result);
		}
	}

	add_entities_to_map(head->next, parent, result);
	
	if(head->recipe->subrecipes)
		add_entities_to_map(head->recipe->subrecipes->head, entity, result);
}
