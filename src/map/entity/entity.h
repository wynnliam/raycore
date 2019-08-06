// Liam Wynn, 7/15/2019, Raycore

#ifndef ENTITY
#define ENTITY

#define ENTITY_TYPE_NONE				-1
#define ENTITY_TYPE_EXAMPLE				0
#define ENTITY_TYPE_LEVEL_TRANSITION	1

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

	// The tiles themselves are the positions of the actual tile.
	unsigned int child_tiles[CHILD_TILE_COUNT];
	// If the map is sufficiently large, any value >= 0 is a valid child.
	// As such, we have a corresponding flag that tells us if the child
	// is "active" (not 0) or not (0).
	unsigned int is_child_tile_active[CHILD_TILE_COUNT];

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

struct entity* construct_empty_entity();

// Child entity CRUD interfaces
int insert_child_entity(struct entity* insert_into, struct entity* child);
int remove_child_entity(struct entity* remove_from, int child_id);
struct entity* find_child_entity(struct entity* find_in, int child_id);
void clear_child_entities(struct entity* to_clear);

// Child thing CRUD interfaces
int insert_child_thing(struct entity* insert_into, struct thingdef* child);
int remove_child_thing(struct entity* remove_from, int child_id);
struct thingdef* find_child_thing(struct entity* find_in, int child_id);
void clear_child_things(struct entity* to_clear);

// Child tile CRUD interfaces
int insert_child_tile(struct entity* insert_into, unsigned int tile_pos);
void clear_child_tiles(struct entity* to_clear);
// For entities and things:
// Insert, Remove, Find, Clear
// For tiles:
// Insert, Find, Clear

#endif

