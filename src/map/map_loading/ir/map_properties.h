// Liam Wynn, 7/26/2019, Raycore

#ifndef IR_MAP_PROPERTIES
#define IR_MAP_PROPERTIES

struct recipe;
struct recipe_list;

// An intermediate representation for map properties.
struct ir_map_properties {
	char* mapdef_name;
	char* sky_tex;

	int use_fog;
	int fog_r, fog_g, fog_b;
};

struct ir_map_properties* construct_properties();
struct ir_map_properties* build_ir_map_properties_from_recipe(struct recipe* recipe);
void clean_ir_map_properties(struct ir_map_properties* to_clean);

#endif
