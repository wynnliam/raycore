// Liam Wynn, 4/17/2019, Spaceman

// TODO: Clean up:
// Loading (if num levels is 0)
// look up
// free space

#include "map_loading.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// And array of every level we want in the game.
char** map_lookup_table;
unsigned int map_lookup_table_size = 0;

int initialize_map_lookup_table() {
	FILE* load_conf = fopen("./assests/maps/load.conf", "r");
	// For reading values into
	char buffer[1000];

	if(!load_conf)
		return 0;
	if(map_lookup_table_size > 0)
		return 0;

	// Read table size:
	fscanf(load_conf, "%d", &map_lookup_table_size);
	printf("Num maps: %d\n", map_lookup_table_size);

	map_lookup_table = (char**)malloc(sizeof(char**) * map_lookup_table_size);

	// Load levels
	unsigned int i;
	for(i = 0; i < map_lookup_table_size; ++i) {
		fscanf(load_conf, "%s", buffer);

		map_lookup_table[i] = (char*)malloc(strlen(buffer) + 1);
		strcpy(map_lookup_table[i], buffer);

		printf("%d: %s\n", i, map_lookup_table[i]);
	}

	fclose(load_conf);

	return 0;
}

char* do_map_lookup(unsigned int index) {
	if(map_lookup_table_size == 0 || index >= map_lookup_table_size)
		return NULL;

	return map_lookup_table[index];
}

unsigned int get_num_loaded_maps() {
	return map_lookup_table_size;
}
