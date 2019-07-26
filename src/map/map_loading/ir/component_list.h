// Liam Wynn, 7/24/2019, Spaceman Engine Level Loading

#ifndef COMPONENT_LIST
#define COMPONENT_LIST

struct recipe;
struct recipe_list;
struct texture_list;

// A rectangular group of tiles
struct component {
	// Used to denote the tile texture we have.
	// If no texture, we assume it is texture 0.
	unsigned int tex_id;

	unsigned int x, y, w, h;
};

struct component_list_node {
	struct component* data;
	struct component_list_node* next;
};

struct component_list {
	struct component_list_node* head;
};

struct component* construct_component(unsigned int tex_id, unsigned int x, unsigned int y,
									  unsigned int w, unsigned int h);
struct component* construct_component_from_recipe_and_texture_list(struct recipe* recipe, struct texture_list* texture_list);
void clean_component(struct component* to_clean);
void print_component(struct component* to_print);

struct component_list* construct_component_list();
struct component_list* build_component_list_from_recipe_list_and_texture_list(
						struct recipe_list* map_tree, struct texture_list* texture_list);
void insert_component_into_list(struct component_list* list, struct component* component);
void clean_component_list(struct component_list* list);
void print_component_list(struct component_list* list);
#endif

