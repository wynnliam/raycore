// Liam Wynn, 8/6/2019, Raycore

#ifndef ENTITY_LEVEL_TRANSITION
#define ENTITY_LEVEL_TRANSITION

struct entity;
struct mapdef;

struct level_transition_entity {
	unsigned int goto_level_id;
	// TODO: Specify spawn.
};

struct entity* construct_entity_level_transition();

void entity_level_transition_initialize(struct entity* entity, struct mapdef* curr_map);
void entity_level_transition_update(struct entity* entity, struct mapdef* curr_map);
void entity_level_transition_clean(struct entity* entity, struct mapdef* curr_map);
#endif

