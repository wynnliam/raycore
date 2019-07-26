// Liam Wynn, 6/7/2019, Spaceman Engine Level Loading

#include "./attribute.h"
#include "../utilities/utilities.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ATTRIBUTE FUNCTION IMPLEMENTATIONS */
struct attribute* construct_attribute(const char* name, const char* value) {
	if(!name || !value)
		return NULL;

	struct attribute* result = (struct attribute*)malloc(sizeof(struct attribute));

	result->name = (char*)malloc(strlen(name) + 1);
	strcpy(result->name, name);

	result->value = (char*)malloc(strlen(value) + 1);
	strcpy(result->value, value);

	return result;
}

int print_attribute(const struct attribute* to_print) {
	if(!to_print)
		return 0;

	printf("attribute: %s = %s\n", to_print->name, to_print->value);

	return 1;
}

int clean_attribute(struct attribute* to_clean) {
	if(!to_clean)
		return 0;

	if(!to_clean->name || !to_clean->value)
		return 0;

	free(to_clean->name);
	free(to_clean->value);
	free(to_clean);

	return 1;
}

/* ATTRIBUTE LIST FUNCTION IMPLEMENTATIONS */
struct attribute_list* construct_attribute_list() {
	struct attribute_list* result = (struct attribute_list*)malloc(sizeof(struct attribute_list));

	result->head = NULL;

	return result;
}

int insert_attribute_into_list(struct attribute* attr, struct attribute_list* attr_list) {
	if(!attr || !attr_list)
		return 0;

	struct attribute_list_node* attr_node = (struct attribute_list_node*)malloc(sizeof(struct attribute_list_node));
	attr_node->attribute = attr;
	attr_node->next = attr_list->head;

	attr_list->head = attr_node;

	return 1;
}

char* get_attribute_value(struct attribute_list* list, const char* name) {
	if(!list)
		return NULL;

	char* result = NULL;
	struct attribute_list_node* curr = list->head;

	while(curr) {
		if(strcmp(curr->attribute->name, name) == 0) {
			result = (char*)malloc(strlen(curr->attribute->value) + 1);
			strcpy(result, curr->attribute->value);
			break;
		}

		curr = curr->next;
	}

	return result;
}

int print_attribute_list(struct attribute_list* attr_list, unsigned int num_spaces) {
	if(!attr_list)
		return 0;

	struct attribute_list_node* temp = attr_list->head;

	while(temp) {
		if(temp->attribute) {
			printf("%*s", num_spaces, "");
			print_attribute(temp->attribute);
		}

		temp = temp->next;
	}

	return 1;
}

int clean_attribute_list(struct attribute_list* attr_list) {
	if(!attr_list)
		return 0;

	struct attribute_list_node* curr = attr_list->head;
	struct attribute_list_node* temp = NULL;

	while(curr) {
		temp = curr->next;

		clean_attribute(curr->attribute);
		free(curr);
		curr = temp;
	}

	free(attr_list);

	return 1;
}
