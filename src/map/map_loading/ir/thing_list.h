// Liam Wynn, 7/18/2019, Spaceman Engine Level Loading

#ifndef THING_LIST
#define THING_LIST

struct recipe;
struct recipe_list;

struct thinglist_data {
	char* sprite_sheet;

	int anim_class;
	int x, y;
	int rot;
};

struct thinglist_node {
	struct thinglist_data* data;
	struct thinglist_node* next;
};

struct thing_list {
	struct thinglist_node* head;
};

struct thinglist_data* construct_thinglist_data(const char* sprite_sheet, int anim_class, int x, int y, int rot);
int thinglist_data_equals(struct thinglist_data* a, struct thinglist_data* b);
void clean_thinglist_data(struct thinglist_data* to_clean);
void print_thinglist_data(struct thinglist_data* to_print);

struct thinglist_data* thinglist_data_from_recipe(struct recipe* recipe);

struct thing_list* construct_thing_list();
int insert_data_into_thing_list(struct thing_list* list, struct thinglist_data* data);
int thing_list_contains(struct thing_list* list, struct thinglist_data* to_find);
void clean_thing_list(struct thing_list* to_clean);
void print_thing_list(struct thing_list* list);

struct thing_list* build_thing_list_from_map_tree(struct recipe_list* map_tree);
#endif

