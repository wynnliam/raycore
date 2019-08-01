// Liam Wynn, 8/1/2019, Raycore

#ifndef MAP_ENTITY_LOADING
#define MAP_ENTITY_LOADING

struct entity;
struct recipe;

struct entity* construct_entity_from_recipe(struct recipe* recipe);
#endif
