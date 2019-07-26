// Liam Wynn, 7/15/2019, Raycore

#ifndef ENTITY_EXAMPLE
#define ENTITY_EXAMPLE

// Forward declarations to keep this as independent
// of everything else as possible.
struct entity;
struct mapdef;

struct entity_example {
	int counter;
};

struct entity* construct_entity_example();

void entity_example_initialize(struct entity* entity, struct mapdef* curr_map);
void entity_example_update(struct entity* entity, struct mapdef* curr_map);
void entity_example_clean(struct entity* entity, struct mapdef* curr_map);
#endif

