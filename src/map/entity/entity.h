// Liam Wynn, 7/15/2019, Raycore

#ifndef ENTITY
#define ENTITY

#define ENTITY_TYPE_NONE	-1
#define ENTITY_TYPE_EXAMPLE	0

// Forward declaration of a map. This way,
struct mapdef;

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

	// A way of idenfitying this kind of entity.
	int type;
	// A unique identity for this particular entity.
	int id;

	// Although entities do not render, per se, they do
	// exist in the level. This way, we can still interact
	// with entities in some capacity.
	int x, y;
};

#endif

