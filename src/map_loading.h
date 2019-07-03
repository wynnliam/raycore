// Liam Wynn, 4/17/2019, Raycore

#ifndef MAP_LOADING
#define MAP_LOADING

/*
	Initializes the level lookup table. It does so by
	loading load.conf (assests/maps/load.conf) into
	map_lookup_table.

	PRECONDITIONS:
		None

	POSTCONDITIONS:
		map_lookup_table is populated according to load.conf

	RETURNS:
		0 - load.conf was not found or map_lookup_table already initialized.
		1 - map_lookup_table is populated.
*/
int initialize_map_lookup_table();

char* do_map_lookup(unsigned int index);

unsigned int get_num_loaded_maps();

#endif
