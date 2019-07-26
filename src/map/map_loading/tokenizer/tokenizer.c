// Liam Wynn, 6/11/2019, Spaceman Engine Level Loading

#include "./tokenizer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static FILE* map_file = NULL;

static int open_file_for_tokenizing(const char* file_path);
static int close_token_file();

static int read_next_token(struct token* t);

static struct token_list* construct_token_list();
static int insert_token_into_list(struct token* token, struct token_list* list);

static int is_whitespace(int c);
static int is_end_of_line(int c);
static int is_start_of_comment(int c);

static int is_alphabetic(int c);
static int is_numeric(int c);

static int read_token(struct token* t, char curr);
static int get_token_type(int c);
static int read_token_as_name(struct token* t, char curr);
static int read_token_as_attribute(struct token* t, char curr);
static int read_token_as_single_symbol(struct token* t, char symbol);

struct token_list* get_tokens(const char* file_path) {
	if(!open_file_for_tokenizing(file_path))
		return NULL;

	struct token* curr_token;
	struct token_list* token_list = construct_token_list();

	do {
		curr_token = (struct token*)malloc(sizeof(struct token));

		if(read_next_token(curr_token)) {
			if(curr_token->symbol) {
				insert_token_into_list(curr_token, token_list);
			}
		} else {
			free(curr_token);
			break;
		}
	} while(1);

	close_token_file();

	return token_list;
}

struct token_list* construct_token_list() {
	struct token_list* result;

	result = (struct token_list*)malloc(sizeof(struct token_list));
	result->tail = NULL;

	return result;
}

void print_token_list(struct token_list* list) {
	if(!list || !list->tail)
		return;

	struct token_list_node* head = list->tail->next;
	struct token_list_node* curr = head;

	list->tail->next = NULL;

	while(curr) {
		printf("%d\t%s\n", curr->token->type, curr->token->symbol);
		curr = curr->next;
	}

	list->tail->next = head;
}

int insert_token_into_list(struct token* token, struct token_list* list) {
	if(!token || !list)
		return 0;

	struct token_list_node* curr;

	curr = (struct token_list_node*)malloc(sizeof(struct token_list_node));
	curr->token = token;

	if(!list->tail) {
		curr->next = curr;
		list->tail = curr;
	} else {
		curr->next = list->tail->next;
		list->tail->next = curr;
		list->tail = curr;
	}

	return 1;
}

int clean_token_list(struct token_list* list) {
	if(!list)
		return 0;

	if(!list->tail) {
		free(list);
		return 1;
	}

	struct token_list_node* curr;
	struct token_list_node* temp;

	curr = list->tail->next;
	temp = NULL;

	list->tail->next = NULL;

	while(curr) {
		temp = curr->next;

		if(curr->token) {
			if(curr->token->symbol)
				free(curr->token->symbol);

			free(curr->token);
		}

		free(curr);
		curr = temp;
	}

	free(list);

	return 1;
}

int open_file_for_tokenizing(const char* file_path) {
	if(!file_path || map_file)
		return 0;

	map_file = fopen(file_path, "r");

	if(!map_file)
		return 0;

	return 1;
}

int close_token_file() {
	if(!map_file)
		return 0;

	fclose(map_file);
	map_file = NULL;

	return 1;
}

int read_next_token(struct token* t) {
	if(!t)
		return 0;

	char curr;

	// This loop is neccessary since it will throw out multiple
	// junk symbols at a time (ie: comments and whitespace).
	// When we read a proper token, we break this loop.
	do {
		curr = getc(map_file);

		if(curr == EOF) {
			t->symbol = NULL;
			t->type = TOKEN_NONE;
			return 0;
		}

		// In this case, we discard the character.
		if(is_whitespace(curr) || is_end_of_line(curr))
			continue;
		// In this case, we discard the entire line
		else if(is_start_of_comment(curr))
			while((curr = getc(map_file)) != '\n');
		// We are reading a meaningful token, so handle it accordingly.
		else
			return read_token(t, curr);
	} while(1);

	return 1;
}

int is_whitespace(int c) {
	return (c != 0 && c != 10 && (c < 33 || c > 127));
}

int is_end_of_line(int c) {
	return c == '\n';
}

int is_start_of_comment(int c) {
	return c == '#';
}

int is_alphabetic(int c) {
	return (c >= 97 && c <= 122) || (c >= 65 && c <= 90);
}

int is_numeric(int c) {
	return (c >= 48 && c <= 57);
}

static int read_token(struct token* t, char curr) {
	if(!t)
		return 0;

	t->type = get_token_type(curr);
	switch(t->type) {
		case TOKEN_NAME:
			return read_token_as_name(t, curr);
		case TOKEN_ATTR:
			return read_token_as_attribute(t, curr);
		case TOKEN_OPEN_CURL:
			return read_token_as_single_symbol(t, '{');
		case TOKEN_CLOSE_CURL:
			return read_token_as_single_symbol(t, '}');
		case TOKEN_EQUAL:
			return read_token_as_single_symbol(t, '=');
		case TOKEN_SEMI:
			return read_token_as_single_symbol(t, ';');
		default:
			t->type = TOKEN_NONE;
			t->symbol = NULL;
			return 0;
	}
}

static int get_token_type(int c) {
	if(is_alphabetic(c) || c == '_' || is_numeric(c))
		return TOKEN_NAME;
	else if(c == '"')
		return TOKEN_ATTR;
	else if(c == '{')
		return TOKEN_OPEN_CURL;
	else if(c == '}')
		return TOKEN_CLOSE_CURL;
	else if(c == '=')
		return TOKEN_EQUAL;
	else if(c == ';')
		return TOKEN_SEMI;
	else
		return TOKEN_NONE;
}

static int read_token_as_name(struct token* t, char curr) {
	if(!t)
		return 0;

	char buffer[10000];
	unsigned int buffer_index;

	buffer_index = 0;
	while(is_alphabetic(curr) || curr == '_' || is_numeric(curr)) {
		buffer[buffer_index] = curr;
		buffer_index++;
		curr = getc(map_file);
	}

	buffer[buffer_index] = '\0';
	t->symbol = (char*)malloc(strlen(buffer) + 1);
	strcpy(t->symbol, buffer);

	return 1;
}

static int read_token_as_attribute(struct token* t, char curr) {
	if(!t)
		return 0;

	char buffer[10000];
	unsigned int buffer_index;

	// Could also put curr in place of ", but they are the same.
	buffer[0] = '"';
	buffer_index = 1;

	do {
		curr = getc(map_file);
		buffer[buffer_index] = curr;
		buffer_index++;
	} while(curr != '"');

	buffer[buffer_index] = '\0';
	t->symbol = (char*)malloc(strlen(buffer) + 1);
	strcpy(t->symbol, buffer);

	return 1;
}

static int read_token_as_single_symbol(struct token* t, char symbol) {
	if(!t)
		return 0;

	char token[2];

	token[0] = symbol;
	token[1] = '\0';

	t->symbol = (char*)malloc(strlen(token) + 1);
	strcpy(t->symbol, token);

	return 1;
}
