// Liam Wynn, 6/7/2019, Spaceman Engine Level Loading

#include "./parser.h"
#include "../utilities/utilities.h"

#include <stdlib.h>
#include <stdio.h>

// Returns 1 if curr is the start of a recipe name
// (IE: a name followed by a open curly bracket)
static int is_recipe(struct token_list_node* curr);
static int is_recipe_end(struct token_list_node* curr);
static int is_attribute(struct token_list_node* curr);

// Side effect! Will change the value of curr
static struct recipe* build_recipe_from_tokens(struct token_list_node** curr);

struct recipe_list* build_map_tree(struct token_list* tokens) {
	if(!tokens || !tokens->tail)
		return NULL;

	struct recipe_list* result = construct_recipe_list();
	struct recipe* curr_recipe;

	// Begin by breaking the token list into a simple linear linked list.
	struct token_list_node* head = tokens->tail->next;
	// Use this to walk the list.
	struct token_list_node* curr = head;
	tokens->tail->next = NULL;

	while(curr) {
		if(is_recipe(curr)) {
			curr_recipe = build_recipe_from_tokens(&curr);
			insert_recipe_into_recipe_list(curr_recipe, result);
			//curr = curr->next->next;
		} /*else if(is_recipe_end(curr)) {
			printf("%s\n", curr->token->symbol);
			//curr = curr->next;
		} else if(is_attribute(curr)) {
			printf("%s %s %s %s\n", curr->token->symbol,
									curr->next->token->symbol,
									curr->next->next->token->symbol,
									curr->next->next->next->token->symbol);
			//curr = curr->next->next->next->next;
		}*/ else {
			clean_recipe_list(result);
			result = NULL;
			break;
		}
	}

	// Reconnect the list.
	tokens->tail->next = head;

	return result;
}

struct recipe* build_recipe_from_tokens(struct token_list_node** curr) {
	if(!curr)
		return NULL;

	struct recipe* result = construct_recipe(0, (*curr)->token->symbol);

	struct recipe* subrecipe = NULL;
	struct attribute* attribute = NULL;

	// TODO: Finish me!

	// Since we know we are working with a recipe, we can go ahead and move to
	// the next unscanned token.
	*curr = (*curr)->next->next;

	do {
		if(is_recipe_end(*curr)) {
			*curr = (*curr)->next;
			break;
		} else if(is_attribute(*curr)) {
			char* value_no_quotes = remove_quotations_from_string((*curr)->next->next->token->symbol);
			attribute = construct_attribute((*curr)->token->symbol, value_no_quotes);
			free(value_no_quotes);

			insert_attribute_into_recipe(attribute, result);
			*curr = (*curr)->next->next->next->next;
		} else if(is_recipe(*curr)) {
			subrecipe = build_recipe_from_tokens(curr);

			if(!subrecipe) {
				clean_recipe(result);
				return NULL;
			}

			insert_recipe_into_recipe_list(subrecipe, result->subrecipes);
		} else {
			// Error!
			clean_recipe(result);
			return NULL;
		}
	} while(1);

	return result;
}

int is_recipe(struct token_list_node* curr) {
	if(!curr || !curr->next)
		return 0;

	return curr->token->type == TOKEN_NAME && curr->next->token->type == TOKEN_OPEN_CURL;
}

static int is_recipe_end(struct token_list_node* curr) {
	if(!curr)
		return 0;

	return curr->token->type == TOKEN_CLOSE_CURL;
}

static int is_attribute(struct token_list_node* curr) {
	if(!curr || !curr->next || !curr->next->next || !curr->next->next->next)
		return 0;

	return curr->token->type == TOKEN_NAME &&
		   curr->next->token->type == TOKEN_EQUAL &&
		   curr->next->next->token->type == TOKEN_ATTR &&
		   curr->next->next->next->token->type == TOKEN_SEMI;
}
