// Liam Wynn, 7/26/2019, Raycore

#include "./map_properties.h"

#include <stdlib.h>

struct ir_map_properties* construct_properties() {
	struct ir_map_properties* result = (struct ir_map_properties*)malloc(sizeof(struct ir_map_properties));

	result->mapdef_name = NULL;

	result->sky_tex = NULL;

	result->use_fog = 0;
	result->fog_r = 0;
	result->fog_g = 0;
	result->fog_b = 0;

	return result;
}

void clean_ir_map_properties(struct ir_map_properties* to_clean) {
	if(!to_clean)
		return;

	if(to_clean->mapdef_name)
		free(to_clean->mapdef_name);
	if(to_clean->sky_tex)
		free(to_clean->sky_tex);

	free(to_clean);
}

