// Liam Wynn, 6/7/2019, Spaceman Engine Level Loading

#ifndef MAP_LOAD_PARSER
#define MAP_LOAD_PARSER

#include "./recipe.h"
#include "../tokenizer/tokenizer.h"

struct recipe_list* build_map_tree(struct token_list* tokens);

#endif
