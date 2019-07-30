// Liam Wynn, 7/15/2019, Raycore

#ifndef ENTITY
#define ENTITY

#define ENTITY_TYPE_NONE	-1
#define ENTITY_TYPE_EXAMPLE	0

#define CHILD_ENTITY_COUNT	1000
// A tile can either be a floorceildef or walldef.
// We define which tiles are children by their position.
#define CHILD_TILE_COUNT	1000
#define CHILD_THING_COUNT	1000

struct mapdef;
struct thingdef;

// TODO: Have children.
struct entity {
	// A general purpose pointer for storing particular entity
	// data.
	void* data;

	// Functions for: upon level creation, upon updating the current frame,
	// and upon cleaning up the level. I am supposing that the function definitions
	// look like this. As I need more information, I shall update them accordingly.
	void (*initialize)(struct entity* entity, struct mapdef* curr_map);
	void (*update)(struct entity* entity, struct mapdef* curr_map);
	void (*clean)(struct entity* entity, struct mapdef* curr_map);

	struct entity* child_entities[CHILD_ENTITY_COUNT];
	struct thingdef* child_things[CHILD_THING_COUNT];
	unsigned int child_tiles[CHILD_TILE_COUNT];

	// A way of idenfitying this kind of entity.
	int type;
	// A unique identity for this particular entity.
	int id;

	// Although entities do not render, per se, they do
	// exist in the level. This way, we can still interact
	// with entities in some capacity.
	int x, y;

	unsigned int num_child_entity;
	unsigned int num_child_tiles;
	unsigned int num_child_things;
};

// Should be called before we attach a data object or functions.
// This ensures our entity is clean for use.
int initialize_entity(struct entity* to_initialize);

#endif

