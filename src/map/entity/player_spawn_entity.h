// Liam Wynn, 8/6/2019, Raycore

#ifndef ENTITY_PLAYER_SPAWN
#define ENTITY_PLAYER_SPAWN

struct entity;
struct mapdef;

struct player_spawn_entity {
	// Specify where the player is spawned
	// and what direction they will face.
	int player_x, player_y;
	int player_rot;
};

struct entity* construct_entity_player_spawn();

void entity_player_spawn_initialize(struct entity* entity, struct mapdef* curr_map);
void entity_player_spawn_update(struct entity* entity, struct mapdef* curr_map);
void entity_player_spawn_clean(struct entity* entity, struct mapdef* curr_map);

#endif

