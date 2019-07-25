// Liam Wynn, 6/11/2019, Spaceman Engine Level Loading

#ifndef MAP_LOAD_TOKENIZER
#define MAP_LOAD_TOKENIZER

#define TOKEN_NONE			0
#define TOKEN_NAME			1
#define TOKEN_ATTR			2
#define TOKEN_OPEN_CURL		3
#define TOKEN_CLOSE_CURL	4
#define TOKEN_EQUAL			5
#define TOKEN_SEMI			6

struct token {
	char* symbol;
	int type;
};

struct token_list_node {
	struct token* token;
	struct token_list_node* next;
};

struct token_list {
	struct token_list_node* tail;
};

struct token_list* get_tokens(const char* file_path);
int clean_token_list(struct token_list* list);
void print_token_list(struct token_list* list);

#endif
