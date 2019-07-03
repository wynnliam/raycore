// Liam Wynn, 7/10/2018, Raycaster: Map File Loader

/*
	Manages all of the procedures for parsing a map file. I will
	assume all characters except the following are whitespace:
	0, 10, 32 - 127.
*/

#include <stdio.h>
#include <stdlib.h>

#include "map_data.h"

#ifndef MAP_DATA_PARSER
#define MAP_DATA_PARSER

// The parser is looking to read in a recipe's header (basically its name)
#define PARSING_HEADER		0
// The parser has read a recipe header and is now reading its attribute list.
#define PARSING_ATTRIBUTES	1

// At the moment, there is:
// properties, component, and thing.
#define NUM_RECIPE_TYPES	3

// Define the types of recipes we can read in.
#define RTYPE_PROPERTIES	0
#define RTYPE_COMPONENT		1
#define RTYPE_THING			2
#define RTYPE_INVALID		-1

// Defines the type of a given attribute. This will govern how
// we read in a attribute's value.
#define ATYPE_INTEGER	0
#define ATYPE_STRING	1
#define ATYPE_INVALID	2

/*
	The main driver procedure for parsing a map file. This will only return
	the map in a sort of "intermediate" format which I call the map_data format.
	A regular map is a set of arrays for the tile grid, the textures, and things.
	The map_data is more true to the sqm file type in that it stores a list of components,
	which can then be turned into a grid the raycaster can understand.

	I think about it like this: humans can understand the sqm file type. The raycaster understands
	a tile grid, and the map_data is an object that can mediate translating from one to
	another.

	This procedure does the first half of this translation. Given a human readable sqm file, we
	return a map_data struct. This can then be translated into a level the raycaster can
	handle.

	PRECONDITIONS:
		an sqm file is opened and given as a FILE pointer. Note that the parser implicitly checks
		this by doing parsing.

	POSTCONDITIONS:
		This procedure will not modify the overall state of the program. Given an sqm file, it will
		return a map_data struct without leaving the overall program in a different state.

	ARGUMENTS:
		map_file - an sqm file being streamed via a FILE pointer. This is what we will build
		a map_data struct out of.

	RETURNS:
		NULL - if the map_file has any problems.
		a valid map_data struct - if map_file is parsed entirely.
*/
struct map_data* parse_to_map_data(FILE* map_file);

/*
	Resets the state of the parser. Does so by setting the curr_state
	to PARSING_HEADER.

	PRECONDITIONS:
		none

	POSTCONDITIONS:
		Sets curr_state to PARSING_HEADER.
*/
void reset();

/*
	A simple debug procedure that prints all information about
	the state of the parser.
*/
void print_state();

/*
	Returns if c is whitespace or not.

	ARGUMENTS:
		c - a character from a map file we are reading in.

	PRECONDITIONS:
		none, technically. Any character will do. However, it is suggested
		you obtain c from an sqm file.

	POSTCONDITIONS:
		none.

	RETURNS:
		0 if c is not white space. 1 if c is white space.
*/
int is_whitespace(char c);

/*
	Returns if c is the end of a line or not.

	ARGUMENTS:
		c - a character from a map file we are reading in.

	PRECONDITIONS:
		none, technically. Any character will do. However, it is suggested
		you obtain c from an sqm file.

	POSTCONDITIONS:
		none.

	RETURNS:
		1 if c is an end of line character. 0 if c is not an end onf line character.
*/
int is_end_of_line(char c);

/*
	Returns if a given character denotes a comment. We say a comment is one line,
	and starts with a #. As such, we check to see if c is a #. This procedure makes
	more sense in the context of read_comment.

	ARGUMENTS:
		c - a character from a map file we are reading in.

	PRECONDITIONS:
		none, technically. Any character will do. However, it is suggested
		you obtain c from an sqm file.

	POSTCONDITIONS:
		none.
*/
int is_comment(char c);

/*
	Tells us if c is a lower case alphabetic character.
*/
int is_lower_alpha(char c);

/*
	Tells us if c is a valid character for a recipe or attribute.
*/
int is_valid_name_char(char c);

/*
	Given a character c, if it denotes the start of a comment, we read characters
	until we hit an end of line character. In other words, if c denotes the start
	of a comment, we consume the comment and throw it away.

	However, if c did not denote a comment, then we read a potentially important
	piece of information. As such, we "unget" c and return 0. This way, the main
	driver of the parser can properly process that piece of information.

	PRECONDITIONS:
		Loaded an sqm file into memory to be processed. Character c grabbed from map.

	POSTCONDITIONS:
		If c is in fact the start of a comment, then FILE pointer map will be changed
		because we read all characters after c through the next end of line. Otherwise,
		if c was not denoting the start of a comment, then we will unget c from the
		file and reset reading by one character.

	ARGUMENTS:
		c - a character from map that was read in.
		map - the FILE to consume characters from.
*/
int read_comment(char c, FILE* map);

/*
	Main driver procedure for reading important information in the file.
	Basically, something is "important" if it's not a comment or whitespace.
	Based on the curr_state flag, we determine if we are reading a recipe header, or
	a recipe's contents. These are handled by parse_recipe_header and parse_attribute
	respectively.

	PREREQUISITES:
		curr_state has a valid value.
		Map FILE open to read from.

	POSTCONDITIONS:
		curr_state may change if we finish reading the header of a recipe, or reading
		its contents.

	ARGUMENTS:
		c - the first character of some piece of important information. Since we read it in
		in parse_to_map_data prior to calling this routine, we shall pass it in.
		map_file - whom we stream characters from.
		map_data - when we read an attribute, we will place the data in here.

	RETURNS:
		The result of either parse_recipe_header or parse_attribute, or -1 if the
		curr_state has an invalid value.
*/
int read_data(char c, FILE* map_file, struct map_data* map_data);

/*
	Performs parsing logic when we are in the PARSING_HEADER state. Essentially it handles
	all the whitespace and end of line characters and opening curly bracket. Once we read in
	the recipe header, we determine the curr_recipe_type state variable. If this is invalid,
	we terminate the process (return -1). Otherwise we may now read in the list of attributes
	for the recipe.

	PRECONDITIONS:
		-curr_state is PARSING_HEADER
		-map is a valid file.
		-We've read in all the prior whitespace, comments, and end of line characters before
		a recipe name.

	POSTCONDITIONS:
		-curr_state set to PARSING_ATTRIBUTES

	ARGUMENTS:
		c - The first character for the recipe name.
		map_file - the file we read characters from.
		map_data - For adding components/things to.

	RETURNS:
		1 - successfully read in recipe name
		-1 - The parser failed: bad format or the recipe type is invalid.
*/
int parse_recipe_header(char c, FILE* map_file, struct map_data* map_data);

/*
	Set the curr_recipe_type state variable to a number as follows:
	0 - properties
	1 - component
	2 - thing
	-1 - invalid recipe header.

	PRECONDITIONS:
		none

	POSTCONDITIONS:
		sets curr_recipe_type to an integer value corresponding to the
		kind of recipe we are about to read in.
*/
void set_curr_recipe_type(char* recipe_header, struct map_data* map_data);

/*
	Parses a single attribute statement in a recipe. It simply parses a single
	attribute inside a recipe. This COULD mean its finished the entire list, or
	just a single attribute within the list. Depending on the curr_recipe_type
	we will also check that the attributes themselves are valid (please see is_valid_attribute).

	PRECONDITIONS:
		-curr_state is PARSING_ATTRIBUTES.
		-map is a valid file.

	POSTCONDITIONS:
		-curr_state MIGHT be set to PARSING_HEADER.

	ARGUMENTS:
		c - The first character for the recipe name.
		map_file - the file we read characters from.
		map_data - where we store the read in attributes.

	RETURNS:
		1 - successfully read in an attribute
		-1 - The parser failed: bad format or the attribute is not for the current recipe.
*/
int parse_attribute(char c, FILE* map_file, struct map_data* map_data);

/*
	Based on the given attribute name, and the curr_recipe_type, we determine
	the attribute's type. The type is important, since it will govern how we process
	the raw value. If it is a string, for example, not a whole lot needs to be done
	(unless the string value is "none", in which case it's treated as a null string).
	An integer, will require the use of atoi to read in. If the string cannot be converted
	to an integer, the parser will fail.

	Further, there is also a bit of error checking done implicitly. Basically, if I read in
	a certain recipe type, there are only certain attributes I can have. If I read in say a
	sky_tex attribute for a component, this is invalid. In such a case, the curr_attribute_type
	will be set to invalid.

	PRECONDITIONS:
		-curr_state must be PARSING_ATTRIBUTES.
		-curr_recipe_type must not be RTYPE_INVALID.
		-attribute_name must not be NULL.

	POSTCONDITIONS:
		Sets curr_attribute_type.

	ARGUMENTS:
		attribute_name - used in conjunction with curr_recipe_type to find the attribute's type.
		Implicitly handles error where a bad value is given to an attribute.
*/
void set_curr_attribute_type(char* attribute_name);

/*
	Sets the value of a member inside a map_data structure. To do so,
	we take the attribute name and parser state variables and set the appropriate
	member inside the map_data structure.

	PRECONDITIONS:
		-curr_state must be PARSING_ATTRIBUTES
		-curr_recipe_type must not be RTYPE_INVALID
		-curr_attribute_type must not be ATYPE_INVALID
		-arguments must not be NULL.

	POSTCONDITIONS:
		This procedure does not modify the overall state of the application.

	RETURNS:
		1 is all preconditions are met. 0 if otherwise OR the attribute_name does not
		match to anything in the map_data structure.
*/
int set_map_data_val(struct map_data* map_data, char* attribute_name, char* attribute_val);

#endif
