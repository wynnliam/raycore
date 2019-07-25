// Liam Wynn, 6/24/2019, Spaceman Engine Level Loading

#ifndef TEXTURE_LIST
#define TEXTURE_LIST

struct recipe_list;
struct recipe;

struct texlist_data {
	// These two values identify a texlist_data.
	// note that either of these (or both) can be NULL.
	char* tex_0;
	char* tex_1;

	// Tells us if we should interpret this texture data as
	// a floor/ceil pair or just a wall. While two-texture walls
	// have no meaning, a floor/ceil pair can have zero, one, or
	// two textures. 0 means floor/ceiling pair, and not 0 means wall.

	// Quick note: this flag tells us *how* to interpret the two textures.
	// if a floor/ceil pair, then tex_0 is the floor texture, and tex_1 is
	// the ceil. In a wall, tex_1 has no meaning, and tex_0 is the wall texture.
	int is_floor_ceil_pair;

	// Used in the mapdef to identify the tile texture in the mapdef.
	// The rule is that floor/ceiling textures are a number from 0 to 99 (inclusive)
	// and the wall textures are from 100 to 199. This nomenclature is an artifact
	// from early mapdef development, and is the way we differentiate walls from floor/ceils
	// while using the layout int array.

	// This value is set when we insert into a texture_list.
	unsigned int mapdef_id;
};

struct texlist_node {
	struct texlist_data* data;
	struct texlist_node* next;
};

struct texture_list {
	struct texlist_node* head;

	// Used to identify walls and floor/ceil textures.
	// For example, when we insert a new wall texture,
	// its id becomes num_walls + 100 (and then num_walls is incremented).
	// For floor/ceil textures the id is just the current value of num_floor_ceils
	// (which too is also incremented after inserting a floor/ceil texture).
	unsigned int num_walls;
	unsigned int num_floor_ceils;
};

struct texlist_data* construct_texlist_data(const char* tex_0, const char* tex_1, int is_floor_ceil_pair);
int texlist_data_equals(struct texlist_data* a, struct texlist_data* b);
void clean_textlist_data(struct texlist_data* to_clean);
void print_texlist_data(struct texlist_data* data);

struct texlist_node* construct_texlist_node(struct texlist_data* data);

struct texture_list* construct_texture_list();
int insert_texture_into_texture_list(struct texture_list* list, struct texlist_data* data);
int texture_list_contains(struct texture_list* list, struct texlist_data* to_find);
unsigned int find_mapdef_id(struct texlist_data* find_id_for, struct texture_list* list);
int build_texture_list_from_map_tree(struct recipe_list* map_tree, struct texture_list* result);
struct texlist_data* build_texlist_data_from_recipe(struct recipe* component_recipe);
void clear_texture_list(struct texture_list* list);
void print_texture_list(struct texture_list* list);

#endif

