#include "parser.h"

// Used to perform string comparisons for recipe header and attributes.
#include <string.h>
// Used to read a datatype value as integer
#include <stdlib.h>

static int curr_state;
// Only set to non-negative value if we've read a recipe type that is valid.
static int curr_recipe_type;
static int curr_attribute_type;

struct map_data* parse_to_map_data(FILE* map_file) {
	// What we will return.
	struct map_data* result = (struct map_data*)malloc(sizeof(struct map_data));
	// Used to read in characters from map_file.
	char c;

	result->component_head = NULL;
	result->thing_head = NULL;

	// By default, we do not use fog.
	result->use_fog = 0;
	result->fog_r = 0;
	result->fog_g = 0;
	result->fog_b = 0;

	reset();
	while(1) {
		c = fgetc(map_file);

		// If the end of the file is reached, break out of the parser.
		if(c == EOF)
			break;

		// If the current character is "junk", just eat it.
		if(is_whitespace(c) || is_end_of_line(c))
			continue;

		// If the current character is not the start of a comment,
		// then do actual parsing. If it is the start of a comment,
		// we just read the entire line.
		if(!read_comment(c, map_file)) {
			// If it was not a comment, then c is the start of something
			// worth parsing. However, if read_comment returns 1, then that
			// means it was a comment. read_comment will read/consume characters
			// until it hits a newline. When it goes to read again, and fails,
			// we will know c is the start of a line worth parsing.
			if(read_data(c, map_file, result) == -1)
				break; // Terminate parser.
		}
	}

	return result;
}

int is_whitespace(char c) {
	return (c != EOF) && (c != 0 && c != 10 && (c < 33 || c > 127));
}

int is_end_of_line(char c) {
	return c == '\n';
}

int is_comment(char c) {
	return c == '#';
}

int is_lower_alpha(char c) {
	return 97 <= c && c <= 122;
}

int is_valid_name_char(char c) {
	return (97 <= c && c <= 122) || (48 <= c && c <= 57) || c == '_';
}

int read_comment(char c, FILE* map) {
	int result;

	// If it is a comment, read until the end of the line.
	if(is_comment(c)) {
		while(c != '\n')
			c = fgetc(map);

		ungetc(c, map);
		result = 1;
	}

	else {
		result = 0;
	}

	return result;
}

int read_data(char c, FILE* map_file, struct map_data* map_data) {

	if(curr_state == PARSING_HEADER)
		return parse_recipe_header(c, map_file, map_data);
	if(curr_state == PARSING_ATTRIBUTES)
		return parse_attribute(c, map_file, map_data);

	// The parser is in a bad state.
	return -1;
}

int parse_recipe_header(char c, FILE* map_file, struct map_data* map_data) {
	// Where we will read a value into.
	char buffer[10000];
	// Used to store characters in buffer.
	unsigned int index;

	buffer[0] = c;
	index = 1;

	// We read in the name of the recipe. We do not need to check
	// for any comments, end of line, or whitespace. These are handled
	// by the parser prior to reading in the recipe header. 
	while(is_lower_alpha((c = fgetc(map_file)))) {
		buffer[index] = c;
		++index;
	}

	// We've read in all possible alphabetical characters.
	// After this, we should only encounter whitespace and then a {
	buffer[index] = '\0';
	printf("Reading in recipe %s...\n", buffer);

	// Check to make sure the recipe type is valid
	// TODO: at this point, allocate items in the map_data.
	set_curr_recipe_type(buffer, map_data);
	if(curr_recipe_type == RTYPE_INVALID) {
		printf("ERROR: Invalid recipe type!\n");
		return -1;
	}

	// Processing all whitespace between last lower alpha character in recipe name
	// until curly bracket.
	while(is_whitespace(c) || is_end_of_line(c))
		c = fgetc(map_file);

	// ERROR: Read in invalid character
	if(c != '{') {
		printf("ERROR: invalid character while reading in recipe name!\n");
		return -1;
	}

	// We are now inside a recipe.
	curr_state = PARSING_ATTRIBUTES;

	return 1;
}

void set_curr_recipe_type(char* recipe_header, struct map_data* map_data) {
	int result;

	if(!recipe_header) {
		curr_recipe_type = RTYPE_INVALID;
		return;
	}

	if(strcmp(recipe_header, "properties") == 0)
		result = RTYPE_PROPERTIES;

	else if(strcmp(recipe_header, "component") == 0) {
		add_component(map_data);
		result = RTYPE_COMPONENT;
	}

	else if(strcmp(recipe_header, "thing") == 0) {
		add_thing_data(map_data);
		result = RTYPE_THING;
	}

	else
		result = RTYPE_INVALID;

	curr_recipe_type = result;
}

int parse_attribute(char c, FILE* map_file, struct map_data* map_data) {
	// Store the name and value of the attribute as raw strings.
	char name_buff[1000];
	char val_buff[1000];
	// Used to fill buffer.
	unsigned int index;

	// The parser detects c is a "relevent" character (not whitespace or a comment or end of line).
	// However, it is a closing curly brace, so it denotes the end of the recipe's attributes.
	if(c == '}') {
		curr_state = PARSING_HEADER;
		return 1;
	}

	// When we reach this point, we've already read in
	// all the prior white space behind an attribute.
	// We thus read until we no longer hit an alphabetical character.

	// Do this so that we can read whole attribute in and easily error check
	// afterwards (weird situation when c is a lone invalid symbol.
	ungetc(c, map_file);
	index = 0;

	while(is_valid_name_char(c = fgetc(map_file))) {
		name_buff[index] = c;
		++index;
	}

	// Error check for invalid characters here
	if(c != '=' && !is_whitespace(c) && !is_end_of_line(c)) {
		printf("ERROR: Invalid symbol %c when reading attribute name!\n", c);
		return -1;
	}

	name_buff[index] = '\0';

	set_curr_attribute_type(name_buff);
	if(curr_attribute_type == ATYPE_INVALID) {
		printf("ERROR: Invalid attribute for recipe!\n");
		return -1;
	}

	// At this point, we read zero or more whitespace and end of line characters.
	// We read until we hit the equals sign. This will have us move in to read the
	// attribute's value.
	while(c != '=') {
		if(is_whitespace(c) || is_end_of_line(c))
			c = fgetc(map_file);
		// Invalid symbol found!
		else {
			printf("ERROR: Invalid symbol %c found while reading attribute value\n", c);
			return -1;
		}
	}

	// Now we have read the =. Next we will have zero or more white space or end of lines
	// until we hit a first ".
	c = fgetc(map_file);
	while(c != '"') {
		if(is_whitespace(c) || is_end_of_line(c))
			c = fgetc(map_file);
		// Invalid symbol found!
		else {
			printf("ERROR: Invalid symbol %c found while reading attribute value\n", c);
			return -1;
		}
	}

	// Now we may read the value for the attribute. We go until we've hit a second pair
	// of quotations.

	// Reset the index for reading.
	index = 0;
	while((c = fgetc(map_file)) != '"') {
		// All characters except end of line are valid here.
		if(is_end_of_line(c)) {
			printf("ERROR: Invalid symbol %c found while reading attribute value\n", c);
			return -1;
		}

		val_buff[index] = c;
		++index;	
	}

	val_buff[index] = '\0';

	if(!set_map_data_val(map_data, name_buff, val_buff)) {
		printf("ERROR: Invalid recipe or attribute\n");
		return -1;
	}

	// Finally, we finish off the attribute by reading the semicolon.
	while((c = fgetc(map_file)) != ';') {
		if(is_whitespace(c) || is_end_of_line(c))
			continue;
		// Invalid symbol found!
		else {
			printf("ERROR: Invalid symbol %c found while reading attribute value\n", c);
			return -1;
		}
	}

	return 1;
}

void set_curr_attribute_type(char* attribute_name) {
	if(!attribute_name || curr_state == PARSING_HEADER || curr_recipe_type == RTYPE_INVALID) {
		curr_attribute_type = ATYPE_INVALID;
	}

	if(curr_recipe_type == RTYPE_PROPERTIES) {
		if(strcmp(attribute_name, "name") == 0)
			curr_attribute_type = ATYPE_STRING;
		else if(strcmp(attribute_name, "sky_tex") == 0)
			curr_attribute_type = ATYPE_STRING;
		else if(strcmp(attribute_name, "use_fog") == 0)
			curr_attribute_type = ATYPE_INTEGER;
		else if(strcmp(attribute_name, "fog_r") == 0)
			curr_attribute_type = ATYPE_INTEGER;
		else if(strcmp(attribute_name, "fog_g") == 0)
			curr_attribute_type = ATYPE_INTEGER;
		else if(strcmp(attribute_name, "fog_b") == 0)
			curr_attribute_type = ATYPE_INTEGER;
		else
			curr_attribute_type = ATYPE_INVALID;
	}

	else if(curr_recipe_type == RTYPE_COMPONENT) {
		if(strcmp(attribute_name, "x") == 0)
			curr_attribute_type = ATYPE_INTEGER;
		else if(strcmp(attribute_name, "y") == 0)
			curr_attribute_type = ATYPE_INTEGER;
		else if(strcmp(attribute_name, "w") == 0)
			curr_attribute_type = ATYPE_INTEGER;
		else if(strcmp(attribute_name, "h") == 0)
			curr_attribute_type = ATYPE_INTEGER;
		else if(strcmp(attribute_name, "is_floor_ciel") == 0)
			curr_attribute_type = ATYPE_INTEGER;
		else if(strcmp(attribute_name, "invisible_wall") == 0)
			curr_attribute_type = ATYPE_INTEGER;
		else if(strcmp(attribute_name, "tex_0") == 0)
			curr_attribute_type = ATYPE_STRING;
		else if(strcmp(attribute_name, "tex_1") == 0)
			curr_attribute_type = ATYPE_STRING;
		else
			curr_attribute_type = ATYPE_INVALID;
	}

	else {
		if(strcmp(attribute_name, "type") == 0)
			curr_attribute_type = ATYPE_INTEGER;
		else if(strcmp(attribute_name, "x") == 0)
			curr_attribute_type = ATYPE_INTEGER;
		else if(strcmp(attribute_name, "y") == 0)
			curr_attribute_type = ATYPE_INTEGER;
		else if(strcmp(attribute_name, "rot") == 0)
			curr_attribute_type = ATYPE_INTEGER;
		else if(strcmp(attribute_name, "anim_class") == 0)
			curr_attribute_type = ATYPE_INTEGER;
		else if(strcmp(attribute_name, "sprite_sheet") == 0)
			curr_attribute_type = ATYPE_STRING;
		else
			curr_attribute_type = ATYPE_INVALID;
	}
}

int set_map_data_val(struct map_data* map_data, char* attribute_name, char* attribute_val) {
	// Perform the error checking.
	if(!map_data || !attribute_name || !attribute_val)
		return 0;
	if(curr_state != PARSING_ATTRIBUTES)
		return 0;
	if(curr_recipe_type == RTYPE_INVALID || curr_attribute_type == ATYPE_INVALID)
		return 0;

	// What we will return.
	int result;
	// If the attribute value has to be an integer, we use this to set it.
	int attrib_val_as_int;

	if(curr_attribute_type == ATYPE_INTEGER)
		attrib_val_as_int = atoi(attribute_val);

	// Go through each possible attribute and set the appropriate one.
	if(curr_recipe_type == RTYPE_PROPERTIES) {
		if(strcmp(attribute_name, "name") == 0) {
			map_data->name = (char*)malloc(strlen(attribute_val) + 1);
			strcpy(map_data->name, attribute_val);
			result = 1;
		}

		else if(strcmp(attribute_name, "sky_tex") == 0) {
			map_data->sky_tex = (char*)malloc(strlen(attribute_val) + 1);
			strcpy(map_data->sky_tex, attribute_val);
			result = 1;
		}

		else if(strcmp(attribute_name, "use_fog") == 0) {
			map_data->use_fog = attrib_val_as_int;
			result = 1;
		}

		else if(strcmp(attribute_name, "fog_r") == 0) {
			map_data->fog_r = attrib_val_as_int;
			result = 1;
		}

		else if(strcmp(attribute_name, "fog_g") == 0) {
			map_data->fog_g = attrib_val_as_int;
			result = 1;
		}

		else if(strcmp(attribute_name, "fog_b") == 0) {
			map_data->fog_b = attrib_val_as_int;
			result = 1;
		}

		else
			result = 0;
	}

	else if(curr_recipe_type == RTYPE_COMPONENT) {
		if(strcmp(attribute_name, "x") == 0) {
			map_data->component_head->x = attrib_val_as_int;
			result = 1;
		}

		else if(strcmp(attribute_name, "y") == 0) {
			map_data->component_head->y = attrib_val_as_int;
			result = 1;
		}

		else if(strcmp(attribute_name, "w") == 0) {
			map_data->component_head->w = attrib_val_as_int;
			result = 1;
		}

		else if(strcmp(attribute_name, "h") == 0) {
			map_data->component_head->h = attrib_val_as_int;
			result = 1;
		}

		else if(strcmp(attribute_name, "is_floor_ciel") == 0) {
			map_data->component_head->is_floor_ceil = attrib_val_as_int;
			result = 1;
		}

		else if(strcmp(attribute_name, "invisible_wall") == 0) {
			map_data->component_head->invisible_wall = attrib_val_as_int;
			result = 1;
		}

		else if(strcmp(attribute_name, "tex_0") == 0) {
			map_data->component_head->tex_0 = (char*)malloc(strlen(attribute_val) + 1);
			strcpy(map_data->component_head->tex_0, attribute_val);
			result = 1;
		}

		else if(strcmp(attribute_name, "tex_1") == 0) {
			map_data->component_head->tex_1 = (char*)malloc(strlen(attribute_val) + 1);
			strcpy(map_data->component_head->tex_1, attribute_val);
			result = 1;
		}

		else
			result = 0;
	}

	else if(curr_recipe_type == RTYPE_THING) {
		if(strcmp(attribute_name, "type") == 0) {
			map_data->thing_head->type = attrib_val_as_int;
			result = 1;
		}

		else if(strcmp(attribute_name, "x") == 0) {
			map_data->thing_head->x = attrib_val_as_int;
			result = 1;
		}

		else if(strcmp(attribute_name, "y") == 0) {
			map_data->thing_head->y = attrib_val_as_int;
			result = 1;
		}

		else if(strcmp(attribute_name, "rot") == 0) {
			map_data->thing_head->rot = attrib_val_as_int;
			result = 1;
		}

		else if(strcmp(attribute_name, "anim_class") == 0) {
			map_data->thing_head->anim_class = attrib_val_as_int;
			result = 1;
		}

		else if(strcmp(attribute_name, "sprite_sheet") == 0) {
			map_data->thing_head->sprite_sheet = (char*)malloc(strlen(attribute_val) + 1);
			strcpy(map_data->thing_head->sprite_sheet, attribute_val);
			result = 1;
		}

		else
			result = 0;
	}

	else
		result = 0;

	return result;
}

void reset() {
	curr_state = PARSING_HEADER;
	curr_recipe_type = RTYPE_INVALID;
	curr_attribute_type = ATYPE_INVALID;
}

void print_state() {
	printf("curr_state = %d\n", curr_state);
}
