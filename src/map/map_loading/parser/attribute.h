// Liam Wynn, 6/7/2019, Spaceman Engine Level Loading

struct attribute {
	char* name;
	char* value;
};

struct attribute_list_node {
	struct attribute* attribute;
	struct attribute_list_node* next;
};

// Kind of unneccessary, except that it makes this slightly
// more readable when we pass in lists to list functions and not
// list nodes.
struct attribute_list {
	struct attribute_list_node* head;
};

// ATTRIBUTE FUNCTIONS
struct attribute* construct_attribute(const char* name, const char* value);
int clean_attribute(struct attribute* to_clean);
int print_attribute(const struct attribute* to_print);

// ATTRIBUTE LIST FUNCTIONS
struct attribute_list* construct_attribute_list();
int insert_attribute_into_list(struct attribute* attr, struct attribute_list* attr_list);
char* get_attribute_value(struct attribute_list* list, const char* name);
int clean_attribute_list(struct attribute_list* attr_list);
int print_attribute_list(struct attribute_list* attr_list, unsigned int num_spaces);
